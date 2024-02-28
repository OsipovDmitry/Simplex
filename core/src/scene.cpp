#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>

#include <core/igraphicsrenderer.h>
#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/backgrounddrawable.h>

#include "sceneprivate.h"
#include "backgrounddrawableprivate.h"

namespace simplex
{
namespace core
{

const std::string SceneRootNodeName = "SceneRootNode";

Scene::~Scene() = default;

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

std::shared_ptr<const BackgroundDrawable> Scene::backgroundDrawable() const
{
    return const_cast<Scene*>(this)->backgroundDrawable();
}

std::shared_ptr<BackgroundDrawable> Scene::backgroundDrawable()
{
    return m_->backgroundDrawable();
}

Scene::Scene(const std::weak_ptr<GraphicsEngine> &graphicsEngine, const std::string &name)
    : m_(std::make_unique<ScenePrivate>(graphicsEngine, name, std::shared_ptr<SceneRootNode>(new SceneRootNode(SceneRootNodeName))))
{
    if (BackgroundDrawablePrivate::screenQuadVertexArray().expired())
        LOG_CRITICAL << "Background screen quad vertex array is expired";

    m_->backgroundDrawable() = std::make_shared<BackgroundDrawable>(BackgroundDrawablePrivate::screenQuadVertexArray().lock());
}



}
}
