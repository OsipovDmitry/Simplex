#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

FrameBufferWrapper::FrameBufferWrapper(const std::shared_ptr<graphics::IFrameBuffer> &frameBuffer)
    : m_frameBuffer(frameBuffer)
{
}

FrameBufferWrapper::~FrameBufferWrapper() = default;


const std::shared_ptr<graphics::IFrameBuffer> &FrameBufferWrapper::frameBuffer() const
{
    return m_frameBuffer;
}

GFrameBuffer::GFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0,
                                 core::graphics::FrameBufferAttachment::Color1,
                                 core::graphics::FrameBufferAttachment::Color2,
                                 core::graphics::FrameBufferAttachment::Depth,
                                 core::graphics::FrameBufferAttachment::Stencil});
    m_frameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
    m_frameBuffer->setClearColor(1u, glm::vec4(0.f, 0.f, 0.f, 0.f));
    m_frameBuffer->setClearColor(2u, glm::vec4(0.f, 0.f, 0.f, 0.f));
    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0,
                                   core::graphics::FrameBufferAttachment::Color1,
                                   core::graphics::FrameBufferAttachment::Color2});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(true);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Always, 0x01u, 0xFFu);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack, {graphics::StencilOperation::Keep,
                                                                           graphics::StencilOperation::Keep,
                                                                           graphics::StencilOperation::Replace});
    m_frameBuffer->setBlending(false);
}

void GFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                          const glm::uvec2 &viewportSize)
{
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::R11F_G11F_B10F));

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color1,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::RGB10_A2));

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color2,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::RGBA8));

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Depth,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::Depth32F));

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Stencil,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::Stencil8));
}

graphics::PConstTexture GFrameBuffer::color0Texture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PConstTexture GFrameBuffer::color1Texture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color1, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

graphics::PConstTexture GFrameBuffer::color2Texture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color2, info))
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

OITFrameBuffer::OITFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
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

void OITFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                            const graphics::PConstTexture &depthTexture,
                            const glm::uvec2 &viewportSize)
{
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Depth, depthTexture);

    auto oitDepthTexture = graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                    viewportSize.y,
                                                                    core::graphics::PixelInternalFormat::R32F);
    m_oitDepthImage = graphicsRenderer->createImage(oitDepthTexture, graphics::IImage::DataAccess::ReadWrite);

    auto oitIndicesTexture = graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                      viewportSize.y,
                                                                      core::graphics::PixelInternalFormat::R32UI);
    m_oitIndicesImage = graphicsRenderer->createImage(oitIndicesTexture, graphics::IImage::DataAccess::ReadWrite);
}

const graphics::PConstImage &OITFrameBuffer::oitDepthImage()
{
    return m_oitDepthImage;
}

const graphics::PConstImage &OITFrameBuffer::oitIndicesImage()
{
    return m_oitIndicesImage;
}

LightFrameBuffer::LightFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
{
    m_frameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 0.f));
    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);
    setForClearPass();
}

void LightFrameBuffer::setForClearPass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void LightFrameBuffer::setForStencilPass()
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

void LightFrameBuffer::setForLightPass()
{
    m_frameBuffer->setClearMask({});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(true, graphics::FaceType::Front);

    m_frameBuffer->setColorMasks(true);

    m_frameBuffer->setDepthTest(false);

    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::NotEqual, 0x00u, 0xFFu);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::Keep, graphics::StencilOperation::Keep});

    m_frameBuffer->setBlending(true);
    m_frameBuffer->setBlendEquation(0u, graphics::BlendEquation::Add, graphics::BlendEquation::Add);
    m_frameBuffer->setBlendFactor(0u, graphics::BlendFactor::One, graphics::BlendFactor::One, graphics::BlendFactor::One, graphics::BlendFactor::Zero);
}

void LightFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                              const graphics::PConstTexture &depthTexture,
                              const glm::uvec2 &viewportSize)
{
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Depth, depthTexture);

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Stencil,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::Stencil8));

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::RGBA16F));
}

graphics::PConstTexture LightFrameBuffer::colorTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

FinalFrameBuffer::FinalFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
{
    setForBackgroundPass();
}

void FinalFrameBuffer::setForBackgroundPass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Equal, 0x00, 0xFF);
    m_frameBuffer->setStencilOperations(graphics::FaceType::FrontAndBack,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::Keep, graphics::StencilOperation::Keep});
    m_frameBuffer->setBlending(false);
}

void FinalFrameBuffer::setForForegroundPass()
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

void FinalFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                              const graphics::PConstTexture &stencilTexture,
                              const glm::uvec2 &viewportSize)
{
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Stencil, stencilTexture);

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::RGBA16F));
}

graphics::PConstTexture FinalFrameBuffer::colorTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}

PostprocessFrameBuffer::PostprocessFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->defaultFrameBuffer())
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
    m_frameBuffer->setBlending(false);
}

void PostprocessFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &, const glm::uvec2 &)
{
}


}
}
