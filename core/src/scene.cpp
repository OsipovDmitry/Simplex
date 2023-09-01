#include <core/scene.h>
#include <core/scenerootnode.h>

#include "sceneprivate.h"

namespace simplex
{
namespace core
{

const std::string SceneRootNodeName = "SceneRootNode";

Scene::Scene(std::weak_ptr<GraphicsEngine> graphicsEngine, const std::string &name)
    : m_(std::make_unique<ScenePrivate>(graphicsEngine, name, std::shared_ptr<SceneRootNode>(new SceneRootNode(SceneRootNodeName))))
{
}

Scene::~Scene()
{
}

const std::string &Scene::name() const
{
    return m_->name();
}

std::weak_ptr<const GraphicsEngine> Scene::graphicsEngine() const
{
    return const_cast<Scene*>(this)->graphicsEngine();
}

std::weak_ptr<GraphicsEngine> Scene::graphicsEngine()
{
    return m_->graphicsEngine();
}

std::shared_ptr<const SceneRootNode> Scene::sceneRootNode() const
{
    return const_cast<Scene*>(this)->sceneRootNode();
}

std::shared_ptr<SceneRootNode> Scene::sceneRootNode()
{
    return m_->sceneRootNode();
}


}
}
