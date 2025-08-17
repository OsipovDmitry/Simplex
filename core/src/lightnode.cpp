#include <core/lightnode.h>

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

LightNode::~LightNode() = default;

std::shared_ptr<LightNode> LightNode::asLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<LightNode>(wp.lock());
}

std::shared_ptr<const LightNode> LightNode::asLightNode() const
{
    return const_cast<LightNode*>(this)->asLightNode();
}

LightType LightNode::type() const
{
    return m().type();
}

std::shared_ptr<PointLightNode> LightNode::asPointLightNode()
{
    return nullptr;
}

std::shared_ptr<const PointLightNode> LightNode::asPointLightNode() const
{
    return nullptr;
}

std::shared_ptr<SpotLightNode> LightNode::asSpotLightNode()
{
    return nullptr;
}

std::shared_ptr<const SpotLightNode> LightNode::asSpotLightNode() const
{
    return nullptr;
}

std::shared_ptr<DirectionalLightNode> LightNode::asDirectionalLightNode()
{
    return nullptr;
}

std::shared_ptr<const DirectionalLightNode> LightNode::asDirectionalLightNode() const
{
    return nullptr;
}

std::shared_ptr<IBLLightNode> LightNode::asIBLLightNode()
{
    return nullptr;
}

std::shared_ptr<const IBLLightNode> LightNode::asIBLLightNode() const
{
    return nullptr;
}

bool LightNode::isLightingEnabled() const
{
    return m().isLightingEnabled();
}

void LightNode::setLightingEnabled(bool value)
{
    m().isLightingEnabled() = value;
}

Shadow &LightNode::shadow()
{
    return m().shadow();
}

const Shadow &LightNode::shadow() const
{
    return const_cast<LightNode*>(this)->shadow();
}

LightNode::LightNode(std::unique_ptr<LightNodePrivate> lightNodePrivate)
    : Node(std::move(lightNodePrivate))
{
    setLightingEnabled(true);
}

}
}
