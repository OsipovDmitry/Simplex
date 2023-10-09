#ifndef CORE_DIRECTIONALLIGHTNODE_H
#define CORE_DIRECTIONALLIGHTNODE_H

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate;
class CORE_SHARED_EXPORT DirectionalLightNode : public LightNode
{
    PIMPL(DirectionalLightNode)
public:
    DirectionalLightNode(const std::string&);
    ~DirectionalLightNode() override;

    std::shared_ptr<DirectionalLightNode> asDirectionalLightNode() override;
    std::shared_ptr<const DirectionalLightNode> asDirectionalLightNode() const override;

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);

    void doAfterTransformChanged() override;

protected:
    glm::mat4x4 doAreaMatrix() const override;
};

}
}

#endif // CORE_DIRECTIONALLIGHTNODE_H
