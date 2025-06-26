#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/meshpainter.h>

#include <core/graphicsengine.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/listenernode.h>
#include <core/settings.h>

#include "sceneprivate.h"

namespace simplex
{
namespace core
{

Scene::~Scene() = default;

const std::string &Scene::name() const
{
    return m_->name();
}

std::shared_ptr<ApplicationBase> Scene::application()
{
    auto wpApplication = m().application();
    return wpApplication.expired() ? nullptr : wpApplication.lock();
}

std::shared_ptr<const ApplicationBase> Scene::application() const
{
    return const_cast<Scene*>(this)->application();
}

std::shared_ptr<SceneRootNode> Scene::sceneRootNode()
{
    return m_->sceneRootNode();
}

std::shared_ptr<const SceneRootNode> Scene::sceneRootNode() const
{
    return const_cast<Scene*>(this)->sceneRootNode();
}

std::shared_ptr<ListenerNode> Scene::listenerNode()
{
    return m_->listenerNode();
}

std::shared_ptr<const ListenerNode> Scene::listenerNode() const
{
    return const_cast<Scene*>(this)->listenerNode();
}

Background &Scene::background()
{
    return m_->background();
}

const Background &Scene::background() const
{
    return const_cast<Scene*>(this)->background();
}

const std::unordered_set<std::shared_ptr<Animation>> &Scene::animations()
{
    return m_->animations();
}

void Scene::removeAnimation(const std::shared_ptr<Animation> &value)
{
    if (auto it = m_->animations().find(value); it != m_->animations().end())
        m_->animations().erase(it);
}

void Scene::addAnimation(const std::shared_ptr<Animation> &value)
{
    m_->animations().insert(value);
}

Scene::Scene(const std::weak_ptr<ApplicationBase> &application, const std::string &name)
    : m_(std::make_unique<ScenePrivate>(application, name))
{
    m_->listenerNode() = std::shared_ptr<ListenerNode>(new ListenerNode(settings::Settings::instance().application().scene().listenerNodeName()));

    m_->sceneRootNode() = std::shared_ptr<SceneRootNode>(new SceneRootNode(settings::Settings::instance().application().scene().sceneRootNodeName()));
    m_->sceneRootNode()->attach(m_->listenerNode());
}



}
}
