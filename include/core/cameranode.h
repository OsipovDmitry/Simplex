#ifndef CORE_CAMERANODE_H
#define CORE_CAMERANODE_H

#include <optional>

#include <utils/enumclass.h>
#include <utils/glm/vec3.hpp>
#include <utils/sortedobject.h>

#include <core/node.h>

namespace simplex
{
namespace core
{

ENUMCLASS(DrawDataCullingAlgorithm, uint16_t, Disabled, SuperFast, Fast, Correct)
ENUMCLASS(ShadowDataCullingAlgorithm, uint16_t, Disabled, SuperFast, Fast, Correct)
ENUMCLASS(SpotLightCullingAlgorithm, uint16_t, SuperFast, Fast, Correct)
ENUMCLASS(ShadowFilter, uint16_t, Discrete, VSM, EVSM, HamburgerMSM, HausdorffMSM)

class CameraNodePrivate;
class CORE_SHARED_EXPORT CameraNode : public Node, public utils::SortedObject
{
    PRIVATE_IMPL(CameraNode)
public:
    CameraNode(const std::string&);
    ~CameraNode() override;

    std::shared_ptr<CameraNode> asCameraNode() override;
    std::shared_ptr<const CameraNode> asCameraNode() const override;

    bool isRenderingEnabled() const;
    void setRenderingEnabled(bool);

    bool isDefaultFramebufferUsed() const;
    const std::optional<glm::uvec2>& separateFrameBufferFixedSize() const;
    void useDefaultFramebuffer();
    void useSeparateFramebuffer(const std::optional<glm::uvec2>& = std::nullopt);

    const utils::ClipSpace& clipSpace() const;
    void setOrthoClipSpace(float height);
    void setPerspectiveClipSpace(float fovY);

    const utils::Range& cullPlanesLimits() const;
    void setCullPlanesLimits(const utils::Range&);

    const glm::uvec3& clusterSize() const;
    void setClusterSize(const glm::uvec3&);

    ShadowFilter shadowFilter() const;
    void setShadowFilter(ShadowFilter);

    float shadowBlurSigma() const;
    void setShadowBlurSigma(float);

    float shadowLightBleedingAmount() const;
    void setShadowLightBleedingAmount(float);

    float shadowPositiveExponent() const;
    void setShadowPositiveExponent(float);

    float shadowNegativeExponent() const;
    void setShadowNegativeExponent(float);

    float shadowMomentsBias() const;
    void setShadowMomentsBias(float);

    float shadowDepthBiasFactor() const;
    void setShadowDepthsBiasFactor(float);
};

} // namespace core
} // namespace simplex

#endif // CORE_CAMERANODE_H
