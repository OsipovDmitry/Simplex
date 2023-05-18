#ifndef CORE_DRAWABLENODEPRIVATE_H
#define CORE_DRAWABLENODEPRIVATE_H

#include <unordered_set>
#include <memory>

#include <core/forwarddecl.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class DrawableNodePrivate : public NodePrivate
{
public:
    DrawableNodePrivate(const std::string &name);

    bool &isLocalBoundingBoxDirty();
    utils::BoundingBox &localBoundingBox();

    std::unordered_set<std::shared_ptr<IDrawable>> &drawables();

    std::shared_ptr<ColoredDrawable> &localBoundingBoxDrawable();

    static void dirtyLocalBoundingBox(std::shared_ptr<DrawableNode> node);

private:
    std::unordered_set<std::shared_ptr<IDrawable>> m_drawables;
    utils::BoundingBox m_localBoundingBox;
    bool m_localBoundingBoxDirty;

    std::shared_ptr<ColoredDrawable> m_localBoundingBoxDrawable;
};

}
}

#endif // CORE_DRAWABLENODEPRIVATE_H
