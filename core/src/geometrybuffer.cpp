#include <utils/glm/common.hpp>

#include <core/graphicsrendererbase.h>
#include <core/settings.h>

#include "geometrybuffer.h"

namespace simplex
{
namespace core
{

GeometryBuffer::GeometryBuffer(const glm::uvec2& size)
{
    resize(size);
}

const glm::uvec2& GeometryBuffer::size() const
{
    return m_size;
}

void GeometryBuffer::resize(const glm::uvec2& size)
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
    m_OITDepthTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32F);
    m_OITIndicesTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32UI);
    m_finalTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA16F);

    m_OITNodesBuffer = POITNodesBuffer::element_type::create();
    static const auto& OITSetings = settings::Settings::instance().graphics().oit();
    m_OITNodesBuffer->resize(glm::min(OITSetings.maxNodes(), m_size.x * m_size.y * OITSetings.nodesPerPixel()));
}

void GeometryBuffer::clearOITNodesBuffer()
{
    m_OITNodesBuffer->setReservedData({ settings::Settings::instance().graphics().oit().maxNodes(), 0u });
}

graphics::PConstTexture GeometryBuffer::colorTexture0() const
{
    return m_colorTexture0;
}

graphics::PConstTexture GeometryBuffer::colorTexture1() const
{
    return m_colorTexture1;
}

graphics::PConstTexture GeometryBuffer::colorTexture2() const
{
    return m_colorTexture2;
}

graphics::PConstTexture GeometryBuffer::depthTexture() const
{
    return m_depthTexture;
}

graphics::PConstTexture GeometryBuffer::stencilTexture() const
{
    return m_stencilTexture;
}

graphics::PConstTexture GeometryBuffer::OITDepthTexture() const
{
    return m_OITDepthTexture;
}

graphics::PConstTexture GeometryBuffer::OITIndicesTexture() const
{
    return m_OITIndicesTexture;
}

graphics::PConstTexture GeometryBuffer::finalTexture() const
{
    return m_finalTexture;
}

PConstOITNodesBuffer GeometryBuffer::OITNodesBuffer() const
{
    return m_OITNodesBuffer;
}

}
}
