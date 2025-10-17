#include <utils/logger.h>

#include <core/scene.h>
#include <core/spotlightnode.h>

#include "graphicsengineprivate.h"
#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

SpotLightNode::SpotLightNode(const std::string &name)
    : LightNode(std::make_unique<SpotLightNodePrivate>(*this, name))
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

const glm::vec3 &SpotLightNode::color() const
{
    return m().color();
}

void SpotLightNode::setColor(const glm::vec3 &value)
{
    auto& mPrivate = m();

    mPrivate.color() = value;
    mPrivate.changeInSceneData();
}

const glm::vec2 &SpotLightNode::radiuses() const
{
    return m().radiuses();
}

void SpotLightNode::setRadiuses(const glm::vec2 &value)
{
    if (value[0] < 0.f)
        LOG_CRITICAL << "minRadius must be greater or equal than 0.0";

    if (value[1] <= value[0])
        LOG_CRITICAL << "maxRadius must be greater than minRadius";

    auto& mPrivate = m();
    mPrivate.radiuses() = value;
    mPrivate.changeInSceneData();

    mPrivate.dirtyAreaMatrix();
    mPrivate.dirtyAreaBoundingBox();
}

const glm::vec2 &SpotLightNode::halfAngles() const
{
    return m().halfAngles();
}

void SpotLightNode::setHalfAngles(const glm::vec2 &value)
{
    if (value[0] < 0.f)
        LOG_CRITICAL << "Min angle must be greater or equal than 0.0";

    if (value[0] >= value[1])
        LOG_CRITICAL << "Max angle must be greater than min angle";

    if (value[1] >= glm::half_pi<float>())
        LOG_CRITICAL << "Max angle must be less than pi/2";

    auto& mPrivate = m();
    mPrivate.halfAngles() = value;
    mPrivate.changeInSceneData();

    mPrivate.dirtyAreaMatrix();
    mPrivate.dirtyAreaBoundingBox();
}

}
}
