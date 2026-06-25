#ifndef CORE_SHADOWEDLIGHTNODE_H
#define CORE_SHADOWEDLIGHTNODE_H

#include <core/coloredlightnode.h>

namespace simplex
{
namespace core
{

class ShadowedLightNodePrivate;
class CORE_SHARED_EXPORT ShadowedLightNode : public ColoredLightNode
{
    PRIVATE_IMPL(ShadowedLightNode)
public:
    ~ShadowedLightNode() override;

    virtual std::shared_ptr<DirectionalLightNode> asDirectionalLightNode();
    virtual std::shared_ptr<const DirectionalLightNode> asDirectionalLightNode() const;

    virtual std::shared_ptr<PointLightNode> asPointLightNode();
    virtual std::shared_ptr<const PointLightNode> asPointLightNode() const;

    virtual std::shared_ptr<SpotLightNode> asSpotLightNode();
    virtual std::shared_ptr<const SpotLightNode> asSpotLightNode() const;

    bool isShadingEnabled() const;
    void setShadingEnabled(bool);

    uint32_t shadowMapSize() const;
    void setShadowMapSize(uint32_t);

    const utils::Range& shadowCullPlanesLimits() const;
    void setShadowCullPlanesLimits(const utils::Range&);

    bool isVolumetricScatteringEnabled() const;
    void setVolumetricScatteringEnabled(bool);

protected:
    ShadowedLightNode(std::unique_ptr<ShadowedLightNodePrivate>);
};

} // namespace core
} // namespace simplex

#endif // CORE_SHADOWEDLIGHTNODE_H
