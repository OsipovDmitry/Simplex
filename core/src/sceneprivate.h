#ifndef SCENEPRIVATE_H
#define SCENEPRIVATE_H

#include <string>
#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ScenePrivate
{
public:
    ScenePrivate(const std::weak_ptr<GraphicsEngine> &graphicsEngine, const std::string &name, const std::shared_ptr<SceneRootNode> &node);

    const std::string &name() const;

    std::weak_ptr<GraphicsEngine> &graphicsEngine();
    std::shared_ptr<SceneRootNode> &sceneRootNode();

    std::shared_ptr<Drawable> &backgroundScreenQuadDrawable();

    static std::weak_ptr<const graphics::ITexture> &defaultBacgroundTexture();

private:
    static std::weak_ptr<const graphics::ITexture> s_defaultBacgroundTexture;

    std::string m_name;
    std::weak_ptr<GraphicsEngine> m_graphicsEngine;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<Drawable> m_backgroundScreenQuadDrawable;

};

}
}

#endif // SCENEPRIVATE_H
