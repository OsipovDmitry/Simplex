#include <core/igraphicsrenderer.h>
#include <core/ssao.h>
#include <core/shadow.h>

#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

FrameBufferWrapper::FrameBufferWrapper(const std::shared_ptr<graphics::IFrameBuffer> &frameBuffer)
    : m_frameBuffer(frameBuffer)
    , m_viewportSize(0u)
{
}

FrameBufferWrapper::~FrameBufferWrapper() = default;


const std::shared_ptr<graphics::IFrameBuffer> &FrameBufferWrapper::frameBuffer() const
{
    return m_frameBuffer;
}

const glm::uvec2 &FrameBufferWrapper::viewportSize() const
{
    return m_viewportSize;
}

GFrameBuffer::GFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
{
    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);
    m_frameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
    setForGeometryOpaquePass();
}

void GFrameBuffer::setForGeometryOpaquePass()
{
    m_frameBuffer->setClearMask({ core::graphics::FrameBufferAttachment::Depth, core::graphics::FrameBufferAttachment::Stencil });
    m_frameBuffer->setDrawBuffers({});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(false);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(true);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Always, 0x01u, 0xFFu);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack, {graphics::StencilOperation::Keep,
                                                                           graphics::StencilOperation::Keep,
                                                                           graphics::StencilOperation::Replace});
    m_frameBuffer->setBlending(false);
}

void GFrameBuffer::setForGeometryTransparentPass()
{
    m_frameBuffer->setClearMask({});
    m_frameBuffer->setDrawBuffers({});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(false);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void GFrameBuffer::setForStencilPass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Stencil});
    m_frameBuffer->setDrawBuffers({});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(false);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(false);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Always, 0x00u, 0xFFu);
    m_frameBuffer->setStencilOperations(graphics::FaceType::Front,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::DecrementWrap, graphics::StencilOperation::Keep});
    m_frameBuffer->setStencilOperations(graphics::FaceType::Back,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::IncrementWrap, graphics::StencilOperation::Keep});

    m_frameBuffer->setBlending(false);
}

void GFrameBuffer::setForLightPass()
{
    m_frameBuffer->setClearMask({});
    m_frameBuffer->setDrawBuffers({});

    m_frameBuffer->setFaceCulling(true, graphics::FaceType::Front);
    m_frameBuffer->setColorMasks(false);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::NotEqual, 0x00u, 0xFFu);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::Keep, graphics::StencilOperation::Keep});
    m_frameBuffer->setBlending(false);
}

void GFrameBuffer::setForBackgroundPass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Equal, 0x00, 0xFF);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::Keep, graphics::StencilOperation::Keep});
    m_frameBuffer->setBlending(false);
}

void GFrameBuffer::setForFinalPass()
{
    m_frameBuffer->setClearMask({});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(true);
    m_frameBuffer->setBlendEquation(0u, graphics::BlendEquation::Add, graphics::BlendEquation::Add);
    m_frameBuffer->setBlendFactor(0u,
                                  graphics::BlendFactor::SrcAlpha, graphics::BlendFactor::OneMinusSrcAlpha,
                                  graphics::BlendFactor::One, graphics::BlendFactor::SrcAlpha);
}

void GFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                          const glm::uvec2 &viewportSize)
{
    if (m_viewportSize != viewportSize)
    {
        m_viewportSize = viewportSize;

        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                              graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                       m_viewportSize.y,
                                                                       core::graphics::PixelInternalFormat::RGBA16F));

        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Depth,
                              graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                       m_viewportSize.y,
                                                                       core::graphics::PixelInternalFormat::Depth32F));

        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Stencil,
                              graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                       m_viewportSize.y,
                                                                       core::graphics::PixelInternalFormat::Stencil8));

        m_oitDepthImage = graphicsRenderer->createImage(graphics::IImage::DataAccess::ReadWrite,
                                                        graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                                                 m_viewportSize.y,
                                                                                                 core::graphics::PixelInternalFormat::R32F));

        m_oitIndicesImage = graphicsRenderer->createImage(graphics::IImage::DataAccess::ReadWrite,
                                                          graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                                                   m_viewportSize.y,
                                                                                                   core::graphics::PixelInternalFormat::R32UI));
    }
}

graphics::PConstTexture GFrameBuffer::colorTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PConstTexture GFrameBuffer::depthTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Depth, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PConstTexture GFrameBuffer::stencilTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Stencil, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PConstImage GFrameBuffer::oitDepthImage()
{
    return m_oitDepthImage;
}

graphics::PConstImage GFrameBuffer::oitIndicesImage()
{
    return m_oitIndicesImage;
}

PostprocessFrameBuffer::PostprocessFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                                               const std::shared_ptr<graphics::IFrameBuffer> &userFrameBuffer)
    : FrameBufferWrapper(userFrameBuffer ? userFrameBuffer : graphicsRenderer->createFrameBuffer())
    , m_useUserFrameBuffer(userFrameBuffer != nullptr)
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void PostprocessFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer, const glm::uvec2 &viewportSize)
{
    if (m_viewportSize != viewportSize)
    {
        m_viewportSize = viewportSize;

        if (!m_useUserFrameBuffer)
            m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                                  graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                           m_viewportSize.y,
                                                                           core::graphics::PixelInternalFormat::RGBA8));
    }
}

ShadowFrameBuffer::ShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
    , m_shadingMode(ShadingMode::Disabled)
    , m_shadingFilter(ShadingFilter::Point)
{
    setForOpaquePass();
}

void ShadowFrameBuffer::setForOpaquePass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0, core::graphics::FrameBufferAttachment::Depth});
    m_frameBuffer->setClearColor(0u, glm::vec4(1.f));
    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);
    m_frameBuffer->setDrawBuffers({ (m_shadingFilter == ShadingFilter::VSM) ? core::graphics::FrameBufferAttachment::Color0 : core::graphics::FrameBufferAttachment::Count,
                                    core::graphics::FrameBufferAttachment::Count });

    m_frameBuffer->setFaceCulling(false/*true, graphics::FaceType::Front*/);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(true);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void ShadowFrameBuffer::setForTransparentPass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color1});
    m_frameBuffer->setClearColor(1u, glm::vec4(1.f));
    m_frameBuffer->setDrawBuffers({ core::graphics::FrameBufferAttachment::Count,
                                    (m_shadingMode == ShadingMode::Color) ? core::graphics::FrameBufferAttachment::Color1 : core::graphics::FrameBufferAttachment::Count });

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(true);
    m_frameBuffer->setBlendEquation(1u, graphics::BlendEquation::Add, graphics::BlendEquation::Add);
    m_frameBuffer->setBlendFactor(1u,
                                  graphics::BlendFactor::Zero, graphics::BlendFactor::SrcColor,
                                  graphics::BlendFactor::Zero, graphics::BlendFactor::SrcAlpha);

}

void ShadowFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                               const glm::uvec2 &viewportSize,
                               ShadingMode shadingMode,
                               ShadingFilter shadingFilter)
{
    if ((m_viewportSize != viewportSize) || (m_shadingMode != shadingMode) || (m_shadingFilter != shadingFilter))
    {
        m_viewportSize = viewportSize;
        m_shadingMode = shadingMode;
        m_shadingFilter = shadingFilter;

        auto depthTexture = doTexture(graphicsRenderer, m_viewportSize, graphics::PixelInternalFormat::Depth32F);
        depthTexture->setFilterMode(graphics::TextureFilterMode::Point);
        depthTexture->setWrapMode(graphics::TextureWrapMode::ClampToBorder);
        depthTexture->setBorderColor(glm::vec4(1.f));
        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Depth, depthTexture);

        if (m_shadingFilter == ShadingFilter::VSM)
        {
            auto depthVSMTexture = doTexture(graphicsRenderer, m_viewportSize, graphics::PixelInternalFormat::RG32F);
            depthVSMTexture->setFilterMode(graphics::TextureFilterMode::Linear);
            depthVSMTexture->setWrapMode(graphics::TextureWrapMode::ClampToBorder);
            depthVSMTexture->setBorderColor(glm::vec4(1.f));
            m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0, depthVSMTexture);
        }
        else
        {
            m_frameBuffer->detach(core::graphics::FrameBufferAttachment::Color0);
        }

        if (m_shadingMode == ShadingMode::Color)
        {
            auto colorTexture = doTexture(graphicsRenderer, m_viewportSize, graphics::PixelInternalFormat::R11F_G11F_B10F);
            colorTexture->setFilterMode(graphics::TextureFilterMode::Linear);
            colorTexture->setWrapMode(graphics::TextureWrapMode::ClampToBorder);
            colorTexture->setBorderColor(glm::vec4(1.f));
            m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color1, colorTexture);
        }
        else
        {
            m_frameBuffer->detach(core::graphics::FrameBufferAttachment::Color1);
        }
    }
}

graphics::PConstTexture ShadowFrameBuffer::colorTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color1, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PConstTexture ShadowFrameBuffer::depthTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Depth, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PConstTexture ShadowFrameBuffer::depthVSMTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

ShadowFrameBuffer2D::ShadowFrameBuffer2D(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : ShadowFrameBuffer(graphicsRenderer)
{
}

graphics::PTexture ShadowFrameBuffer2D::doTexture(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                                                  const glm::uvec2 &viewportSize,
                                                  graphics::PixelInternalFormat internalFormat) const
{
    return graphicsRenderer->createTexture2DEmpty(viewportSize.x, viewportSize.y, internalFormat);
}

ShadowFrameBufferCube::ShadowFrameBufferCube(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : ShadowFrameBuffer(graphicsRenderer)
{
}

graphics::PTexture ShadowFrameBufferCube::doTexture(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                                                    const glm::uvec2 &viewportSize,
                                                    graphics::PixelInternalFormat internalFormat) const
{
    return graphicsRenderer->createTextureCubeEmpty(viewportSize.x, viewportSize.y, internalFormat);
}

SSAOFrameBuffer::SSAOFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
    , m_pixelInternalFormat(graphics::PixelInternalFormat::Undefined)
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void SSAOFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                             const glm::uvec2 &viewportSize,
                             SSAOMode ssaoMode)
{
    const auto internalFormat = ssaoMode == SSAOMode::Color ?
                core::graphics::PixelInternalFormat::RGB8 :
                core::graphics::PixelInternalFormat::R8;

    if ((m_viewportSize != viewportSize) || (m_pixelInternalFormat != internalFormat))
    {
        m_viewportSize = viewportSize;
        m_pixelInternalFormat = internalFormat;

        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                              graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                       m_viewportSize.y,
                                                                       m_pixelInternalFormat));
    }
}

graphics::PConstTexture SSAOFrameBuffer::colorTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PixelInternalFormat SSAOFrameBuffer::pixelInternalFormat() const
{
    return m_pixelInternalFormat;
}

BlurFrameBuffer::BlurFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
    , m_pixelInternalFormat(graphics::PixelInternalFormat::Undefined)
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void BlurFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                             const glm::uvec2 &viewportSize,
                             graphics::PixelInternalFormat pixelInternalFormat)
{
    if ((m_viewportSize != viewportSize) || (m_pixelInternalFormat != pixelInternalFormat))
    {
        m_viewportSize = viewportSize;
        m_pixelInternalFormat = pixelInternalFormat;

        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                              graphicsRenderer->createTextureRectEmpty(m_viewportSize.x,
                                                                       m_viewportSize.y,
                                                                       m_pixelInternalFormat));
    }
}

graphics::PConstTexture BlurFrameBuffer::colorTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

}
}
