#include <utils/glm/common.hpp>

#include <core/graphicsrendererbase.h>
#include <core/programsmanager.h>
#include <core/settings.h>

#include "geometrybuffer.h"
#include "resources.h"
#include "uniform.h"

namespace simplex
{
namespace core
{

GeometryBuffer::GeometryBuffer(const glm::uvec2& size)
    : StateSet()
    , m_size(0u)
    , m_isInitialized(false)

{
}

const glm::uvec2& GeometryBuffer::size() const
{
    return m_size;
}

void GeometryBuffer::resize(const glm::uvec2& size, const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
{
    if (m_size == size)
        return;

    m_size = glm::max(size, glm::uvec2(1u, 1u));

    m_colorTextureHandle = graphicsRenderer->createTextureHandle(
        graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA32UI));
    m_colorTextureHandle->makeResident();

    m_depthStencilTextureHandle = graphicsRenderer->createTextureHandle(
        graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Dept32FStencil8));
    m_depthStencilTextureHandle->makeResident();

    m_OITNodeIDImageHandle = graphicsRenderer->createImageHandle(
        graphics::Image::create(
            graphics::Image::DataAccess::ReadWrite,
            graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32UI)));
    m_OITNodeIDImageHandle->makeResident();

    m_finalTextureHandle = graphicsRenderer->createTextureHandle(
        graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA16F));
    m_finalTextureHandle->makeResident();

    static const auto& OITSetings = settings::Settings::instance().graphics().oit();
    auto OITBufferSize = glm::min(OITSetings.maxNodes(), m_size.x * m_size.y * OITSetings.nodesPerPixel());

    m_GBuffer = PGBuffer::element_type::create(GBufferDescription::make(
        m_colorTextureHandle->handle(),
        m_depthStencilTextureHandle->handle(),
        m_OITNodeIDImageHandle->handle(),
        m_finalTextureHandle->handle(),
        m_size,
        OITBufferSize));

    m_OITBuffer = POITBuffer::element_type::create();
    m_OITBuffer->resize(OITBufferSize);

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::GBuffer) = graphics::BufferRange::create(m_GBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OITNodesBuffer) = graphics::BufferRange::create(m_OITBuffer->buffer());
}

void GeometryBuffer::initialize(const std::shared_ptr<ProgramsManager>& programsManager)
{
    if (m_isInitialized)
        return;

    m_clearOITNodeIDImageProgram = programsManager->loadOrGetComputeProgram(
        resources::ClearOITNodeIDImagePassComputeShaderPath,
        {});

    m_sortOITNodesProgram = programsManager->loadOrGetComputeProgram(
        resources::SortOITNodesPassComputeShaderPath,
        {});
}

void GeometryBuffer::clear(
    const std::shared_ptr<graphics::RendererBase>& graphicsRenderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer) const
{
    m_GBuffer->setField(offsetof(GBufferDescription, OITNodesCount), 0u);

    graphicsRenderer->compute(
        glm::uvec3(m_size, 1u),
        m_clearOITNodeIDImageProgram,
        { shared_from_this() });

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, colorTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::DepthStencil, depthStencilTexture());

    framebuffer->setClearDepthStencil(1.f, 0x00u);
    framebuffer->setClearColor(0u, glm::uvec4(0u));
    framebuffer->setClearMask({
        graphics::FrameBufferAttachment::Color0,
        core::graphics::FrameBufferAttachment::Depth,
        core::graphics::FrameBufferAttachment::Stencil });

    framebuffer->clear();
}

void GeometryBuffer::sortOITNodes(const std::shared_ptr<graphics::RendererBase>& graphicsRenderer) const
{
    graphicsRenderer->compute(
        glm::uvec3(m_size, 1u),
        m_sortOITNodesProgram,
        { shared_from_this() });
}

PConstGBuffer GeometryBuffer::GBuffer() const
{
    return m_GBuffer;
}

PConstOITBuffer GeometryBuffer::OITBuffer() const
{
    return m_OITBuffer;
}

graphics::PConstTexture GeometryBuffer::colorTexture() const
{
    return m_colorTextureHandle->texture();
}

graphics::PConstTexture GeometryBuffer::depthStencilTexture() const
{
    return m_depthStencilTextureHandle->texture();
}

graphics::PConstTexture GeometryBuffer::finalTexture() const
{
    return m_finalTextureHandle->texture();
}

}
}
