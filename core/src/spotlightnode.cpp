#include <utils/logger.h>

#include <core/spotlightnode.h>

#include "graphicsengineprivate.h"
#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

SpotLightNode::SpotLightNode(const std::string& name)
    : ShadowedLightNode(std::make_unique<SpotLightNodePrivate>(*this, name))
{
    setColor(glm::vec3(1.f));
    setRadiuses(glm::vec2(1.f, 2.f));
    setHalfAngles(glm::quarter_pi<float>() * glm::vec2(0.f, 1.f));
}

SpotLightNode::~SpotLightNode() = default;

std::shared_ptr<SpotLightNode> SpotLightNode::asSpotLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<SpotLightNode>(wp.lock());
}

std::shared_ptr<const SpotLightNode> SpotLightNode::asSpotLightNode() const
{
    return const_cast<SpotLightNode*>(this)->asSpotLightNode();
}

const utils::Range& SpotLightNode::radiuses() const
{
    return m().radiuses();
}

void SpotLightNode::setRadiuses(const utils::Range& value)
{
    if (value.nearValue() < 0.f) LOG_CRITICAL << "Radiuses' near value must be greater or equal than 0.0";
    if (value.isEmpty()) LOG_CRITICAL << "Radiuses' range can't be empty";

    auto& mPrivate = m();
    mPrivate.radiuses() = value;
    mPrivate.onChanged();
}

const utils::Range& SpotLightNode::halfAngles() const
{
    return m().halfAngles();
}

void SpotLightNode::setHalfAngles(const utils::Range& value)
{
    if (value.nearValue() < 0.f) LOG_CRITICAL << "Half angles' near value must be greater or equal than 0.0";
    if (value.isEmpty()) LOG_CRITICAL << "Half angles' range can't be empty";
    if (value.farValue() >= glm::half_pi<float>()) LOG_CRITICAL << "Half angles' far value must be less than half of pi";

    auto& mPrivate = m();
    mPrivate.halfAngles() = value;
    mPrivate.onChanged();
}

} // namespace core
} // namespace simplex
