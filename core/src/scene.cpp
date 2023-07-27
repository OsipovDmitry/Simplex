#include <core/scene.h>
#include <core/scenerootnode.h>

#include "sceneprivate.h"

namespace simplex
{
namespace core
{

const std::string SceneRootNodeName = "SceneRootNode";

Scene::Scene(const std::string &name)
    : m_(std::make_unique<ScenePrivate>(name, std::shared_ptr<SceneRootNode>(new SceneRootNode(SceneRootNodeName))))
{
}

Scene::~Scene()
{
}

const std::string &Scene::name() const
{
    return m_->name();
}

std::shared_ptr<const SceneRootNode> Scene::sceneRootNode() const
{
    return m_->sceneRootNode();
}

std::shared_ptr<SceneRootNode> Scene::sceneRootNode()
{
    return m_->sceneRootNode();
}


}
}
