#ifndef CORE_DRAWABLENODEPRIVATE_H
#define CORE_DRAWABLENODEPRIVATE_H

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

class DrawableNodePrivate : public NodePrivate
{
    PUBLIC_IMPL(DrawableNode)
public:
    DrawableNodePrivate(DrawableNode&, const std::string&);
    ~DrawableNodePrivate() override;

    void doAfterTransformChanged() override;
    void doAttachToScene(const std::shared_ptr<Scene>&) override;
    void doDetachFromScene(const std::shared_ptr<Scene>&) override;

    void addDrawDataToSceneData(const std::shared_ptr<SceneData>&, const std::shared_ptr<const Drawable>&);
    void removeDrawDataFromSceneData(const std::shared_ptr<const Drawable>&);
    void changeDrawDataInSceneData();

    std::unordered_set<std::shared_ptr<Drawable>> &drawables();
    std::unordered_map<std::shared_ptr<const Drawable>, std::shared_ptr<DrawDataHandler>>& handlers();

private:
    std::unordered_set<std::shared_ptr<Drawable>> m_drawables;
    std::unordered_map<std::shared_ptr<const Drawable>, std::shared_ptr<DrawDataHandler>> m_handlers;
};

}
}

#endif // CORE_DRAWABLENODEPRIVATE_H
