#include <utils/glm/common.hpp>

#include <core/graphicsrendererbase.h>

#include "gframebuffer.h"

namespace simplex
{
namespace core
{

GFramebuffer::GFramebuffer(const glm::uvec2& size)
{
    resize(size);
}

const glm::uvec2& GFramebuffer::size() const
{
    return m_size;
}

void GFramebuffer::resize(const glm::uvec2& size)
{
    if (m_size == size)
        return;

    m_size = glm::max(size, glm::uvec2(1u, 1u));

    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
    {
        LOG_CRITICAL << "No current graphics renderer";
        return;
    }

    m_colorTexture0 = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R11F_G11F_B10F);
    m_colorTexture1 = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R11F_G11F_B10F);
    m_colorTexture2 = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA8);
    m_depthTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Depth32F);
    m_stencilTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Stencil8);
    m_OITDepthTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Depth32F);
    m_OITIndicesTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32UI);
}

graphics::PConstTexture GFramebuffer::colorTexture0() const
{
    return m_colorTexture0;
}

graphics::PConstTexture GFramebuffer::colorTexture1() const
{
    return m_colorTexture1;
}

graphics::PConstTexture GFramebuffer::colorTexture2() const
{
    return m_colorTexture2;
}

graphics::PConstTexture GFramebuffer::depthTexture() const
{
    return m_depthTexture;
}

graphics::PConstTexture GFramebuffer::stencilTexture() const
{
    return m_stencilTexture;
}

graphics::PConstTexture GFramebuffer::OITDepthTexture() const
{
    return m_OITDepthTexture;
}

graphics::PConstTexture GFramebuffer::OITIndicesTexture() const
{
    return m_OITIndicesTexture;
}

}
}
