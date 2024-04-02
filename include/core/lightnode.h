#ifndef CORE_LIGHTNODE_H
#define CORE_LIGHTNODE_H

#include <utils/glm/vec3.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/pimpl.h>
#include <utils/enumclass.h>

#include <core/forwarddecl.h>
#include <core/node.h>

namespace simplex
{
namespace core
{

ENUMCLASS(LightType, uint16_t,
          Point,
          Spot,
          Directional,
          IBL)

ENUMCLASS(LightShadingMode, uint16_t,
          Disabled, // no shading
          Opaque, // opaque geometries cast balck shadow
          OpaqueAndTransparent, // all the geometries cast black shadow
          Color) // opaque geometries cast black shadow, transparent geometies cast color shadow

class LightNodePrivate;
class CORE_SHARED_EXPORT LightNode : public Node
{
    PIMPL(LightNode)
public:
    ~LightNode() override;

    std::shared_ptr<LightNode> asLightNode() override;
    std::shared_ptr<const LightNode> asLightNode() const override;

    utils::BoundingBox doBoundingBox() const override;

    virtual LightType type() const = 0;

    virtual std::shared_ptr<PointLightNode> asPointLightNode();
    virtual std::shared_ptr<const PointLightNode> asPointLightNode() const;

    virtual std::shared_ptr<SpotLightNode> asSpotLightNode();
    virtual std::shared_ptr<const SpotLightNode> asSpotLightNode() const;

    virtual std::shared_ptr<DirectionalLightNode> asDirectionalLightNode();
    virtual std::shared_ptr<const DirectionalLightNode> asDirectionalLightNode() const;

    virtual std::shared_ptr<IBLLightNode> asIBLLightNode();
    virtual std::shared_ptr<const IBLLightNode> asIBLLightNode() const;

    bool isLightingEnabled() const;
    void setLightingEnabled(bool);

    LightShadingMode shadingMode() const;
    void setShadingMode(LightShadingMode);

    const glm::uvec2 &shadowMapSize() const;
    void setShadowMapSize(const glm::uvec2 &);

    const utils::Range &shadowCullPlanesLimits() const;
    void setShadowCullPlanesLimits(const utils::Range&);

    void updateShadowFrameBuffer();
    std::vector<glm::mat4x4> updateLayeredShadowMatrices(const utils::FrustumCorners&,
                                                         const utils::Range&);

    const glm::mat4x4 &areaMatrix() const;
    std::shared_ptr<const LightDrawable> areaDrawable() const;
    const utils::BoundingBox &areaBoundingBox() const;
    void recalculateAreaBoundingBox();

protected:
    LightNode(std::unique_ptr<LightNodePrivate>);

    bool canAttach(const std::shared_ptr<Node>&) override;
    bool canDetach(const std::shared_ptr<Node>&) override;

    virtual glm::mat4x4 doAreaMatrix() const = 0;
    virtual utils::BoundingBox doAreaBoundingBox() const = 0;

    virtual glm::mat4x4 doUpdateLayeredShadowMatrices(const utils::FrustumCorners&,
                                                      const utils::Range&,
                                                      std::vector<glm::mat4x4>&) const = 0;
};

}
}

#endif // CORE_LIGHTNODE_H
