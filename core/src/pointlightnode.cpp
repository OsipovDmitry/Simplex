#include <utils/logger.h>

#include <core/scene.h>
#include <core/pointlightnode.h>

#include "graphicsengineprivate.h"
#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

PointLightNode::PointLightNode(const std::string &name)
    : LightNode(std::make_unique<PointLightNodePrivate>(*this, name))
{
    setColor(glm::vec3(1.f));
    setRadiuses(glm::vec2(1.f, 2.f));
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

const glm::vec3 &PointLightNode::color() const
{
    return m().color();
}

void PointLightNode::setColor(const glm::vec3 &value)
{
    auto& mPrivate = m();

    mPrivate.color() = value;
    mPrivate.changeInSceneData();
}

const glm::vec2 &PointLightNode::radiuses() const
{
    return m().radiuses();
}

void PointLightNode::setRadiuses(const glm::vec2 &value)
{
    if (value[0u] < 0.f)
        LOG_CRITICAL << "minRadius must be greater or equal than 0.0";

    if (value[1u] <= value[0])
        LOG_CRITICAL << "maxRadius must be greater than minRadius";

    auto &mPrivate = m();
    mPrivate.radiuses() = value;
    mPrivate.changeInSceneData();
}

}
}
