#ifndef CORE_DIRECTIONALLIGHTNODE_H
#define CORE_DIRECTIONALLIGHTNODE_H

#include <utils/glm/vec3.hpp>

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate;
class CORE_SHARED_EXPORT DirectionalLightNode : public LightNode
{
    PRIVATE_IMPL(DirectionalLightNode)
public:
    DirectionalLightNode(const std::string&);
    ~DirectionalLightNode() override;

    std::shared_ptr<DirectionalLightNode> asDirectionalLightNode() override;
    std::shared_ptr<const DirectionalLightNode> asDirectionalLightNode() const override;

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);
};

}
}

#endif // CORE_DIRECTIONALLIGHTNODE_H
