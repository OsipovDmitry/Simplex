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
    ScenePrivate(std::weak_ptr<GraphicsEngine> graphicsEngine, const std::string &name, std::shared_ptr<SceneRootNode> node)
        : m_name(name)
        , m_graphicsEngine(graphicsEngine)
        , m_sceneRootNode(node)
    {
    }

    const std::string &name() const { return m_name; }


    std::weak_ptr<GraphicsEngine> graphicsEngine() { return m_graphicsEngine; }
    std::shared_ptr<SceneRootNode> sceneRootNode() { return m_sceneRootNode; }

private:
    std::string m_name;
    std::weak_ptr<GraphicsEngine> m_graphicsEngine;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;

};

}
}

#endif // SCENEPRIVATE_H
