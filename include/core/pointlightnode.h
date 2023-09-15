#ifndef CORE_POINTLIGHTNODE_H
#define CORE_POINTLIGHTNODE_H

#include <utils/glm/vec2.hpp>

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class PointLightNodePrivate;
class CORE_SHARED_EXPORT PointLightNode : public LightNode
{
    PIMPL(PointLightNode)
public:
    PointLightNode(const std::string&);
    ~PointLightNode() override;

    LightNodeType type() const override;

    std::shared_ptr<PointLightNode> asPointLightNode() override;
    std::shared_ptr<const PointLightNode> asPointLightNode() const override;

    const glm::vec2 &radiuses() const;
    void setRadiuses(const glm::vec2&);

};

}
}

#endif // CORE_POINTLIGHTNODE_H
