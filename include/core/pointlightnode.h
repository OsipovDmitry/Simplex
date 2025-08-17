#ifndef CORE_POINTLIGHTNODE_H
#define CORE_POINTLIGHTNODE_H

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class PointLightNodePrivate;
class CORE_SHARED_EXPORT PointLightNode : public LightNode
{
    PRIVATE_IMPL(PointLightNode)
public:
    PointLightNode(const std::string&);
    ~PointLightNode() override;

    std::shared_ptr<PointLightNode> asPointLightNode() override;
    std::shared_ptr<const PointLightNode> asPointLightNode() const override;

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);

    const glm::vec2 &radiuses() const;
    void setRadiuses(const glm::vec2&);

};

}
}

#endif // CORE_POINTLIGHTNODE_H
