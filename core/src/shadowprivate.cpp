#include <core/shadow.h>
#include <core/graphicsrendererbase.h>

#include "shadowprivate.h"
#include "framebufferhelpers.h"
#include "blur.h"

namespace simplex
{
namespace core
{

ShadowPrivate::ShadowPrivate()
{
}

ShadowPrivate::~ShadowPrivate() = default;

ShadingMode &ShadowPrivate::mode()
{
    return m_mode;
}

ShadingFilter &ShadowPrivate::filter()
{
    return m_filter;
}

float &ShadowPrivate::depthBias()
{
    return m_depthBias;
}

glm::uvec2 &ShadowPrivate::mapSize()
{
    return m_mapSize;
}

utils::Range &ShadowPrivate::cullPlanesLimits()
{
    return m_cullPlanesLimits;
}

std::shared_ptr<ShadowFrameBuffer> &ShadowPrivate::frameBuffer()
{
    return m_frameBuffer;
}

std::shared_ptr<Blur> &ShadowPrivate::blur()
{
    return m_blur;
}

graphics::PBufferRange &ShadowPrivate::layeredMatricesBuffer()
{
    return m_layeredMatricesBuffer;
}

void ShadowPrivate::update(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer, const std::vector<glm::mat4x4> &layeredShadowMatrices)
{
    if (!m_frameBuffer)
    {
        m_frameBuffer = createShadowFrameBuffer(graphicsRenderer);
        m_frameBuffer->resize(graphicsRenderer, mapSize(), mode(), filter());
    }

    if (!m_layeredMatricesBuffer)
    {
        auto buffer = graphicsRenderer->createBuffer(0u);
        m_layeredMatricesBuffer = graphicsRenderer->createBufferRange(buffer, 0u);
    }

    if (m_filter == ShadingFilter::VSM)
    {
        if (!m_blur)
            m_blur = std::make_shared<Blur>();
    }
    else
        m_blur = nullptr; // blur is not needed if filter is not VSM

    const size_t bufferSize = 4u * sizeof(uint32_t) + layeredShadowMatrices.size() * sizeof(glm::mat4x4);
    m_layeredMatricesBuffer->buffer()->resize(bufferSize, nullptr);
    auto data = m_layeredMatricesBuffer->buffer()->map(graphics::IBuffer::MapAccess::WriteOnly);
    auto p = data->get();
    *reinterpret_cast<uint32_t*>(p) = static_cast<uint32_t>(layeredShadowMatrices.size());
    p += 4u * sizeof(uint32_t);
    for (size_t i = 0u; i < layeredShadowMatrices.size(); ++i)
    {
        *reinterpret_cast<glm::mat4x4*>(p) = layeredShadowMatrices[i];
        p += sizeof(glm::mat4x4);
    }
}

}
}
