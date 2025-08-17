#ifndef CORE_LIGHTNODE_H
#define CORE_LIGHTNODE_H

#include <utils/pimpl.h>
#include <utils/enumclass.h>

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

class LightNodePrivate;
class CORE_SHARED_EXPORT LightNode : public Node
{
    PRIVATE_IMPL(LightNode)
public:
    ~LightNode() override;

    std::shared_ptr<LightNode> asLightNode() override;
    std::shared_ptr<const LightNode> asLightNode() const override;

    LightType type() const;

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

    Shadow &shadow();
    const Shadow &shadow() const;

protected:
    LightNode(std::unique_ptr<LightNodePrivate>);

};

}
}

#endif // CORE_LIGHTNODE_H
