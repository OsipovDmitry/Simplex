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
                                 core::graphics::FrameBufferAttachment::Color1});
    m_frameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
    m_frameBuffer->setClearColor(1u, glm::vec4(0.f, 0.f, 0.f, 0.f));
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0, core::graphics::FrameBufferAttachment::Color1});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(true);
    m_frameBuffer->setStencilTest(false);
}

void GFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                          const graphics::PConstTexture &sharedDepthStencilTexture,
                          const glm::uvec2 &viewportSize)
{
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::R11F_G11F_B10F));

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color1,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::RGBA8));

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::DepthStencil, sharedDepthStencilTexture);
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
}

void OITFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                            const graphics::PConstTexture &sharedDepthStencilTexture,
                            const glm::uvec2 &viewportSize)
{
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::DepthStencil, sharedDepthStencilTexture);

    auto oitIndicesTexture = graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                      viewportSize.y,
                                                                      core::graphics::PixelInternalFormat::R32UI);
    m_oitIndicesImage = graphicsRenderer->createImage(oitIndicesTexture, graphics::IImage::DataAccess::ReadWrite);
}

const graphics::PConstImage &OITFrameBuffer::oitIndicesImage()
{
    return m_oitIndicesImage;
}

FinalFrameBuffer::FinalFrameBuffer(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer)
    : FrameBufferWrapper(graphicsRenderer->createFrameBuffer())
{
    m_frameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
    m_frameBuffer->setClearDepthStencil(1.f, 0x00u);

    setForBackgroundPass();
}

void FinalFrameBuffer::setForBackgroundPass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0, core::graphics::FrameBufferAttachment::DepthStencil});
    m_frameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});

    m_frameBuffer->setFaceCulling(false);
    m_frameBuffer->setColorMasks(true);
    m_frameBuffer->setDepthTest(false);
    m_frameBuffer->setStencilTest(false);
}

void FinalFrameBuffer::setForStencilPass()
{
    m_frameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Stencil});
    m_frameBuffer->setDrawBuffers({});

    m_frameBuffer->setFaceCulling(false);

    m_frameBuffer->setColorMasks(false);

    m_frameBuffer->setDepthTest(true);
    m_frameBuffer->setDepthMask(false);

    m_frameBuffer->setStencilTest(true);
    m_frameBuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Always, 0x00u, 0x00u);
    m_frameBuffer->setStencilOperations(graphics::FaceType::Front,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::DecrementWrap, graphics::StencilOperation::Keep});
    m_frameBuffer->setStencilOperations(graphics::FaceType::Back,
                                        {graphics::StencilOperation::Keep, graphics::StencilOperation::IncrementWrap, graphics::StencilOperation::Keep});
}

void FinalFrameBuffer::setForLightPass()
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
}

void FinalFrameBuffer::resize(const std::shared_ptr<graphics::IRenderer> &graphicsRenderer,
                              const graphics::PConstTexture &sharedDepthStencilTexture,
                              const glm::uvec2 &viewportSize)
{
    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::DepthStencil, sharedDepthStencilTexture);

    m_frameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                          graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                   viewportSize.y,
                                                                   core::graphics::PixelInternalFormat::R11F_G11F_B10F));
}

graphics::PConstTexture FinalFrameBuffer::colorTexture() const
{
    graphics::PConstTexture result;

    if (graphics::IFrameBuffer::AttachmentInfo info; m_frameBuffer->attachment(core::graphics::FrameBufferAttachment::Color0, info))
        result = std::dynamic_pointer_cast<const graphics::ITexture>(info.renderSurface);

    return result;
}


}
}
