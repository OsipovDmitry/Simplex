#include "cameranodeprivate.h"

#include <utils/logger.h>

#include <core/bloom.h>
#include <core/cameranode.h>
#include <core/scene.h>
#include <core/shadowssettings.h>

#include "geometrybuffer.h"
#include "renderpipeline.h"
#include "sceneprivate.h"

namespace simplex
{
namespace core
{

CameraNodePrivate::CameraNodePrivate(CameraNode& cameraNode, const std::string& name)
    : NodePrivate(cameraNode, name)

{
}

CameraNodePrivate::~CameraNodePrivate() = default;

void CameraNodePrivate::onAttachToScene(const std::shared_ptr<Scene>& scene)
{
    if (!m_isDefaultFrameBufferUsed)
    {
        m_geometryBuffer = std::make_shared<GeometryBuffer>();
        m_geometryBuffer->setFixedSize(m_separateFramebufferFixedSize);
    }

    m_renderPipeLine = std::make_shared<RenderPipeLine>(scene->m().shadowAtlasSize());

    m_renderPipeLine->setShadowFilter(m_shadowsSettings->filter());
    m_renderPipeLine->setShadowBlurSigma(m_shadowsSettings->blurSigma());
    m_renderPipeLine->setShadowLightBleedingAmount(m_shadowsSettings->lightBleedingAmount());
    m_renderPipeLine->setShadowPositiveExponent(m_shadowsSettings->positiveExponent());
    m_renderPipeLine->setShadowNegativeExponent(m_shadowsSettings->negativeExponent());
    m_renderPipeLine->setShadowMomentsBias(m_shadowsSettings->momentsBias());
    m_renderPipeLine->setShadowDepthBiasFactor(m_shadowsSettings->depthBiasFactor());
    m_renderPipeLine->setShadowCascadesBlendDistanceFactor(m_shadowsSettings->cascadesBlendDistanceFactor());
    m_renderPipeLine->setShadowCascadesDistancePower(m_shadowsSettings->cascadesDistancePower());

    m_renderPipeLine->setBloomEnabled(m_bloom->isEnabled());
    m_renderPipeLine->setBloomContribution(m_bloom->contribution());
    m_renderPipeLine->setBloomPassesCount(m_bloom->passesCount());
    m_renderPipeLine->setBloomUpSamplePassBlurRadius(m_bloom->upSamplePassBlurRadius());
}

void CameraNodePrivate::onDetachFromScene(const std::shared_ptr<Scene>&)
{
    m_renderPipeLine.reset();
    m_geometryBuffer.reset();
}

bool& CameraNodePrivate::isRenderingEnabled()
{
    return m_isRenderingEnabled;
}

std::shared_ptr<GeometryBuffer>& CameraNodePrivate::geometryBuffer()
{
    return m_geometryBuffer;
}

std::shared_ptr<RenderPipeLine>& CameraNodePrivate::renderPipeLine()
{
    return m_renderPipeLine;
}

utils::ClipSpaceType& CameraNodePrivate::clipSpaceType()
{
    return m_clipSpaceType;
}

float& CameraNodePrivate::clipSpaceVerticalParam()
{
    return m_clipSpaceVerticalParam;
}

utils::ClipSpace& CameraNodePrivate::clipSpace()
{
    return m_clipSpace;
}

utils::Range& CameraNodePrivate::cullPlanesLimits()
{
    return m_cullPlanesLimits;
}

glm::uvec3& CameraNodePrivate::clusterSize()
{
    return m_clusterSize;
}

bool& CameraNodePrivate::isDefaultFrameBufferUsed()
{
    return m_isDefaultFrameBufferUsed;
}

std::optional<glm::uvec2>& CameraNodePrivate::separateFramebufferFixedSize()
{
    return m_separateFramebufferFixedSize;
}

std::unique_ptr<ShadowsSettings>& CameraNodePrivate::shadowsSettings()
{
    return m_shadowsSettings;
}

std::unique_ptr<Bloom>& CameraNodePrivate::bloom()
{
    return m_bloom;
}

void CameraNodePrivate::resize(const glm::uvec2& size)
{
    auto newSize = glm::max(size, glm::uvec2(1u, 1u));
    if (m_size == newSize) return;

    m_size = newSize;
    updateClipSpace();
}

void CameraNodePrivate::updateClipSpace()
{
    const auto viewportAspectRatio = static_cast<float>(m_size[0u]) / static_cast<float>(m_size[1u]);

    switch (m_clipSpaceType)
    {
        case utils::ClipSpaceType::Ortho:
        {
            m_clipSpace = utils::ClipSpace::makeOrtho(viewportAspectRatio, m_clipSpaceVerticalParam);
            break;
        }
        case utils::ClipSpaceType::Perspective:
        {
            m_clipSpace = utils::ClipSpace::makePerspective(viewportAspectRatio, m_clipSpaceVerticalParam);
            break;
        }
        default:
        {
            LOG_CRITICAL << "Undefined ClipSpaceType";
            break;
        }
    }
}

} // namespace core
} // namespace simplex
