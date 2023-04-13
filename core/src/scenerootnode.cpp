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
    return std::dynamic_pointer_cast<SceneRootNode>(shared_from_this());
}

std::shared_ptr<const SceneRootNode> SceneRootNode::asSceneRootNode() const
{
    return std::dynamic_pointer_cast<const SceneRootNode>(shared_from_this());
}

std::shared_ptr<Scene> SceneRootNode::scene()
{
    return m().scene()->shared_from_this();
}

SceneRootNode::SceneRootNode(const std::string &name, Scene *scene)
    : Node(new SceneRootNodePrivate(name, scene))
{
}

}
}
