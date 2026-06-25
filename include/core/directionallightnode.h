#ifndef CORE_DIRECTIONALLIGHTNODE_H
#define CORE_DIRECTIONALLIGHTNODE_H

#include <core/shadowedlightnode.h>

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate;
class CORE_SHARED_EXPORT DirectionalLightNode : public ShadowedLightNode
{
    PRIVATE_IMPL(DirectionalLightNode)
public:
    DirectionalLightNode(const std::string&);
    ~DirectionalLightNode() override;

    std::shared_ptr<DirectionalLightNode> asDirectionalLightNode() override;
    std::shared_ptr<const DirectionalLightNode> asDirectionalLightNode() const override;

    uint32_t shadowCascadesCount() const;
    void setShadowCascadesCount(uint32_t);
};

} // namespace core
} // namespace simplex

#endif // CORE_DIRECTIONALLIGHTNODE_H
