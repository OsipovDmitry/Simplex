#ifndef CORE_SCENE_H
#define CORE_SCENE_H

#include <memory>

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
    PIMPL(Scene)
public:
    ~Scene() override;

    const std::string &name() const override;

    std::weak_ptr<const GraphicsEngine> graphicsEngine() const;
    std::weak_ptr<GraphicsEngine> graphicsEngine();

    std::shared_ptr<const SceneRootNode> sceneRootNode() const;
    std::shared_ptr<SceneRootNode> sceneRootNode();

    std::shared_ptr<const BackgroundDrawable> backgroundDrawable() const;
    std::shared_ptr<BackgroundDrawable> backgroundDrawable();

private:
    Scene(const std::weak_ptr<GraphicsEngine>&, const std::string&);

    std::unique_ptr<ScenePrivate> m_;

    friend class GraphicsEngine;
};

}
}

#endif // CORE_SCENE_H
