#include <utils/logger.h>

#include <core/directionallightnode.h>

#include "directionallightnodeprivate.h"
#include "shadow.h"

namespace simplex
{
namespace core
{

DirectionalLightNode::DirectionalLightNode(const std::string& name)
    : ShadowedLightNode(std::make_unique<DirectionalLightNodePrivate>(*this, name))
{
    setShadowCascadesCount(4u);
}

DirectionalLightNode::~DirectionalLightNode() = default;

std::shared_ptr<DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<DirectionalLightNode>(wp.lock());
}

std::shared_ptr<const DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode() const
{
    return const_cast<DirectionalLightNode*>(this)->asDirectionalLightNode();
}

uint32_t DirectionalLightNode::shadowCascadesCount() const
{
    return m().shadowCascadesCount();
}

void DirectionalLightNode::setShadowCascadesCount(uint32_t value)
{
    if (value < 1u)
    {
        LOG_CRITICAL << "Cascades count can't be less than 1";
        return;
    }

    if (value > 6u)
    {
        LOG_CRITICAL << "Cascades count can't be more than 6";
        return;
    }

    auto& mPrivate = m();
    mPrivate.shadowCascadesCount() = value;
    if (auto& shadow = mPrivate.shadow()) shadow->onChanged();
}

} // namespace core
} // namespace simplex
