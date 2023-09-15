#include <core/directionallightnode.h>

#include "directionallightnodeprivate.h"

namespace simplex
{
namespace core
{

DirectionalLightNode::DirectionalLightNode(const std::string &name)
    : LightNode(std::make_unique<DirectionalLightNodePrivate>(name))
{
}

DirectionalLightNode::~DirectionalLightNode() = default;

LightNodeType DirectionalLightNode::type() const
{
    return LightNodeType::Directional;
}

std::shared_ptr<DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode()
{
    return std::dynamic_pointer_cast<DirectionalLightNode>(shared_from_this());
}

std::shared_ptr<const DirectionalLightNode> DirectionalLightNode::asDirectionalLightNode() const
{
    return std::dynamic_pointer_cast<const DirectionalLightNode>(shared_from_this());
}

}
}
