#ifndef CORE_SCENE_H
#define CORE_SCENE_H

#include <memory>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/inamedobject.h>
#include <core/sortedobject.h>

namespace simplex
{
namespace core
{

class ScenePrivate;
class CORE_SHARED_EXPORT Scene : public INamedObject, public SortedObject, public std::enable_shared_from_this<Scene>
{
    NONCOPYBLE(Scene)
public:
    Scene(const std::string&);
    ~Scene() override;

    const std::string &name() const override;

    std::shared_ptr<SceneRootNode> sceneRootNode();

private:
    std::unique_ptr<ScenePrivate> m_;
};

}
}

#endif // CORE_SCENE_H
