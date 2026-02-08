#ifndef CORE_SCENEPRIVATE_H
#define CORE_SCENEPRIVATE_H

#include <string>
#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class SceneData;

class ScenePrivate
{
public:
    ScenePrivate(Scene&, const std::string&);

    const std::string &name() const;

    std::shared_ptr<SceneData>& sceneData();
    std::shared_ptr<Background>& background();
    std::shared_ptr<SceneRootNode> &sceneRootNode();
    std::shared_ptr<ListenerNode> &listenerNode();

private:
    Scene& d_;

    std::string m_name;
    std::shared_ptr<SceneData> m_sceneData;
    std::shared_ptr<Background> m_background;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<ListenerNode> m_listenerNode;
};

}
}

#endif // CORE_SCENEPRIVATE_H
