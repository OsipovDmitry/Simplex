#include <core/shadow.h>
#include <core/graphicsrendererbase.h>

#include "shadowprivate.h"
#include "shadowbuffer.h"
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

std::shared_ptr<ShadowBuffer>& ShadowPrivate::shadowBuffer()
{
    return m_shadowBuffer;
}

std::shared_ptr<ShadowFrameBuffer> &ShadowPrivate::frameBuffer()
{
    return m_frameBuffer;
}

std::shared_ptr<Blur> &ShadowPrivate::blur()
{
    return m_blur;
}

ShadowPrivate::LayeredMatricesBuffer& ShadowPrivate::layeredMatricesBuffer()
{
    return m_layeredMatricesBuffer;
}

void ShadowPrivate::update(
    const std::shared_ptr<graphics::IFrameBuffer> &frameBuffer,
    const std::vector<glm::mat4x4> &layeredShadowMatrices)
{
    for (size_t i = 0u; i < layeredShadowMatrices.size(); ++i)
        m_layeredMatricesBuffer->set(i, layeredShadowMatrices[i]);

    m_shadowBuffer->update(m_mapSize, m_mode, m_filter);

    // tmp
    m_frameBuffer = std::make_shared<ShadowFrameBuffer>(frameBuffer, m_shadowBuffer);

    if (m_filter == ShadingFilter::VSM)
    {
        if (!m_blur)
            m_blur = std::make_shared<Blur>();
    }
    else
        m_blur = nullptr; // blur is not needed if filter is not VSM
}

}
}
