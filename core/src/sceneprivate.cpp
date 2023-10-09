#include "sceneprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<const graphics::ITexture> ScenePrivate::s_defaultBacgroundTexture;

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

std::shared_ptr<Drawable> &ScenePrivate::backgroundScreenQuadDrawable()
{
    return m_backgroundScreenQuadDrawable;
}

std::weak_ptr<const graphics::ITexture> &ScenePrivate::defaultBacgroundTexture()
{
    return s_defaultBacgroundTexture;
}


}
}
