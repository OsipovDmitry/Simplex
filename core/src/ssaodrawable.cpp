#include <random>

#include <utils/logger.h>
#include <utils/glm/trigonometric.hpp>
#include <utils/glm/gtc/constants.hpp>

#include <core/settings.h>
#include <core/uniform.h>

#include "ssaodrawable.h"

namespace simplex
{
namespace core
{

SSAODrawable::SSAODrawable(const std::shared_ptr<graphics::IVertexArray> &vao)
    : Drawable(vao)
{
    const auto &SSAOSettings = settings::Settings::instance().graphics().ssao();

    setContribution(SSAOSettings.contribution());
    setKernelBufferSize(SSAOSettings.kernelSize());
    setNoiseTextureSize(SSAOSettings.noiseSize());
    setRadius(SSAOSettings.radius());
}

SSAODrawable::~SSAODrawable() = default;

float SSAODrawable::contribution() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::SSAOContribution));
    return uni ? uni->data() : settings::Settings::instance().graphics().ssao().contribution();
}

void SSAODrawable::setContribution(float value)
{
    if (value < 0.f)
        LOG_CRITICAL << "SSAO contribution can't be less than 0.0";

    getOrCreateUniform(UniformId::SSAOContribution) = makeUniform(value);
}

uint32_t SSAODrawable::kernelBufferSize() const
{
    return m_kernelBufferSize;
}

void SSAODrawable::setKernelBufferSize(uint32_t value)
{
    m_kernelBufferSize = value;
    setKernelBuffer(nullptr); // reset to update
}

uint32_t SSAODrawable::noiseTextureSize() const
{
    return m_noiseTextureSize;
}

void SSAODrawable::setNoiseTextureSize(uint32_t value)
{
    m_noiseTextureSize = value;
    setNoiseTexture(nullptr); // reset to update
}

float SSAODrawable::radius() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::SSAORadius));
    return uni ? uni->data() : settings::Settings::instance().graphics().ssao().radius();
}

void SSAODrawable::setRadius(float value)
{
    if (value <= 0.f)
        LOG_CRITICAL << "SSAO radius must be greater than 0.0";

    getOrCreateUniform(UniformId::SSAORadius) = makeUniform(value);
}

void SSAODrawable::update(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
{
    if (!kernelBuffer())
    {
        std::uniform_real_distribution<float> randomFloats(0.f, 1.f);
        std::default_random_engine generator;

        const auto numSamples = kernelBufferSize();
        const size_t bufferSize = 4u * sizeof(uint32_t) + numSamples * sizeof(glm::vec4);
        auto kernelBuffer = graphicsRenderer->createBufferRange(graphicsRenderer->createBuffer(bufferSize), 0u);
        auto data = kernelBuffer->buffer()->map(graphics::IBuffer::MapAccess::WriteOnly);
        auto p = data->get();
        *reinterpret_cast<uint32_t*>(p) = numSamples;
        p += 4u * sizeof(uint32_t);
        for (uint32_t i = 0u; i < numSamples; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.f - 1.f,
                             randomFloats(generator) * 2.f - 1.f,
                             randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            const float scale = static_cast<float>(i+1u) / static_cast<float>(numSamples);
            sample *= glm::mix(0.f, 1.f, scale * scale);

            *reinterpret_cast<glm::vec4*>(p) = glm::vec4(sample, 1.f);
            p += sizeof(glm::vec4);
        }

        setKernelBuffer(kernelBuffer);
    }

    if (!noiseTexture())
    {
        const auto textureSize = noiseTextureSize();
        std::vector<glm::vec2> data(textureSize * textureSize);

        std::uniform_real_distribution<float> randomFloats(0.f, glm::two_pi<float>());
        std::default_random_engine generator;
        for (size_t i = 0u; i < data.size(); ++i)
        {
            const float angle = randomFloats(generator);
            data[i] = glm::vec2(glm::cos(angle), glm::sin(angle));
        }

        const auto noiseImage = utils::Image::loadFromData(textureSize, textureSize, 2u, utils::PixelComponentType::Single, data.data());
        setNoiseTexture(graphicsRenderer->createTextureRect(noiseImage));
    }
}

graphics::PConstBufferRange SSAODrawable::kernelBuffer() const
{
    return SSBO(SSBOId::SSAOKernel);
}

void SSAODrawable::setKernelBuffer(const graphics::PConstBufferRange &value)
{
    static SSBOId s_SSBOId = SSBOId::SSAOKernel;

    if (value)
        getOrCreateSSBO(s_SSBOId) = value;
    else
        removeSSBO(s_SSBOId);
}

graphics::PConstTexture SSAODrawable::noiseTexture() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::SSAONoiseMap));
    return uni ? uni->data() : nullptr;
}

void SSAODrawable::setNoiseTexture(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::SSAONoiseMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

}
}
