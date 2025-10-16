#ifndef CORE_VISUALDRAWABLENODEPRIVATE_H
#define CORE_VISUALDRAWABLENODEPRIVATE_H

#include <unordered_set>
#include <unordered_map>

#include <utils/pimpl.h>

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class DrawDataHandler;
class SceneData;

class VisualDrawableNodePrivate : public NodePrivate
{
    PUBLIC_IMPL(VisualDrawableNode)
public:
    VisualDrawableNodePrivate(VisualDrawableNode&, const std::string&);
    ~VisualDrawableNodePrivate() override;

    utils::BoundingBox doBoundingBox() override;
    void doAfterTransformChanged() override;
    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    void addDrawDataToSceneData(const std::shared_ptr<SceneData>&, const std::shared_ptr<const VisualDrawable>&);
    void removeDrawDataFromSceneData(const std::shared_ptr<const VisualDrawable>&);
    void changeDrawDataInSceneData();

    std::unordered_set<std::shared_ptr<VisualDrawable>> &visualDrawables();
    std::unordered_map<std::shared_ptr<const VisualDrawable>, std::shared_ptr<DrawDataHandler>>& handlers();

    const utils::BoundingBox &localBoundingBox();

    void dirtyLocalBoundingBox();

private:
    std::unordered_set<std::shared_ptr<VisualDrawable>> m_visualDrawables;
    std::unordered_map<std::shared_ptr<const VisualDrawable>, std::shared_ptr<DrawDataHandler>> m_handlers;
    utils::BoundingBox m_localBoundingBox;
    bool m_isLocalBoundingBoxDirty;
};

}
}

#endif // CORE_VISUALDRAWABLENODEPRIVATE_H
