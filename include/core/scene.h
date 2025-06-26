#ifndef CORE_SCENE_H
#define CORE_SCENE_H

#include <memory>
#include <unordered_set>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>
#include <utils/sortedobject.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class ScenePrivate;
class CORE_SHARED_EXPORT Scene : public INamedObject, public utils::SortedObject
{
    NONCOPYBLE(Scene)
    PRIVATE_IMPL(Scene)
public:
    ~Scene() override;

    const std::string &name() const override;

    std::shared_ptr<ApplicationBase> application();
    std::shared_ptr<const ApplicationBase> application() const;

    std::shared_ptr<SceneRootNode> sceneRootNode();
    std::shared_ptr<const SceneRootNode> sceneRootNode() const;

    std::shared_ptr<ListenerNode> listenerNode();
    std::shared_ptr<const ListenerNode> listenerNode() const;

    Background &background();
    const Background &background() const;

    const std::unordered_set<std::shared_ptr<Animation>> &animations();
    void removeAnimation(const std::shared_ptr<Animation>&);
    void addAnimation(const std::shared_ptr<Animation>&);

private:
    Scene(const std::weak_ptr<ApplicationBase>&, const std::string&);

    std::unique_ptr<ScenePrivate> m_;

    friend class ApplicationBase;
};

}
}

#endif // CORE_SCENE_H
