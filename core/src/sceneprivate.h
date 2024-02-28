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
    std::shared_ptr<BackgroundDrawable> &backgroundDrawable();

private:
    std::string m_name;
    std::weak_ptr<GraphicsEngine> m_graphicsEngine;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<BackgroundDrawable> m_backgroundDrawable;

};

}
}

#endif // SCENEPRIVATE_H
