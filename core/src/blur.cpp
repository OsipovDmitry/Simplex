#include <utils/glm/gtx/functions.hpp>
#include <utils/logger.h>

#include <core/settings.h>
#include <core/uniform.h>
#include <core/programsmanager.h>

#include "blur.h"

namespace simplex
{
namespace core
{

Blur::Blur()
{
    setSigma(settings::Settings::instance().graphics().blur().sigma());
    setPassIndex(0u);
    setSourceMap(nullptr, 0u);
}

Blur::~Blur() = default;

float Blur::sigma() const
{
    return m_sigma;
}

void Blur::setSigma(float value)
{
    if (value <= 0.f)
        LOG_CRITICAL << "Sigma must be greater than 0.0";

    m_sigma = value;
    setKernelBuffer(nullptr); // reset to update
}

graphics::PConstTexture Blur::sourceMap() const
{
    return m_pingpongSourceMap[0u].first;
}

uint32_t Blur::sourceMapLevel() const
{
    return m_pingpongSourceMap[0u].second;
}

void Blur::setSourceMap(const graphics::PConstTexture &map, uint32_t level, bool canDirtyMap)
{
    if (!map)
    {
        for (auto &map : m_pingpongSourceMap)
            map = {nullptr, 0u};
        return;
    }

    m_pingpongSourceMap[0u].first = canDirtyMap ? map : map->copy();
    m_pingpongSourceMap[0u].second = level;

    m_pingpongSourceMap[1u].first = map->copyEmpty();
    m_pingpongSourceMap[1u].second = level;
}

graphics::PConstTexture Blur::run(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer,
                                  const std::shared_ptr<ProgramsManager> &programsManager)
{
    if (!kernelBuffer())
    {
        static uint32_t s_maxKernelRadius = 64u;
        std::vector<glm::vec4> kernel;
        kernel.reserve(s_maxKernelRadius/4u);

        uint32_t radius = 0u;
        for (; radius < s_maxKernelRadius; ++radius)
        {
            const float sample = glm::gauss(static_cast<float>(radius), 0.0f, m_sigma);
            if (sample < 1e-2f)
                break;

            if (!(radius % 4u))
                kernel.push_back(glm::vec4(0.f));

            kernel[radius / 4u][radius % 4u] = sample;
        }

        if (radius == 0u)
            LOG_CRITICAL << "Radius of blur kernal can't be 0";

        float summ = kernel[0u][0u];
        for (uint32_t r = 1u; r < radius; ++r)
            summ += 2.f * kernel[r / 4u][r % 4u];

        for (auto &samples : kernel)
            samples /= summ;

        const size_t bufferSize = 4u * sizeof(uint32_t) + kernel.size() * sizeof(glm::vec4);
        auto kernelBuffer = graphicsRenderer->createBuffer(bufferSize);
        auto data = kernelBuffer->map(graphics::IBuffer::MapAccess::WriteOnly);
        auto p = data->get();
        *reinterpret_cast<uint32_t*>(p) = radius;
        memcpy(p + 4u * sizeof(uint32_t), kernel.data(), kernel.size() * sizeof(glm::vec4));

        setKernelBuffer(kernelBuffer);
    }

    checkMapsInvariants();

    const auto textureType = m_pingpongSourceMap[0u].first->type();
    const auto textureInternalFormat = m_pingpongSourceMap[0u].first->internalFormat();
    auto program = programsManager->loadOrGetBlurComputeProgram(textureType, textureInternalFormat);

    uint32_t numPasses = 0u;
    switch (textureType)
    {
    case graphics::TextureType::Type1D:
    case graphics::TextureType::Type1DArray: numPasses = 1u; break;
    case graphics::TextureType::Type2D:
    case graphics::TextureType::Type2DArray:
    case graphics::TextureType::TypeCube:
    case graphics::TextureType::TypeCubeArray:
    case graphics::TextureType::TypeRect: numPasses = 2u; break;
    case graphics::TextureType::Type3D: numPasses = 3u; break;
    default: break;
    }

    auto size = m_pingpongSourceMap[0u].first->mipmapSize(m_pingpongSourceMap[0u].second);
    size.z *= m_pingpongSourceMap[0u].first->numFaces();

    for (uint32_t pass = 0u; pass < numPasses; ++pass)
    {
        const auto sourceIdx = pass % 2u;
        const auto destinationIdx = 1u - sourceIdx;

        setSourceImage(graphicsRenderer->createImage(graphics::IImage::DataAccess::ReadOnly,
                                                     m_pingpongSourceMap[sourceIdx].first,
                                                     m_pingpongSourceMap[sourceIdx].second));
        setDestinationImage(graphicsRenderer->createImage(graphics::IImage::DataAccess::WriteOnly,
                                                          m_pingpongSourceMap[destinationIdx].first,
                                                          m_pingpongSourceMap[destinationIdx].second));
        setPassIndex(pass);

        graphicsRenderer->compute(program, size, shared_from_this());
    }

    return m_pingpongSourceMap[numPasses % 2u].first;
}

graphics::PConstBuffer Blur::kernelBuffer()
{
    return SSBO(SSBOId::BlurKernel);
}

void Blur::setKernelBuffer(const graphics::PConstBuffer &value)
{
    static SSBOId s_SSBOId = SSBOId::BlurKernel;

    if (value)
        getOrCreateSSBO(s_SSBOId) = value;
    else
        removeSSBO(s_SSBOId);
}

graphics::PConstImage Blur::sourceImage() const
{
    auto uni = uniform_cast<graphics::PConstImage>(uniform(UniformId::SourceImage));
    return uni ? uni->data() : nullptr;
}

void Blur::setSourceImage(const graphics::PConstImage &value)
{
    static UniformId s_uniformId = UniformId::SourceImage;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

graphics::PConstImage Blur::destinationImage() const
{
    auto uni = uniform_cast<graphics::PConstImage>(uniform(UniformId::DestinationImage));
    return uni ? uni->data() : nullptr;
}

void Blur::setDestinationImage(const graphics::PConstImage &value)
{
    static UniformId s_uniformId = UniformId::DestinationImage;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

void Blur::setPassIndex(uint32_t value)
{
    getOrCreateUniform(UniformId::BlurPassIndex) = makeUniform(value);
}

void Blur::checkMapsInvariants() const
{
    const auto &sorceMap0 = m_pingpongSourceMap[0u].first;
    const auto &sorceMap1 = m_pingpongSourceMap[1u].first;

    if (!sorceMap0)
        LOG_CRITICAL << "Source map 0 can't be nullptr";

    if (!sorceMap1)
        LOG_CRITICAL << "Source map 1 can't be nullptr";

    if (sorceMap0->type() != sorceMap1->type())
        LOG_CRITICAL << "Maps can't be different types";

    if (sorceMap0->internalFormat() != sorceMap1->internalFormat())
        LOG_CRITICAL << "Maps can't have different internal format";

    if (sorceMap0->mipmapSize(m_pingpongSourceMap[0u].second) != sorceMap1->mipmapSize(m_pingpongSourceMap[1u].second))
        LOG_CRITICAL << "Maps can't have different sizes";
}

}
}
