#ifndef CORE_POINTLIGHTNODE_H
#define CORE_POINTLIGHTNODE_H

#include <utils/glm/vec2.hpp>

#include <core/shadowedlightnode.h>

namespace simplex
{
namespace core
{

class PointLightNodePrivate;
class CORE_SHARED_EXPORT PointLightNode : public ShadowedLightNode
{
    PRIVATE_IMPL(PointLightNode)
public:
    PointLightNode(const std::string&);
    ~PointLightNode() override;

    std::shared_ptr<PointLightNode> asPointLightNode() override;
    std::shared_ptr<const PointLightNode> asPointLightNode() const override;

    const utils::Range& radiuses() const;
    void setRadiuses(const utils::Range&);
};

} // namespace core
} // namespace simplex

#endif // CORE_POINTLIGHTNODE_H
