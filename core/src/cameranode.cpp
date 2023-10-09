#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/igraphicsrenderer.h>
#include <core/cameranode.h>
#include <core/scene.h>
#include <core/graphicsengine.h>

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

    auto &gFrameBuffer = mPrivate.gFrameBuffer();
    if (!gFrameBuffer)
        gFrameBuffer = std::make_shared<GFrameBuffer>(graphicsRenderer);

    auto &oitFrameBuffer = mPrivate.oitFrameBuffer();
    if (!oitFrameBuffer)
        oitFrameBuffer = std::make_shared<OITFrameBuffer>(graphicsRenderer);

    auto &finalFrameBuffer = mPrivate.finalFrameBuffer();
    if (!finalFrameBuffer)
        finalFrameBuffer = std::make_shared<FinalFrameBuffer>(graphicsRenderer);

    auto &viewportSize = mPrivate.viewportSize();
    if (viewportSize != value)
    {
        viewportSize = value;

        mPrivate.sharedDepthStencilTexture() = graphicsRenderer->createTextureRectEmpty(
                    viewportSize.x,
                    viewportSize.y,
                    core::graphics::PixelInternalFormat::Depth24Stencil8);

        gFrameBuffer->resize(graphicsRenderer, mPrivate.sharedDepthStencilTexture(), viewportSize);
        oitFrameBuffer->resize(graphicsRenderer, mPrivate.sharedDepthStencilTexture(), viewportSize);
        finalFrameBuffer->resize(graphicsRenderer, mPrivate.sharedDepthStencilTexture(), viewportSize);
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
    mPrivate.gFrameBuffer() = nullptr;
    mPrivate.oitFrameBuffer() = nullptr;
    mPrivate.finalFrameBuffer() = nullptr;
}

}
}
