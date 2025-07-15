#ifndef CORE_SCENE_H
#define CORE_SCENE_H

#include <memory>
#include <unordered_set>
#include <filesystem>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class ScenePrivate;
class CORE_SHARED_EXPORT Scene : public INamedObject
{
    NONCOPYBLE(Scene)
    PRIVATE_IMPL(Scene)
public:
    ~Scene() override;

    const std::string &name() const override;

    std::shared_ptr<SceneRootNode> sceneRootNode();
    std::shared_ptr<const SceneRootNode> sceneRootNode() const;

    std::shared_ptr<ListenerNode> listenerNode();
    std::shared_ptr<const ListenerNode> listenerNode() const;

    Background &background();
    const Background &background() const;

    static std::shared_ptr<Scene> createEmpty(const std::string&);
    static std::shared_ptr<Scene> createFromGLTF(const std::filesystem::path&, const std::shared_ptr<graphics::IFrameBuffer>&);

private:
    Scene(const std::string&);

    std::unique_ptr<ScenePrivate> m_;
};

}
}

#endif // CORE_SCENE_H
