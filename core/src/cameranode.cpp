#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/cameranode.h>

#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

CameraNode::CameraNode(const std::string &name)
    : Node(new CameraNodePrivate(name))
{
    setPerspectiveProjection(glm::half_pi<float>());
    setRenderingEnabled(true);
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
    return m().renderingEnabled();
}

void CameraNode::setRenderingEnabled(bool enabled)
{
    m().renderingEnabled() = enabled;
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

}
}
