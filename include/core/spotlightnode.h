#ifndef CORE_SPOTLIGHTNODE_H
#define CORE_SPOTLIGHTNODE_H

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>

#include <core/shadowedlightnode.h>

namespace simplex
{
namespace core
{

class SpotLightNodePrivate;
class CORE_SHARED_EXPORT SpotLightNode : public ShadowedLightNode
{
    PRIVATE_IMPL(SpotLightNode)
public:
    SpotLightNode(const std::string&);
    ~SpotLightNode() override;

    std::shared_ptr<SpotLightNode> asSpotLightNode() override;
    std::shared_ptr<const SpotLightNode> asSpotLightNode() const override;

    const utils::Range& radiuses() const;
    void setRadiuses(const utils::Range&);

    const utils::Range& halfAngles() const;
    void setHalfAngles(const utils::Range&);
};

} // namespace core
} // namespace simplex

#endif // CORE_SPOTLIGHTNODE_H
