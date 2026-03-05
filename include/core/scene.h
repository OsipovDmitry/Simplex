#ifndef CORE_SCENE_H
#define CORE_SCENE_H

#include <filesystem>
#include <memory>
#include <unordered_set>

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
class CORE_SHARED_EXPORT Scene : public std::enable_shared_from_this<Scene>, public INamedObject
{
    NONCOPYBLE(Scene)
    PRIVATE_IMPL(Scene)
public:
    ~Scene() override;

    const std::string& name() const override;

    std::shared_ptr<SceneRootNode> sceneRootNode();
    std::shared_ptr<const SceneRootNode> sceneRootNode() const;

    std::shared_ptr<ListenerNode> listenerNode();
    std::shared_ptr<const ListenerNode> listenerNode() const;

    std::shared_ptr<Background> background();
    std::shared_ptr<const Background> background() const;

    static std::shared_ptr<Scene> createEmpty(const std::string&);

private:
    Scene(const std::string&);

    std::unique_ptr<ScenePrivate> m_;
};

} // namespace core
} // namespace simplex

#endif // CORE_SCENE_H
