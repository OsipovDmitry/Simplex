#include <core/spotlightnode.h>

#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

SpotLightNode::SpotLightNode(const std::string &name)
    : LightNode(std::make_unique<SpotLightNodePrivate>(name))
{
    setRadiuses(glm::vec2(1.f, 2.f));
    setHalfAngles(glm::vec2(.5f * glm::quarter_pi<float>(), glm::quarter_pi<float>()));
}

LightNodeType SpotLightNode::type() const
{
    return LightNodeType::Spot;
}

SpotLightNode::~SpotLightNode() = default;

std::shared_ptr<SpotLightNode> SpotLightNode::asSpotLightNode()
{
    return std::dynamic_pointer_cast<SpotLightNode>(shared_from_this());
}

std::shared_ptr<const SpotLightNode> SpotLightNode::asSpotLightNode() const
{
    return std::dynamic_pointer_cast<const SpotLightNode>(shared_from_this());
}

const glm::vec2 &SpotLightNode::radiuses() const
{
    return m().radiuses();
}

void SpotLightNode::setRadiuses(const glm::vec2 &value)
{
    assert(value[0] >= 0.f);
    assert(value[0] < value[1]);

    m().radiuses() = value;
}

const glm::vec2 &SpotLightNode::halfAngles() const
{
    return m().halfAngles();
}

void SpotLightNode::setHalfAngles(const glm::vec2 &value)
{
    assert(value[0] >= 0.f);
    assert(value[0] < value[1]);
    assert(value[1] < glm::half_pi<float>());

    m().halfAngles() = value;
}

}
}
