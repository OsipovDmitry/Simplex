#include <core/scene.h>

#include "sceneprivate.h"
#include "scenedata.h"
#include "backgroundprivate.h"

namespace simplex
{
namespace core
{

ScenePrivate::ScenePrivate(Scene& scene, const std::string &name)
    : d_(scene)
    , m_name(name)
    , m_sceneData(std::make_shared<SceneData>())
{
}

const std::string &ScenePrivate::name() const
{
    return m_name;
}

std::shared_ptr<Background>& ScenePrivate::background()
{
    return m_background;
}

std::shared_ptr<SceneRootNode> &ScenePrivate::sceneRootNode()
{
    return m_sceneRootNode;
}

std::shared_ptr<ListenerNode> &ScenePrivate::listenerNode()
{
    return m_listenerNode;
}

std::shared_ptr<SceneData>& ScenePrivate::sceneData()
{
    return m_sceneData;
}

}
}
