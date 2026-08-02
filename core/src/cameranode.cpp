#include <utils/clipspace.h>
#include <utils/logger.h>

#include <core/bloom.h>
#include <core/cameranode.h>
#include <core/settings.h>
#include <core/shadowssettings.h>
#include <core/tonemapping.h>

#include "bloomprivate.h"
#include "cameranodeprivate.h"
#include "geometrybuffer.h"
#include "renderpipeline.h"
#include "shadowssettingsprivate.h"
#include "tonemappingprivate.h"

namespace simplex
{
namespace core
{

CameraNode::CameraNode(const std::string& name)
    : Node(std::make_unique<CameraNodePrivate>(*this, name))
{
    auto& mPrivate = m();
    mPrivate.shadowsSettings() = std::make_unique<ShadowsSettings>(std::make_unique<ShadowsSettingsPrivate>(*this));
    mPrivate.bloom() = std::make_unique<Bloom>(std::make_unique<BloomPrivate>(*this));
    mPrivate.toneMapping() = std::make_unique<ToneMapping>(std::make_unique<ToneMappingPrivate>(*this));

    setRenderingEnabled(true);

    const auto& graphicsSettings = settings::Settings::instance().graphics();
    const auto& cameraSettings = graphicsSettings.camera();

    (cameraSettings.clipSpace().type() == utils::ClipSpaceType::Perspective)
        ? setPerspectiveClipSpace(cameraSettings.clipSpace().perspectiveFOV())
        : setOrthoClipSpace(cameraSettings.clipSpace().orthoHeight());

    setCullPlanesLimits(graphicsSettings.cullPlaneLimits());
    setClusterSize(cameraSettings.clusterSize());
    useDefaultFramebuffer();
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
    return m().isDefaultFrameBufferUsed();
}

const std::optional<glm::uvec2>& CameraNode::separateFrameBufferFixedSize() const
{
    return m().separateFramebufferFixedSize();
}

void CameraNode::useDefaultFramebuffer()
{
    auto& mPrivate = m();
    if (!mPrivate.isDefaultFrameBufferUsed())
    {
        mPrivate.isDefaultFrameBufferUsed() = true;
        mPrivate.separateFramebufferFixedSize().reset();
        mPrivate.geometryBuffer().reset();
    }
}

void CameraNode::useSeparateFramebuffer(const std::optional<glm::uvec2>& size)
{
    auto& mPrivate = m();
    if (mPrivate.isDefaultFrameBufferUsed() || (mPrivate.separateFramebufferFixedSize() != size))
    {
        mPrivate.isDefaultFrameBufferUsed() = false;
        mPrivate.separateFramebufferFixedSize() = size;

        if (auto& geometryBuffer = mPrivate.geometryBuffer())
        {
            geometryBuffer->setFixedSize(mPrivate.separateFramebufferFixedSize());
        }
    }
}

const utils::ClipSpace& CameraNode::clipSpace() const
{
    return m().clipSpace();
}

void CameraNode::setOrthoClipSpace(float height)
{
    auto& mPrivate = m();
    mPrivate.clipSpaceType() = utils::ClipSpaceType::Ortho;
    mPrivate.clipSpaceVerticalParam() = height;
    mPrivate.updateClipSpace();
}

void CameraNode::setPerspectiveClipSpace(float fovY)
{
    auto& mPrivate = m();
    mPrivate.clipSpaceType() = utils::ClipSpaceType::Perspective;
    mPrivate.clipSpaceVerticalParam() = fovY;
    mPrivate.updateClipSpace();
}

const utils::Range& CameraNode::cullPlanesLimits() const
{
    return m().cullPlanesLimits();
}

void CameraNode::setCullPlanesLimits(const utils::Range& value)
{
    if (value.nearValue() <= 0.f) LOG_CRITICAL << "ZNear must be greater than 0.0";

    if (value.farValue() <= value.nearValue()) LOG_CRITICAL << "ZFar must be greater than Znear";

    m().cullPlanesLimits() = value;
}

const glm::uvec3& CameraNode::clusterSize() const
{
    return m().clusterSize();
}

void CameraNode::setClusterSize(const glm::uvec3& value)
{
    m().clusterSize() = value;
}

ShadowsSettings& CameraNode::shadowsSettings()
{
    return *m().shadowsSettings();
}

const ShadowsSettings& CameraNode::shadowsSettings() const
{
    return const_cast<CameraNode*>(this)->shadowsSettings();
}

Bloom& CameraNode::bloom()
{
    return *m().bloom();
}

const Bloom& CameraNode::bloom() const
{
    return const_cast<CameraNode*>(this)->bloom();
}

} // namespace core
} // namespace simplex
