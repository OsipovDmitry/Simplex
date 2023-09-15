#include <core/pointlightnode.h>

#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

PointLightNode::PointLightNode(const std::string &name)
    : LightNode(std::make_unique<PointLightNodePrivate>(name))
{
    setRadiuses(glm::vec2(1.f, 2.f));
}

LightNodeType PointLightNode::type() const
{
    return LightNodeType::Point;
}

PointLightNode::~PointLightNode() = default;

std::shared_ptr<PointLightNode> PointLightNode::asPointLightNode()
{
    return std::dynamic_pointer_cast<PointLightNode>(shared_from_this());
}

std::shared_ptr<const PointLightNode> PointLightNode::asPointLightNode() const
{
    return std::dynamic_pointer_cast<const PointLightNode>(shared_from_this());
}

const glm::vec2 &PointLightNode::radiuses() const
{
    return m().radiuses();
}

void PointLightNode::setRadiuses(const glm::vec2 &value)
{
    assert(value[0] >= 0.f);
    assert(value[1] > value[0]);

    m().radiuses() = value;
}

}
}
