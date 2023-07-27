#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/cameranode.h>
#include <core/rendersurface.h>
#include <core/igraphicsrenderer.h>

#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

CameraNode::CameraNode(std::shared_ptr<RenderSurface> renderSurface, const std::string &name)
    : Node(std::make_unique<CameraNodePrivate>(name))
{
    setPerspectiveProjection(glm::half_pi<float>());
    setRenderingEnabled(true);
    setRenderSurface(renderSurface);
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

void CameraNode::resizeRenderSurfaces(const glm::uvec2 &size)
{
    auto &mPrivate = m();
    mPrivate.renderSurface()->resize(size);
    mPrivate.gSurface()->resize(size);
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

std::shared_ptr<RenderSurface> CameraNode::renderSurface()
{
    return m().renderSurface();
}

std::shared_ptr<GSurface> CameraNode::gSurface()
{
    return m().gSurface();
}

void CameraNode::setRenderSurface(std::shared_ptr<RenderSurface> renderSurface)
{
    assert(renderSurface);

    auto &mPrivate = m();
    mPrivate.renderSurface() = renderSurface;
    mPrivate.gSurface() = std::shared_ptr<GSurface>(new GSurface(renderSurface->renderer().lock()));
    if (renderSurface->isSizeFixed())
        mPrivate.gSurface()->setFixedSize(renderSurface->fixedSize());
    else
        mPrivate.gSurface()->setFloatingSize(renderSurface->sizeScale());
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

GSurface::~GSurface() = default;

std::shared_ptr<graphics::ITexture> GSurface::colorBuffer0()
{
    graphics::IFrameBuffer::AttachmentInfo info;
    frameBuffer()->attachment(core::graphics::FrameBufferAttachment::Color0, info);
    return std::dynamic_pointer_cast<graphics::ITexture>(info.renderSurface);
}

std::shared_ptr<graphics::ITexture> GSurface::colorBuffer1()
{
    graphics::IFrameBuffer::AttachmentInfo info;
    frameBuffer()->attachment(core::graphics::FrameBufferAttachment::Color1, info);
    return std::dynamic_pointer_cast<graphics::ITexture>(info.renderSurface);
}

std::shared_ptr<graphics::ITexture> GSurface::depthBuffer()
{
    graphics::IFrameBuffer::AttachmentInfo info;
    frameBuffer()->attachment(core::graphics::FrameBufferAttachment::DepthStencil, info);
    return std::dynamic_pointer_cast<graphics::ITexture>(info.renderSurface);
}

GSurface::GSurface(std::shared_ptr<graphics::IRenderer> renderer)
    : RenderSurface(renderer, renderer->createFrameBuffer())
{
    auto fb = frameBuffer();
    fb->setClearMask({core::graphics::FrameBufferAttachment::Color0,
                      core::graphics::FrameBufferAttachment::Color1,
                      core::graphics::FrameBufferAttachment::DepthStencil});
    fb->setClearColor(0u, glm::vec4(0.f, 0.f, 0.f, 1.f));
    fb->setClearColor(1u, glm::vec4(0.f, 0.f, 0.f, 0.f));
    fb->setClearDepthStencil(1.f, 0);
}

void GSurface::doResize(const glm::uvec2 &size)
{
    assert(!renderer().expired());
    auto graphicsRenderer = renderer().lock();

    frameBuffer()->attach(core::graphics::FrameBufferAttachment::Color0,
                          graphicsRenderer->createTextureRectEmpty(size.x, size.y, core::graphics::PixelInternalFormat::R11F_G11F_B10F));
    frameBuffer()->attach(core::graphics::FrameBufferAttachment::Color1,
                          graphicsRenderer->createTextureRectEmpty(size.x, size.y, core::graphics::PixelInternalFormat::RGB10_A2));
    frameBuffer()->attach(core::graphics::FrameBufferAttachment::DepthStencil,
                          graphicsRenderer->createTextureRectEmpty(size.x, size.y, core::graphics::PixelInternalFormat::Depth24Stencil8));
}

}
}
