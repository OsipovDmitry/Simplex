#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/cameranode.h>
#include <core/scene.h>
#include <core/graphicsengine.h>
#include <core/igraphicsrenderer.h>

#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

CameraNode::CameraNode(const std::string &name)
    : Node(std::make_unique<CameraNodePrivate>(name))
{
    setRenderingEnabled(true);
    setPerspectiveProjection(glm::half_pi<float>());
    setCullPlanesLimits(glm::vec2(.5f, std::numeric_limits<float>::max()));
}

CameraNode::~CameraNode()
{
}

std::shared_ptr<CameraNode> CameraNode::asCameraNode()
{
    return std::dynamic_pointer_cast<CameraNode>(shared_from_this());
}

std::shared_ptr<const CameraNode> CameraNode::asCameraNode() const
{
    return std::dynamic_pointer_cast<const CameraNode>(shared_from_this());
}

bool CameraNode::isRenderingEnabled() const
{
    return m().isRenderingEnabled();
}

void CameraNode::setRenderingEnabled(bool enabled)
{
    m().isRenderingEnabled() = enabled;
}

void CameraNode::setOrthoProjection()
{
    m().clipSpace() = std::make_shared<utils::OrthoClipSpace>();
}

void CameraNode::setPerspectiveProjection(float fov)
{
    m().clipSpace() = std::make_shared<utils::PerspectiveClipSpace>(fov);
}

glm::mat4 CameraNode::projectionMatrix(float aspect, float zNear, float zFar) const
{
    return m().clipSpace()->projectionMatrix(aspect, zNear, zFar);
}

void CameraNode::resize(const glm::uvec2 &value)
{
    auto parentScene = scene();
    if (!parentScene)
        return;

    auto graphicsEngine = parentScene->graphicsEngine();
    if (graphicsEngine.expired())
        return;

    auto graphicsRenderer = graphicsEngine.lock()->graphicsRenderer();
    if (!graphicsRenderer)
        return;

    auto &mPrivate = m();

    auto &GFrameBuffer = mPrivate.GFrameBuffer();
    if (!GFrameBuffer)
    {
        GFrameBuffer = graphicsRenderer->createFrameBuffer();
        GFrameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0,
                                    core::graphics::FrameBufferAttachment::Color1,
                                    core::graphics::FrameBufferAttachment::DepthStencil});
        GFrameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
        GFrameBuffer->setClearColor(1u, glm::vec4(0.f, 0.f, 0.f, 0.f));
        GFrameBuffer->setClearDepthStencil(1.f, 0);
        GFrameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0, core::graphics::FrameBufferAttachment::Color1});
    }

    auto &OITFrameBuffer = mPrivate.OITFrameBuffer();
    if (!OITFrameBuffer)
    {
        OITFrameBuffer = graphicsRenderer->createFrameBuffer();
        OITFrameBuffer->setClearMask({});
        OITFrameBuffer->setDrawBuffers({});
    }

    auto &finalFrameBuffer = mPrivate.finalFrameBuffer();
    if (!finalFrameBuffer)
    {
        finalFrameBuffer = graphicsRenderer->createFrameBuffer();
        finalFrameBuffer->setClearMask({core::graphics::FrameBufferAttachment::Color0});
        finalFrameBuffer->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
        finalFrameBuffer->setDrawBuffers({core::graphics::FrameBufferAttachment::Color0});
    }

    auto &viewportSize = mPrivate.viewportSize();
    if (viewportSize != value)
    {
        viewportSize = value;

        GFrameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                             graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                      viewportSize.y,
                                                                      core::graphics::PixelInternalFormat::R11F_G11F_B10F));

        GFrameBuffer->attach(core::graphics::FrameBufferAttachment::Color1,
                             graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                      viewportSize.y,
                                                                      core::graphics::PixelInternalFormat::RGBA8));

        auto gDepthStencil = graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                      viewportSize.y,
                                                                      core::graphics::PixelInternalFormat::Depth24Stencil8);
        GFrameBuffer->attach(core::graphics::FrameBufferAttachment::DepthStencil, gDepthStencil);

        OITFrameBuffer->attach(core::graphics::FrameBufferAttachment::DepthStencil, gDepthStencil);

        auto oitIndicesTexture = graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                          viewportSize.y,
                                                                          core::graphics::PixelInternalFormat::R32UI);
        mPrivate.OITIndicesImage() = graphicsRenderer->createImage(oitIndicesTexture, graphics::IImage::DataAccess::ReadWrite);

        finalFrameBuffer->attach(core::graphics::FrameBufferAttachment::Color0,
                                 graphicsRenderer->createTextureRectEmpty(viewportSize.x,
                                                                          viewportSize.y,
                                                                          core::graphics::PixelInternalFormat::RGBA8));
    }
}

const glm::uvec2 &CameraNode::viewportSize() const
{
    return m().viewportSize();
}

const glm::vec2 &CameraNode::cullPlanesLimits() const
{
    return m().cullPlanesLimits();
}

void CameraNode::setCullPlanesLimits(const glm::vec2 &values)
{
    if (values[0] <= 0.f)
        LOG_CRITICAL << "Znear must be greater than 0.0";

    if (values[1] <= values[0])
        LOG_CRITICAL << "Zfar must be greater than Znear";

    m().cullPlanesLimits() = values;
}

std::shared_ptr<graphics::IFrameBuffer> CameraNode::GFrameBuffer()
{
    return m().GFrameBuffer();
}

graphics::PTexture CameraNode::GFrameBufferColorMap0() const
{
    graphics::IFrameBuffer::AttachmentInfo info;
    m().GFrameBuffer()->attachment(core::graphics::FrameBufferAttachment::Color0, info);
    return std::dynamic_pointer_cast<graphics::ITexture>(info.renderSurface);
}

graphics::PTexture CameraNode::GFrameBufferColorMap1() const
{
    graphics::IFrameBuffer::AttachmentInfo info;
    m().GFrameBuffer()->attachment(core::graphics::FrameBufferAttachment::Color1, info);
    return std::dynamic_pointer_cast<graphics::ITexture>(info.renderSurface);
}

graphics::PTexture CameraNode::GFrameBufferDepthStencilMap() const
{
    graphics::IFrameBuffer::AttachmentInfo info;
    m().GFrameBuffer()->attachment(core::graphics::FrameBufferAttachment::DepthStencil, info);
    return std::dynamic_pointer_cast<graphics::ITexture>(info.renderSurface);
}

std::shared_ptr<graphics::IFrameBuffer> CameraNode::OITFrameBuffer()
{
    return m().OITFrameBuffer();
}

std::shared_ptr<graphics::IImage> CameraNode::OITIndicesImage()
{
    return m().OITIndicesImage();
}

std::shared_ptr<graphics::IFrameBuffer> CameraNode::finalFrameBuffer()
{
    return m().finalFrameBuffer();
}

graphics::PTexture CameraNode::finalFrameBufferColorMap() const
{
    graphics::IFrameBuffer::AttachmentInfo info;
    m().finalFrameBuffer()->attachment(core::graphics::FrameBufferAttachment::Color0, info);
    return std::dynamic_pointer_cast<graphics::ITexture>(info.renderSurface);
}

bool CameraNode::canAttach(std::shared_ptr<Node>)
{
    LOG_ERROR << "It's forbidden to attach to camera node \"" << name() << "\"";
    return false;
}

bool CameraNode::canDetach(std::shared_ptr<Node>)
{
    LOG_ERROR << "It's forbidden to detach from camera node \"" << name() << "\"";
    return false;
}

void CameraNode::doDetach()
{
    Node::doDetach();

    auto &mPrivate = m();
    mPrivate.viewportSize() = glm::uvec2(0u, 0u);
    mPrivate.GFrameBuffer() = nullptr;
    mPrivate.finalFrameBuffer() = nullptr;
    mPrivate.OITIndicesImage() = nullptr;
}

}
}
