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
    return std::dynamic_pointer_cast<LightNode>(shared_from_this());
}

std::shared_ptr<const LightNode> LightNode::asLightNode() const
{
    return std::dynamic_pointer_cast<const LightNode>(shared_from_this());
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

bool LightNode::isLightingEnabled() const
{
    return m().isLightingEnabled();
}

void LightNode::setLightingEnabled(bool value)
{
    m().isLightingEnabled() = value;
}

const glm::vec3 &LightNode::color() const
{
    return m().color();
}

void LightNode::setColor(const glm::vec3 &value)
{
    m().color() = value;
}

LightNode::LightNode(std::unique_ptr<LightNodePrivate> lightNodePrivate)
    : Node(std::move(lightNodePrivate))
{
    setColor(glm::vec3(1.f));
    setLightingEnabled(true);
}

bool LightNode::canAttach(std::shared_ptr<Node>)
{
    LOG_ERROR << "It's forbidden to attach to light node \"" << name() << "\"";
    return false;
}

bool LightNode::canDetach(std::shared_ptr<Node>)
{
    LOG_ERROR << "It's forbidden to detach from light node \"" << name() << "\"";
    return false;
}


}
}
