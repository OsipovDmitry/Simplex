#ifndef CORE_SCENE_H
#define CORE_SCENE_H

#include <memory>

#include <utils/noncopyble.h>
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
public:
    ~Scene() override;

    const std::string &name() const override;

    std::shared_ptr<const SceneRootNode> sceneRootNode() const;
    std::shared_ptr<SceneRootNode> sceneRootNode();

private:
    Scene(const std::string&);

    std::unique_ptr<ScenePrivate> m_;

    friend class GraphicsEngine;
};

}
}

#endif // CORE_SCENE_H
