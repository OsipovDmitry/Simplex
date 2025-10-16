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

    m_colorTexture0 = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA32UI);
    m_depthTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Depth32F);
    m_stencilTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Stencil8);
    m_OITDepthTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32F);
    m_OITIndicesTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32UI);
    m_finalTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA16F);

    m_OITBuffer = POITBuffer::element_type::create();
    static const auto& OITSetings = settings::Settings::instance().graphics().oit();
    auto maxNumNodes = glm::min(OITSetings.maxNodes(), m_size.x * m_size.y * OITSetings.nodesPerPixel());
    m_OITBuffer->resize(maxNumNodes);
    m_OITBuffer->setReservedData({ maxNumNodes, 0u });
}

void GeometryBuffer::clearOITBuffer()
{
    auto reserverdData = m_OITBuffer->reservedData();
    reserverdData.nodesCount = 0u;
    m_OITBuffer->setReservedData(reserverdData);
}

graphics::PConstTexture GeometryBuffer::colorTexture0() const
{
    return m_colorTexture0;
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

PConstOITBuffer GeometryBuffer::OITBuffer() const
{
    return m_OITBuffer;
}

}
}
