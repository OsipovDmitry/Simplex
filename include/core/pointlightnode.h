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

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);

    const glm::vec2 &radiuses() const;
    void setRadiuses(const glm::vec2&);

protected:
    glm::mat4x4 doAreaMatrix() const override;

};

}
}

#endif // CORE_POINTLIGHTNODE_H
