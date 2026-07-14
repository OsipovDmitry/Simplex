#include <utils/clipspace.h>
#include <utils/logger.h>

#include <core/cameranode.h>
#include <core/settings.h>

#include "cameranodeprivate.h"
#include "geometrybuffer.h"
#include "renderpipeline.h"

namespace simplex
{
namespace core
{

CameraNode::CameraNode(const std::string& name)
    : Node(std::make_unique<CameraNodePrivate>(*this, name))
{
    setRenderingEnabled(true);

    const auto& graphicsSettings = settings::Settings::instance().graphics();
    const auto& cameraSettings = graphicsSettings.camera();
    const auto& shadowSettings = graphicsSettings.shadow();

    (cameraSettings.clipSpace().type() == utils::ClipSpaceType::Perspective)
        ? setPerspectiveClipSpace(cameraSettings.clipSpace().perspectiveFOV())
        : setOrthoClipSpace(cameraSettings.clipSpace().orthoHeight());

    setCullPlanesLimits(graphicsSettings.cullPlaneLimits());
    setClusterSize(cameraSettings.clusterSize());
    useDefaultFramebuffer();
    setShadowFilter(shadowSettings.filter());
    setShadowBlurSigma(shadowSettings.blurSigma());
    setShadowLightBleedingAmount(shadowSettings.lightBleedingAmount());
    setShadowPositiveExponent(shadowSettings.positiveExponent());
    setShadowNegativeExponent(shadowSettings.negativeExponent());
    setShadowMomentsBias(shadowSettings.momentsBias());
    setShadowDepthsBiasFactor(shadowSettings.depthBiasFactor());
    setShadowCascadesBlendDistanceFactor(shadowSettings.cascadesBlendDistanceFactor());
    setShadowCascadesDistancePower(shadowSettings.cascadesDistancePower());
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

ShadowFilter CameraNode::shadowFilter() const
{
    return m().shadowFilter();
}

void CameraNode::setShadowFilter(ShadowFilter value)
{
    auto& mPrivate = m();
    if (mPrivate.shadowFilter() != value)
    {
        mPrivate.shadowFilter() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowFilter() = mPrivate.shadowFilter();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowBlurSigma() const
{
    return m().shadowBlurSigma();
}

void CameraNode::setShadowBlurSigma(float value)
{
    auto& mPrivate = m();
    if (mPrivate.shadowBlurSigma() != value)
    {
        mPrivate.shadowBlurSigma() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowBlurSigma() = mPrivate.shadowBlurSigma();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowLightBleedingAmount() const
{
    return m().shadowLightBleedingAmount();
}

void CameraNode::setShadowLightBleedingAmount(float value)
{
    auto& mPrivate = m();
    if (mPrivate.shadowLightBleedingAmount() != value)
    {
        mPrivate.shadowLightBleedingAmount() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowLightBleedingAmount() = mPrivate.shadowLightBleedingAmount();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowPositiveExponent() const
{
    return m().shadowPositiveExponent();
}

void CameraNode::setShadowPositiveExponent(float value)
{
    auto& mPrivate = m();
    if (mPrivate.shadowPositiveExponent() != value)
    {
        mPrivate.shadowPositiveExponent() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowPositiveExponent() = mPrivate.shadowPositiveExponent();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowNegativeExponent() const
{
    return m().shadowNegativeExponent();
}

void CameraNode::setShadowNegativeExponent(float value)
{
    auto& mPrivate = m();
    if (mPrivate.shadowNegativeExponent() != value)
    {
        mPrivate.shadowNegativeExponent() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowNegativeExponent() = mPrivate.shadowNegativeExponent();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowMomentsBias() const
{
    return m().shadowMomentsBias();
}

void CameraNode::setShadowMomentsBias(float value)
{
    auto& mPrivate = m();
    if (mPrivate.shadowMomentsBias() != value)
    {
        mPrivate.shadowMomentsBias() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowMomentsBias() = mPrivate.shadowMomentsBias();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowDepthBiasFactor() const
{
    return m().shadowDepthBiasFactor();
}

void CameraNode::setShadowDepthsBiasFactor(float value)
{
    auto& mPrivate = m();
    if (mPrivate.shadowDepthBiasFactor() != value)
    {
        mPrivate.shadowDepthBiasFactor() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowDepthBiasFactor() = mPrivate.shadowDepthBiasFactor();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowCascadesBlendDistanceFactor() const
{
    return m().shadowCascadesBlendDistanceFactor();
}

void CameraNode::setShadowCascadesBlendDistanceFactor(float value) const
{
    if (value < 0.f)
    {
        LOG_CRITICAL << "Cascades blend distance factor can't be less than 0.0";
        return;
    }

    if (value > 1.f)
    {
        LOG_CRITICAL << "Cascades blend distance factor can't be more than 1.0";
        return;
    }

    auto& mPrivate = m();
    if (mPrivate.shadowCascadesBlendDistanceFactor() != value)
    {
        mPrivate.shadowCascadesBlendDistanceFactor() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowCascadesBlendDistanceFactor() = mPrivate.shadowCascadesBlendDistanceFactor();
            renderPipeLine->deinitialize();
        }
    }
}

float CameraNode::shadowCascadesDistancePower() const
{
    return m().shadowCascadesDistancePower();
}

void CameraNode::setShadowCascadesDistancePower(float value) const
{
    if (value <= 0.f)
    {
        LOG_CRITICAL << "Cascades distance power can't be less or equal than 0.0";
        return;
    }

    auto& mPrivate = m();
    if (mPrivate.shadowCascadesDistancePower() != value)
    {
        mPrivate.shadowCascadesDistancePower() = value;

        if (auto& renderPipeLine = mPrivate.renderPipeLine())
        {
            renderPipeLine->shadowCascadesDistancePower() = mPrivate.shadowCascadesDistancePower();
            renderPipeLine->deinitialize();
        }
    }
}

} // namespace core
} // namespace simplex
