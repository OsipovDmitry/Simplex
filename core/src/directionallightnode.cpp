#include <utils/logger.h>

#include <core/scene.h>
#include <core/directionallightnode.h>

#include "graphicsengineprivate.h"
#include "directionallightnodeprivate.h"

namespace simplex
{
namespace core
{

DirectionalLightNode::DirectionalLightNode(const std::string &name)
    : LightNode(std::make_unique<DirectionalLightNodePrivate>(*this, name))
{
    setColor(glm::vec3(1.f));
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

const glm::vec3 &DirectionalLightNode::color() const
{
    return m().color();
}

void DirectionalLightNode::setColor(const glm::vec3 &value)
{
    auto& mPrivate = m();

    mPrivate.color() = value;
    mPrivate.changeInSceneData();
}

}
}
