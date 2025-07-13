#include "sceneprivate.h"

namespace simplex
{
namespace core
{

ScenePrivate::ScenePrivate(const std::string &name)
    : m_name(name)
{
}

const std::string &ScenePrivate::name() const
{
    return m_name;
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

std::unordered_set<std::shared_ptr<Animation>> &ScenePrivate::animations()
{
    return m_animations;
}

}
}
