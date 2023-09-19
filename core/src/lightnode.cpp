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

bool LightNode::isLightingEnabled() const
{
    return m().isLightingEnabled();
}

void LightNode::setLightingEnabled(bool value)
{
    m().isLightingEnabled() = value;
}

const glm::mat4x4 &LightNode::areaMatrix() const
{
    auto &mPrivate = m();
    if (mPrivate.isAreaMatrixDirty())
    {
        mPrivate.areaMatrix() = doAreaMatrix();
        mPrivate.isAreaMatrixDirty() = false;
    }
    return mPrivate.areaMatrix();
}

std::shared_ptr<const DrawableBase> LightNode::areaDrawable() const
{
    return m().areaDrawable();
}

LightNode::LightNode(std::unique_ptr<LightNodePrivate> lightNodePrivate)
    : Node(std::move(lightNodePrivate))
{
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
