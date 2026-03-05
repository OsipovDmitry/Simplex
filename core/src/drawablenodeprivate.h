#ifndef CORE_DRAWABLENODEPRIVATE_H
#define CORE_DRAWABLENODEPRIVATE_H

#include <unordered_map>
#include <unordered_set>

#include <utils/forwarddecl.h>
#include <utils/idgenerator.h>
#include <utils/pimpl.h>

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class DrawDataHandler;
class SceneData;

class DrawableNodePrivate : public NodePrivate
{
    PUBLIC_IMPL(DrawableNode)
public:
    DrawableNodePrivate(DrawableNode&, const std::string&, const std::weak_ptr<SkeletalAnimatedNode>&);
    ~DrawableNodePrivate() override;

    void doAfterTransformChanged() override;
    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    void addDrawDataToSceneData(const std::shared_ptr<SceneData>&, const std::shared_ptr<const Drawable>&);
    void removeDrawDataFromSceneData(const std::shared_ptr<const Drawable>&);
    void changeDrawDataInSceneData();

    std::unordered_set<std::shared_ptr<Drawable>>& drawables();
    std::weak_ptr<SkeletalAnimatedNode>& skeletalAnimatedNode();
    std::unordered_map<std::shared_ptr<const Drawable>, std::shared_ptr<DrawDataHandler>>& handlers();

private:
    std::unordered_set<std::shared_ptr<Drawable>> m_drawables;
    std::weak_ptr<SkeletalAnimatedNode> m_skeletalAnimatedNode;
    std::unordered_map<std::shared_ptr<const Drawable>, std::shared_ptr<DrawDataHandler>> m_handlers;

    utils::IDsGenerator::value_type skeletalAnimatedDataID(const std::shared_ptr<SceneData>&) const;
};

} // namespace core
} // namespace simplex

#endif // CORE_DRAWABLENODEPRIVATE_H
