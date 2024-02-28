#include "sceneprivate.h"

namespace simplex
{
namespace core
{

ScenePrivate::ScenePrivate(const std::weak_ptr<GraphicsEngine> &graphicsEngine, const std::string &name, const std::shared_ptr<SceneRootNode> &node)
    : m_name(name)
    , m_graphicsEngine(graphicsEngine)
    , m_sceneRootNode(node)
{
}

const std::string &ScenePrivate::name() const
{
    return m_name;
}

std::weak_ptr<GraphicsEngine> &ScenePrivate::graphicsEngine()
{
    return m_graphicsEngine;
}

std::shared_ptr<SceneRootNode> &ScenePrivate::sceneRootNode()
{
    return m_sceneRootNode;
}

std::shared_ptr<BackgroundDrawable> &ScenePrivate::backgroundDrawable()
{
    return m_backgroundDrawable;
}

}
}
