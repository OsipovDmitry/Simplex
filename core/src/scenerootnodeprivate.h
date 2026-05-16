#ifndef CORE_SCENEROOTNODEPRIVATE_H
#define CORE_SCENEROOTNODEPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class SceneRootNodePrivate : public NodePrivate
{
public:
    SceneRootNodePrivate(SceneRootNode& sceneRootNode, const std::string& name);

    ~SceneRootNodePrivate() override;

    void onAttachToParent(const std::shared_ptr<Node>&) override;
    void onDetachFromParent(const std::shared_ptr<Node>&) override;

    std::weak_ptr<Scene>& scene();

private:
    std::weak_ptr<Scene> m_scene;
};

} // namespace core
} // namespace simplex

#endif // CORE_SCENEROOTNODEPRIVATE_H
