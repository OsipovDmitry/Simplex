#ifndef SCENEPRIVATE_H
#define SCENEPRIVATE_H

#include <string>
#include <memory>
#include <unordered_set>

#include <utils/idgenerator.h>

#include <core/forwarddecl.h>
#include <core/background.h>

namespace simplex
{
namespace core
{

class ScenePrivate
{
public:
    ScenePrivate(const std::string&);

    const std::string &name() const;

    std::shared_ptr<SceneRootNode> &sceneRootNode();
    std::shared_ptr<ListenerNode> &listenerNode();
    Background &background();

    utils::IDGenerator &meshIDGenerator();

private:
    std::string m_name;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<ListenerNode> m_listenerNode;
    Background m_background;

    utils::IDGenerator m_meshIDGenerator;

};

}
}

#endif // SCENEPRIVATE_H
