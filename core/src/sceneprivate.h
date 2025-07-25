#ifndef CORE_SCENEPRIVATE_H
#define CORE_SCENEPRIVATE_H

#include <string>
#include <memory>

#include <core/graphicsrendererbase.h>
#include <core/background.h>

namespace simplex
{
namespace core
{

struct MaterialMapHandler;
struct MaterialHandler;
struct MeshHandler;
class SceneData;

class ScenePrivate
{
public:
    ScenePrivate(Scene&, const std::string&);

    const std::string &name() const;

    std::shared_ptr<SceneRootNode> &sceneRootNode();
    std::shared_ptr<ListenerNode> &listenerNode();
    Background &background();
    std::shared_ptr<SceneData>& sceneData();

private:
    Scene& d_;

    std::string m_name;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<ListenerNode> m_listenerNode;
    Background m_background;
    std::shared_ptr<SceneData> m_sceneData;
};

}
}

#endif // CORE_SCENEPRIVATE_H
