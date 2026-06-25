#include <utils/logger.h>

#include <core/pointlightnode.h>

#include "graphicsengineprivate.h"
#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

PointLightNode::PointLightNode(const std::string& name)
    : ShadowedLightNode(std::make_unique<PointLightNodePrivate>(*this, name))
{
    setRadiuses(utils::Range(1.f, 2.f));
}

PointLightNode::~PointLightNode() = default;

std::shared_ptr<PointLightNode> PointLightNode::asPointLightNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<PointLightNode>(wp.lock());
}

std::shared_ptr<const PointLightNode> PointLightNode::asPointLightNode() const
{
    return const_cast<PointLightNode*>(this)->asPointLightNode();
}

const utils::Range& PointLightNode::radiuses() const
{
    return m().radiuses();
}

void PointLightNode::setRadiuses(const utils::Range& value)
{
    if (value.nearValue() < 0.f) LOG_CRITICAL << "Radiuses' near value must be greater or equal than 0.0";
    if (value.isEmpty()) LOG_CRITICAL << "Radiuses' range can't be empty";

    auto& mPrivate = m();
    mPrivate.radiuses() = value;
    mPrivate.onChanged();
}

} // namespace core
} // namespace simplex
