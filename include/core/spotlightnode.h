#ifndef CORE_SPOTLIGHTNODE_H
#define CORE_SPOTLIGHTNODE_H

#include <utils/glm/vec2.hpp>

#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class SpotLightNodePrivate;
class CORE_SHARED_EXPORT SpotLightNode : public LightNode
{
    PIMPL(SpotLightNode)
public:
    SpotLightNode(const std::string&);
    ~SpotLightNode() override;

    std::shared_ptr<SpotLightNode> asSpotLightNode() override;
    std::shared_ptr<const SpotLightNode> asSpotLightNode() const override;

    const glm::vec3 &color() const;
    void setColor(const glm::vec3&);

    const glm::vec2 &radiuses() const;
    void setRadiuses(const glm::vec2&);

    const glm::vec2 &halfAngles() const;
    void setHalfAngles(const glm::vec2&);

protected:
    glm::mat4x4 doAreaMatrix() const override;
    utils::BoundingBox doAreaBoundingBox() const override;
};

}
}

#endif // CORE_SPOTLIGHTNODE_H
