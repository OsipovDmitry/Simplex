#include <core/scenerootnode.h>
#include <core/scene.h>

#include "scenerootnodeprivate.h"

namespace simplex
{
namespace core
{

SceneRootNode::~SceneRootNode()
{
}

std::shared_ptr<SceneRootNode> SceneRootNode::asSceneRootNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<SceneRootNode>(wp.lock());
}

std::shared_ptr<const SceneRootNode> SceneRootNode::asSceneRootNode() const
{
    return const_cast<SceneRootNode*>(this)->asSceneRootNode();
}

std::shared_ptr<Scene> SceneRootNode::scene()
{
    auto wpScene = m().scene();
    return wpScene.expired() ? nullptr : wpScene.lock();
}

std::shared_ptr<const Scene> SceneRootNode::scene() const
{
    return const_cast<SceneRootNode*>(this)->scene();
}

SceneRootNode::SceneRootNode(const std::string &name)
    : Node(std::make_unique<SceneRootNodePrivate>(*this, name))
{
}

}
}
