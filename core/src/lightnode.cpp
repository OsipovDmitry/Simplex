#include <utils/logger.h>

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

std::shared_ptr<ColoredLightNode> LightNode::asColoredLightNode()
{
    return nullptr;
}

std::shared_ptr<const ColoredLightNode> LightNode::asColoredLightNode() const
{
    return nullptr;
}

std::shared_ptr<ImageBasedLightNode> LightNode::asIBLLightNode()
{
    return nullptr;
}

std::shared_ptr<const ImageBasedLightNode> LightNode::asIBLLightNode() const
{
    return nullptr;
}

bool LightNode::isLightingEnabled() const
{
    return m().isLightingEnabled();
}

void LightNode::setLightingEnabled(bool value)
{
    auto& mPrivate = m();
    mPrivate.isLightingEnabled() = value;
    mPrivate.onChanged();
}

LightNode::LightNode(std::unique_ptr<LightNodePrivate> lightNodePrivate)
    : Node(std::move(lightNodePrivate))
{
    setLightingEnabled(true);
}

} // namespace core
} // namespace simplex
