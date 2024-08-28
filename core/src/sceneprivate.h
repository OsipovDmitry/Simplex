#ifndef SCENEPRIVATE_H
#define SCENEPRIVATE_H

#include <string>
#include <memory>

#include <core/forwarddecl.h>
#include <core/background.h>

namespace simplex
{
namespace core
{

class BackgroundDrawable;

class ScenePrivate
{
public:
    ScenePrivate(const std::weak_ptr<ApplicationBase>&, const std::string&);

    const std::string &name() const;

    std::weak_ptr<ApplicationBase> &application();
    std::shared_ptr<SceneRootNode> &sceneRootNode();
    std::shared_ptr<ListenerNode> &listenerNode();
    Background &background();

private:
    std::string m_name;
    std::weak_ptr<ApplicationBase> m_application;
    std::shared_ptr<SceneRootNode> m_sceneRootNode;
    std::shared_ptr<ListenerNode> m_listenerNode;
    Background m_background;

};

}
}

#endif // SCENEPRIVATE_H
