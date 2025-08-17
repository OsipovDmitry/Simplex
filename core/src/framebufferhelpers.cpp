#include <core/graphicsrendererbase.h>
#include <core/ssao.h>
#include <core/shadow.h>

#include "framebufferhelpers.h"
#include "geometrybuffer.h"
#include "shadowbuffer.h"

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

GFrameBufferTmp::GFrameBufferTmp(
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<GeometryBuffer>& GBuffer)
    : m_frameBuffer(frameBuffer)
    , m_GBuffer(GBuffer)
{
    setForGeometryOpaquePass();
}

void GFrameBufferTmp::setForGeometryOpaquePass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Depth, m_GBuffer->depthTexture());
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Stencil, m_GBuffer->stencilTexture());

    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);
    m_frameBuffer->setClearMask({ core::graphics::FrameBufferAttachment::Depth, core::graphics::FrameBufferAttachment::Stencil });

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

void GFrameBufferTmp::setForGeometryTransparentPass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Depth, m_GBuffer->depthTexture());
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Stencil, m_GBuffer->stencilTexture());

    m_frameBuffer->setClearMask({});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(false);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void GFrameBufferTmp::setForStencilPass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Depth, m_GBuffer->depthTexture());
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Stencil, m_GBuffer->stencilTexture());

    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Stencil});

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

void GFrameBufferTmp::setForLightPass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Stencil, m_GBuffer->stencilTexture());

    m_frameBuffer->setClearMask({});

    m_frameBuffer->setFaceCulling(true, graphics::FaceType::Front);
    m_frameBuffer->setColorMasks(false);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::NotEqual, 0x00u, 0xFFu);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::Keep, graphics::StencilOperation::Keep});
    m_frameBuffer->setBlending(false);
}

void GFrameBufferTmp::setForBackgroundPass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Color0, m_GBuffer->finalTexture());
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Stencil, m_GBuffer->stencilTexture());

    m_frameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Equal, 0x00, 0xFF);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::Keep, graphics::StencilOperation::Keep});
    m_frameBuffer->setBlending(false);
}

void GFrameBufferTmp::setForFinalPass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(graphics::FrameBufferAttachment::Color0, m_GBuffer->finalTexture());

    m_frameBuffer->setClearMask({});

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

std::shared_ptr<graphics::IFrameBuffer> GFrameBufferTmp::frameBuffer() const
{
    return m_frameBuffer;
}

PostprocessFrameBuffer::PostprocessFrameBuffer(const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer)
    : m_frameBuffer(frameBuffer)
{
}

void PostprocessFrameBuffer::setForPass()
{
    m_frameBuffer->setClearMask({ core::graphics::FrameBufferAttachment::Color0 });

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

std::shared_ptr<graphics::IFrameBuffer> PostprocessFrameBuffer::frameBuffer() const
{
    return m_frameBuffer;
}

ShadowFrameBuffer::ShadowFrameBuffer(
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<ShadowBuffer>& shadowBuffer)
    : m_frameBuffer(frameBuffer)
    , m_shadowBuffer(shadowBuffer)
{
    setForOpaquePass();
}

void ShadowFrameBuffer::setForOpaquePass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Depth, m_shadowBuffer->depthTexture());
    if (m_shadingFilter == ShadingFilter::VSM)
        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0, m_shadowBuffer->depthVSMTexture());

    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0, core::graphics::FrameBufferAttachment::Depth});
    m_frameBuffer->setClearColor(0u, glm::vec4(1.f));
    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);
    //m_frameBuffer->setDrawBuffers({ (m_shadingFilter == ShadingFilter::VSM) ? core::graphics::FrameBufferAttachment::Color0 : core::graphics::FrameBufferAttachment::Count,
    //                                core::graphics::FrameBufferAttachment::Count });

    m_frameBuffer->setFaceCulling(false/*true, graphics::FaceType::Front*/);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(true);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void ShadowFrameBuffer::setForTransparentPass()
{
    m_frameBuffer->detachAll();
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Depth, m_shadowBuffer->depthTexture());
    if (m_shadingMode == ShadingMode::Color)
        m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color1, m_shadowBuffer->colorTexture());

    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color1});
    m_frameBuffer->setClearColor(1u, glm::vec4(1.f));
    //m_frameBuffer->setDrawBuffers({ core::graphics::FrameBufferAttachment::Count,
    //                                (m_shadingMode == ShadingMode::Color) ? core::graphics::FrameBufferAttachment::Color1 : core::graphics::FrameBufferAttachment::Count });

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

std::shared_ptr<graphics::IFrameBuffer> ShadowFrameBuffer::frameBuffer() const
{
    return m_frameBuffer;
}

std::shared_ptr<ShadowBuffer> ShadowFrameBuffer::shadowBuffer() const
{
    return m_shadowBuffer;
}

SSAOFrameBuffer::SSAOFrameBuffer(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
    , m_pixelInternalFormat(graphics::PixelInternalFormat::Count)
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    //m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void SSAOFrameBuffer::resize(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer,
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

    if (auto surface = m_frameBuffer->attachmentSurface(core::graphics::FrameBufferAttachment::Color0))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(surface);

    return result;
}

graphics::PixelInternalFormat SSAOFrameBuffer::pixelInternalFormat() const
{
    return m_pixelInternalFormat;
}

BlurFrameBuffer::BlurFrameBuffer(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
    , m_pixelInternalFormat(graphics::PixelInternalFormat::Count)
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    //m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void BlurFrameBuffer::resize(const std::shared_ptr<graphics::RendererBase> &graphicsRenderer,
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

    if (auto surface = m_frameBuffer->attachmentSurface(core::graphics::FrameBufferAttachment::Color1))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(surface);

    return result;
}

}
}
