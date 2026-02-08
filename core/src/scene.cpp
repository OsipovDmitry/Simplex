#include <core/background.h>
#include <core/listenernode.h>
#include <core/scene.h>
#include <core/scenerootnode.h>
#include <core/settings.h>

#include "scenedata.h"
#include "sceneprivate.h"
#include "scenerootnodeprivate.h"

namespace simplex
{
namespace core
{

Scene::~Scene() = default;

const std::string &Scene::name() const
{
    return m_->name();
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

std::shared_ptr<Background> Scene::background()
{
    return m_->background();
}

std::shared_ptr<const Background> Scene::background() const
{
    return const_cast<Scene*>(this)->background();
}

std::shared_ptr<Scene> Scene::createEmpty(const std::string& sceneName)
{
    auto scene = std::shared_ptr<Scene>(new Scene(sceneName));
    scene->m().sceneRootNode()->m().scene() = scene;

    return scene;
}

Scene::Scene(const std::string &name)
    : m_(std::make_unique<ScenePrivate>(*this, name))
{
    m_->sceneData() = std::make_shared<SceneData>();
    m_->background() = std::make_shared<Background>();

    m_->sceneData()->setBackground(m_->background());

    m_->listenerNode() = std::shared_ptr<ListenerNode>(new ListenerNode(settings::Settings::instance().application().scene().listenerNodeName()));

    m_->sceneRootNode() = std::shared_ptr<SceneRootNode>(new SceneRootNode(settings::Settings::instance().application().scene().sceneRootNodeName()));
    m_->sceneRootNode()->attach(m_->listenerNode());
}



}
}
