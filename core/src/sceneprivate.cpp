#include "sceneprivate.h"

namespace simplex
{
namespace core
{

ScenePrivate::ScenePrivate(const std::weak_ptr<ApplicationBase> &application, const std::string &name)
    : m_name(name)
    , m_application(application)
{
}

const std::string &ScenePrivate::name() const
{
    return m_name;
}

std::weak_ptr<ApplicationBase> &ScenePrivate::application()
{
    return m_application;
}

std::shared_ptr<SceneRootNode> &ScenePrivate::sceneRootNode()
{
    return m_sceneRootNode;
}

std::shared_ptr<ListenerNode> &ScenePrivate::listenerNode()
{
    return m_listenerNode;
}

Background &ScenePrivate::background()
{
    return m_background;
}

}
}
