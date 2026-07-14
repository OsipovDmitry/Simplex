#include "cameranodeprivate.h"

#include <utils/logger.h>

#include <core/scene.h>

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
    m_renderPipeLine->shadowFilter() = m_shadowFilter;
    m_renderPipeLine->shadowBlurSigma() = m_shadowBlurSigma;
    m_renderPipeLine->shadowLightBleedingAmount() = m_shadowLightBleedingAmount;
    m_renderPipeLine->shadowPositiveExponent() = m_shadowPositiveExponent;
    m_renderPipeLine->shadowNegativeExponent() = m_shadowNegativeExponent;
    m_renderPipeLine->shadowMomentsBias() = m_shadowMomentsBias;
    m_renderPipeLine->shadowDepthBiasFactor() = m_shadowDepthBiasFactor;
    m_renderPipeLine->shadowCascadesBlendDistanceFactor() = m_shadowCascadesBlendDistanceFactor;
    m_renderPipeLine->shadowCascadesDistancePower() = m_shadowCascadesDistancePower;
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

ShadowFilter& CameraNodePrivate::shadowFilter()
{
    return m_shadowFilter;
}

float& CameraNodePrivate::shadowBlurSigma()
{
    return m_shadowBlurSigma;
}

float& CameraNodePrivate::shadowLightBleedingAmount()
{
    return m_shadowLightBleedingAmount;
}
float& CameraNodePrivate::shadowPositiveExponent()
{
    return m_shadowPositiveExponent;
}

float& CameraNodePrivate::shadowNegativeExponent()
{
    return m_shadowNegativeExponent;
}

float& CameraNodePrivate::shadowMomentsBias()
{
    return m_shadowMomentsBias;
}

float& CameraNodePrivate::shadowDepthBiasFactor()
{
    return m_shadowDepthBiasFactor;
}

float& CameraNodePrivate::shadowCascadesBlendDistanceFactor()
{
    return m_shadowCascadesBlendDistanceFactor;
}

float& CameraNodePrivate::shadowCascadesDistancePower()
{
    return m_shadowCascadesDistancePower;
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
