#ifndef CORE_SCENEROOTNODE_H
#define CORE_SCENEROOTNODE_H

#include <utils/pimpl.h>

#include <core/node.h>

namespace simplex
{
namespace core
{

class SceneRootNodePrivate;
class CORE_SHARED_EXPORT SceneRootNode : public Node
{
    PRIVATE_IMPL(SceneRootNode)

public:
    ~SceneRootNode() override;

    std::shared_ptr<SceneRootNode> asSceneRootNode() override;
    std::shared_ptr<const SceneRootNode> asSceneRootNode() const override;

    std::shared_ptr<Scene> scene();
    std::shared_ptr<const Scene> scene() const;

protected:
    SceneRootNode(const std::string&);

    friend class Scene;

};

}
}

#endif // CORE_SCENEROOTNODE_H
