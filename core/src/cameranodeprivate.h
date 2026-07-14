#ifndef CORE_CAMERANODEPRIVATE_H
#define CORE_CAMERANODEPRIVATE_H

#include <optional>

#include <utils/clipspace.h>

#include <core/cameranode.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class GeometryBuffer;
class RenderPipeLine;

class CameraNodePrivate : public NodePrivate
{
public:
    CameraNodePrivate(CameraNode&, const std::string&);
    ~CameraNodePrivate() override;

    void onAttachToScene(const std::shared_ptr<Scene>&) override;
    void onDetachFromScene(const std::shared_ptr<Scene>&) override;

    bool& isRenderingEnabled();

    std::shared_ptr<GeometryBuffer>& geometryBuffer();
    std::shared_ptr<RenderPipeLine>& renderPipeLine();

    utils::ClipSpaceType& clipSpaceType();
    float& clipSpaceVerticalParam();
    utils::ClipSpace& clipSpace();

    utils::Range& cullPlanesLimits();
    glm::uvec3& clusterSize();

    bool& isDefaultFrameBufferUsed();
    std::optional<glm::uvec2>& separateFramebufferFixedSize();

    ShadowFilter& shadowFilter();
    float& shadowBlurSigma();
    float& shadowLightBleedingAmount();
    float& shadowPositiveExponent();
    float& shadowNegativeExponent();
    float& shadowMomentsBias();
    float& shadowDepthBiasFactor();
    float& shadowCascadesBlendDistanceFactor();
    float& shadowCascadesDistancePower();

    void resize(const glm::uvec2&);
    void updateClipSpace();

private:
    bool m_isRenderingEnabled = true;
    glm::uvec2 m_size = glm::uvec2(0u);

    std::shared_ptr<GeometryBuffer> m_geometryBuffer;
    std::shared_ptr<RenderPipeLine> m_renderPipeLine;

    utils::ClipSpaceType m_clipSpaceType = utils::ClipSpaceType::Perspective;
    float m_clipSpaceVerticalParam = glm::pi<float>() / 3.f;
    utils::ClipSpace m_clipSpace = utils::ClipSpace();

    utils::Range m_cullPlanesLimits = utils::Range();
    glm::uvec3 m_clusterSize = glm::uvec3();

    bool m_isDefaultFrameBufferUsed = true;
    std::optional<glm::uvec2> m_separateFramebufferFixedSize;

    ShadowFilter m_shadowFilter = ShadowFilter::Discrete;
    float m_shadowBlurSigma = 1.f;
    float m_shadowLightBleedingAmount = .2f;
    float m_shadowPositiveExponent = 1.f;
    float m_shadowNegativeExponent = 1.f;
    float m_shadowMomentsBias = 0.f;
    float m_shadowDepthBiasFactor = 0.f;
    float m_shadowCascadesBlendDistanceFactor = .15f;
    float m_shadowCascadesDistancePower = 1.5f;
};

} // namespace core
} // namespace simplex

#endif // CORE_CAMERANODEPRIVATE_H
