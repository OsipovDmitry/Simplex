#include "geometrybuffer.h"

#include <utils/glm/common.hpp>
#include <utils/glm/gtx/component_wise.hpp>

#include <core/graphicsrendererbase.h>
#include <core/programsloader.h>
#include <core/settings.h>

#include "resources.h"
#include "uniform.h"

namespace simplex
{
namespace core
{

GeometryBuffer::GeometryBuffer()
    : StateSet()
{
    if (m_fixedSize) m_fixedSize = glm::max(m_fixedSize.value(), glm::uvec2(1u));

    const auto& OITSetings = settings::Settings::instance().graphics().oit();
    m_maxOITNodesCount = OITSetings.maxNodesCount();
    m_OITNodesCountPerPixel = OITSetings.nodesCountPerPixel();
}

GeometryBuffer::~GeometryBuffer() = default;

void GeometryBuffer::initialize(const std::shared_ptr<ProgramsLoader>& programsLoader)
{
    if (m_isInitialized) return;

    m_clearOITNodeIDImageProgram =
        programsLoader->loadOrGetComputeProgram(resources::ClearOITNodeIDImagePassComputeShaderPath, {});

    m_sortOITNodesProgram = programsLoader->loadOrGetComputeProgram(resources::SortOITNodesPassComputeShaderPath, {});

    m_isInitialized = true;
}

const std::optional<glm::uvec2>& GeometryBuffer::fixedSize() const
{
    return m_fixedSize;
}

void GeometryBuffer::setFixedSize(const std::optional<glm::uvec2>& value)
{
    const auto newFixedSize = value ? std::optional<glm::uvec2>(glm::max(value.value(), glm::uvec2(1u))) : std::nullopt;

    if (m_fixedSize == newFixedSize) return;

    m_fixedSize = newFixedSize;
    m_size = glm::uvec2(0u); // it'll affect buffers recreating next frame
}

const glm::uvec2& GeometryBuffer::resize(const glm::uvec2& size, const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
{
    const glm::uvec2 newSize = m_fixedSize ? m_fixedSize.value() : glm::max(size, glm::uvec2(1u, 1u));

    if (m_size != newSize)
    {

        m_size = newSize;
        recreateBuffers(graphicsRenderer);
    }

    return m_size;
}

uint32_t GeometryBuffer::maxOITNodesCount() const
{
    return m_maxOITNodesCount;
}

uint32_t GeometryBuffer::OITNodesCountPerPixel() const
{
    return m_OITNodesCountPerPixel;
}

void GeometryBuffer::setOITNodesCount(
    uint32_t maxOITNodesCount,
    uint32_t OITNodesCountPerPixel,
    const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
{
    if ((m_maxOITNodesCount == maxOITNodesCount) && (m_OITNodesCountPerPixel == OITNodesCountPerPixel)) return;

    m_maxOITNodesCount = maxOITNodesCount;
    m_OITNodesCountPerPixel = OITNodesCountPerPixel;
    recreateBuffers(graphicsRenderer);
}

void GeometryBuffer::clear(
    const std::shared_ptr<graphics::RendererBase>& graphicsRenderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer) const
{
    m_GBuffer->setField(offsetof(GBufferDescription, OITNodesCount), 0u);

    graphicsRenderer->compute(glm::uvec3(m_size, 1u), m_clearOITNodeIDImageProgram, {shared_from_this()});

    framebuffer->reset();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, m_colorTextureHandle->texture());
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, m_depthTextureHandle->texture());
    framebuffer->setClearColor(0u, glm::uvec4(0u));
    framebuffer->setColorMask(0u, true);
    framebuffer->setDepthTest(true);
    framebuffer->setDepthFunc(graphics::ComparingFunc::Always);
    framebuffer->setDepthMask(true);

    if (!framebuffer->isComplete())
    {
        LOG_CRITICAL << "Frambuffer is not complete";
        return;
    }

    framebuffer->clear({graphics::FrameBufferAttachment::Color0, graphics::FrameBufferAttachment::Depth});
}

void GeometryBuffer::sortOITNodes(const std::shared_ptr<graphics::RendererBase>& graphicsRenderer) const
{
    graphicsRenderer->compute(glm::uvec3(m_size, 1u), m_sortOITNodesProgram, {shared_from_this()});
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

graphics::PConstTexture GeometryBuffer::depthTexture() const
{
    return m_depthTextureHandle->texture();
}

void GeometryBuffer::recreateBuffers(const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
{
    auto colorTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::RGBA32UI);
    m_colorTextureHandle = graphicsRenderer->createTextureHandle(colorTexture);
    m_colorTextureHandle->makeResident();

    auto depthTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::Depth32F);
    m_depthTextureHandle = graphicsRenderer->createTextureHandle(depthTexture);
    m_depthTextureHandle->makeResident();

    auto OITNodeIDTexture = graphicsRenderer->createTextureRectEmpty(m_size.x, m_size.y, graphics::PixelInternalFormat::R32UI);
    auto OITNodeIDImage = graphics::Image::create(graphics::Image::DataAccess::ReadWrite, OITNodeIDTexture);
    m_OITNodeIDImageHandle = graphicsRenderer->createImageHandle(OITNodeIDImage);
    m_OITNodeIDImageHandle->makeResident();

    const auto OITBufferSize = glm::min(m_maxOITNodesCount, glm::compMul(m_size) * m_OITNodesCountPerPixel);

    m_GBuffer = PGBuffer::element_type::create(GBufferDescription::make(
        m_colorTextureHandle->handle(), m_depthTextureHandle->handle(), m_OITNodeIDImageHandle->handle(), OITBufferSize));

    m_OITBuffer = POITBuffer::element_type::create();
    m_OITBuffer->resize(OITBufferSize);

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::GBuffer) = graphics::BufferRange::create(m_GBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OITNodesBuffer) = graphics::BufferRange::create(m_OITBuffer->buffer());
}

} // namespace core
} // namespace simplex
