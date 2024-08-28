#include <utils/logger.h>
#include <utils/clipspace.h>

#include <core/ssao.h>
#include <core/cameranode.h>
#include <core/scene.h>
#include <core/settings.h>

#include "cameranodeprivate.h"

namespace simplex
{
namespace core
{

CameraNode::CameraNode(const std::string &name, const std::shared_ptr<graphics::IFrameBuffer> &frameBuffer)
    : Node(std::make_unique<CameraNodePrivate>(*this, name))
{
    setRenderingEnabled(true);
    setFrameBuffer(frameBuffer);

    auto &graphicsSettings = settings::Settings::instance().graphics();
    auto &cameraSettings = graphicsSettings.camera();

    (cameraSettings.clipSpace().type() == utils::ClipSpaceType::Perspective) ?
        setPerspectiveClipSpace(cameraSettings.clipSpace().perspectiveFOV()) :
        setOrthoClipSpace(cameraSettings.clipSpace().orthoHeight());

    setCullPlanesLimits(graphicsSettings.cullPlaneLimits());
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

std::shared_ptr<graphics::IFrameBuffer> CameraNode::frameBuffer()
{
    return m().userFrameBuffer();
}

std::shared_ptr<const graphics::IFrameBuffer> CameraNode::frameBuffer() const
{
    return const_cast<CameraNode*>(this)->frameBuffer();
}

void CameraNode::setFrameBuffer(const std::shared_ptr<graphics::IFrameBuffer> &value)
{
    auto &mPrivate = m();
    mPrivate.userFrameBuffer() = value;
    mPrivate.postprocessFrameBuffer() = nullptr; // reset to update
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
}

void CameraNode::setPerspectiveClipSpace(float fovY)
{
    auto &mPrivate = m();
    mPrivate.clipSpaceType() = utils::ClipSpaceType::Perspective;
    mPrivate.clipSpaceVerticalParam() = fovY;
}

const utils::Range &CameraNode::cullPlanesLimits() const
{
    return m().cullPlanesLimits();
}

void CameraNode::setCullPlanesLimits(const utils::Range &value)
{
    if (value.nearValue() <= 0.f)
        LOG_CRITICAL << "Znear must be greater than 0.0";

    if (value.farValue() <= value.nearValue())
        LOG_CRITICAL << "Zfar must be greater than Znear";

    m().cullPlanesLimits() = value;
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
