#ifndef CORE_LIGHTNODE_H
#define CORE_LIGHTNODE_H

#include <utils/glm/vec3.hpp>
#include <utils/pimpl.h>
#include <utils/enumclass.h>

#include <core/forwarddecl.h>
#include <core/node.h>

namespace simplex
{
namespace core
{

ENUMCLASS(LightNodeType, uint16_t,
          Point,
          Spot,
          Directional);

class LightNodePrivate;
class CORE_SHARED_EXPORT LightNode : public Node
{
    PIMPL(LightNode)
public:
    ~LightNode() override;

    virtual LightNodeType type() const = 0;

    std::shared_ptr<LightNode> asLightNode() override;
    std::shared_ptr<const LightNode> asLightNode() const override;

    virtual std::shared_ptr<PointLightNode> asPointLightNode();
    virtual std::shared_ptr<const PointLightNode> asPointLightNode() const;

    virtual std::shared_ptr<SpotLightNode> asSpotLightNode();
    virtual std::shared_ptr<const SpotLightNode> asSpotLightNode() const;

    virtual std::shared_ptr<DirectionalLightNode> asDirectionalLightNode();
    virtual std::shared_ptr<const DirectionalLightNode> asDirectionalLightNode() const;

    bool isLightingEnabled() const;
    void setLightingEnabled(bool);

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);

protected:
    LightNode(std::unique_ptr<LightNodePrivate>);

    bool canAttach(std::shared_ptr<Node>) override;
    bool canDetach(std::shared_ptr<Node>) override;
};

}
}

#endif // CORE_LIGHTNODE_H
