#ifndef CORE_DRAWABLENODEPRIVATE_H
#define CORE_DRAWABLENODEPRIVATE_H

#include <unordered_set>
#include <memory>

#include <core/forwarddecl.h>
#include <core/idrawable.h>
#include <core/drawablenode.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class DrawableNodePrivate : public NodePrivate
{
public:
    DrawableNodePrivate(const std::string &name)
        : NodePrivate(name)
        , m_localBoundingBoxDirty(true)
    {}

    bool &isLocalBoundingBoxDirty()
    {
        return m_localBoundingBoxDirty;
    }

    utils::BoundingBox &localBoundingBox()
    {
        return m_localBoundingBox;
    }

    const std::unordered_set<std::shared_ptr<IDrawable>> &drawables() const { return m_drawables; }
    std::unordered_set<std::shared_ptr<IDrawable>> &drawables() { return m_drawables; }

    static void dirtyLocalBoundingBox(std::shared_ptr<DrawableNode> node)
    {
        node->m().m_localBoundingBoxDirty = true;
        NodePrivate::dirtyBoundingBox(node);
    }

private:
    std::unordered_set<std::shared_ptr<IDrawable>> m_drawables;
    utils::BoundingBox m_localBoundingBox;
    bool m_localBoundingBoxDirty;
};

}
}

#endif // CORE_DRAWABLENODEPRIVATE_H
