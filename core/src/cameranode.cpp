#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/ssao.h>
#include <core/cameranode.h>
#include <core/settings.h>

#include "cameranodeprivate.h"
#include "geometrybuffer.h"

namespace simplex
{
namespace core
{

CameraNode::CameraNode(const std::string &name)
    : Node(std::make_unique<CameraNodePrivate>(*this, name))
{
    setRenderingEnabled(true);
    useDefaultFramebuffer();

    auto &graphicsSettings = settings::Settings::instance().graphics();
    auto &cameraSettings = graphicsSettings.camera();

    (cameraSettings.clipSpace().type() == utils::ClipSpaceType::Perspective) ?
        setPerspectiveClipSpace(cameraSettings.clipSpace().perspectiveFOV()) :
        setOrthoClipSpace(cameraSettings.clipSpace().orthoHeight());

    setCullPlanesLimits(graphicsSettings.cullPlaneLimits());
    setClusterMaxSize(cameraSettings.clusterMaxSize());
}

CameraNode::~CameraNode() = default;

std::shared_ptr<CameraNode> CameraNode::asCameraNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<CameraNode>(wp.lock());
}

std::shared_ptr<const CameraNode> CameraNode::asCameraNode() const
{
    return const_cast<CameraNode*>(this)->asCameraNode();
}

bool CameraNode::isRenderingEnabled() const
{
    return m().isRenderingEnabled();
}

void CameraNode::setRenderingEnabled(bool value)
{
    m().isRenderingEnabled() = value;
}

bool CameraNode::isDefaultFramebufferUsed() const
{
    return m().geometryBuffer() == nullptr;
}

void CameraNode::useDefaultFramebuffer()
{
    m().geometryBuffer() = nullptr;
}

void CameraNode::useSeparateFramebuffer(const glm::uvec2& size)
{
    m().geometryBuffer() = std::make_shared<GeometryBuffer>(size);
}

const utils::ClipSpace &CameraNode::clipSpace() const
{
    return m().clipSpace();
}

void CameraNode::setOrthoClipSpace(float height)
{
    auto &mPrivate = m();
    mPrivate.clipSpaceType() = utils::ClipSpaceType::Ortho;
    mPrivate.clipSpaceVerticalParam() = height;
    mPrivate.updateClipSpace();
}

void CameraNode::setPerspectiveClipSpace(float fovY)
{
    auto &mPrivate = m();
    mPrivate.clipSpaceType() = utils::ClipSpaceType::Perspective;
    mPrivate.clipSpaceVerticalParam() = fovY;
    mPrivate.updateClipSpace();
}

const utils::Range &CameraNode::cullPlanesLimits() const
{
    return m().cullPlanesLimits();
}

void CameraNode::setCullPlanesLimits(const utils::Range &value)
{
    if (value.nearValue() <= 0.f)
        LOG_CRITICAL << "ZNear must be greater than 0.0";

    if (value.farValue() <= value.nearValue())
        LOG_CRITICAL << "ZFar must be greater than Znear";

    m().cullPlanesLimits() = value;
}

const glm::uvec3& CameraNode::clusterMaxSize() const
{
    return m().clusterMaxSize();
}

void CameraNode::setClusterMaxSize(const glm::uvec3& value)
{
    m().clusterMaxSize() = value;
}

SSAO &CameraNode::ssao()
{
    return m().ssao();
}

const SSAO &CameraNode::ssao() const
{
    return const_cast<CameraNode*>(this)->ssao();
}

}
}
