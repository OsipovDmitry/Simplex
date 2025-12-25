#include <utils/glm/common.hpp>

#include <core/graphicsrendererbase.h>
#include <core/settings.h>

#include "geometrybuffer.h"
#include "uniform.h"

namespace simplex
{
namespace core
{

GeometryBuffer::GeometryBuffer(const glm::uvec2& size)
    : StateSet()
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
    m_depthStencilTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Dept32FStencil8);
    m_OITDepthTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32F);
    m_OITIndicesTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32UI);
    m_finalTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA16F);

    m_OITBuffer = POITBuffer::element_type::create();
    static const auto& OITSetings = settings::Settings::instance().graphics().oit();
    m_OITBuffer->resize(glm::min(OITSetings.maxNodes(), m_size.x * m_size.y * OITSetings.nodesPerPixel()));

    getOrCreateUniform(UniformID::ViewportSize) = makeUniform(m_size);
    getOrCreateUniform(UniformID::GBufferColorMap0) = makeUniform(colorTexture0());
    getOrCreateUniform(UniformID::GBufferDepthMap) = makeUniform(depthStencilTexture());
    getOrCreateUniform(UniformID::OITDepthMap) = makeUniform(OITDepthTexture());
    getOrCreateUniform(UniformID::OITIndicesMap) = makeUniform(OITIndicesTexture());
    getOrCreateUniform(UniformID::GBufferFinalMap) = makeUniform(finalTexture());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OITBuffer) = graphics::BufferRange::create(m_OITBuffer->buffer());
}

graphics::PConstTexture GeometryBuffer::colorTexture0() const
{
    return m_colorTexture0;
}

graphics::PConstTexture GeometryBuffer::depthStencilTexture() const
{
    return m_depthStencilTexture;
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
