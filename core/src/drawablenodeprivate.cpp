#include <core/drawablenode.h>

#include "drawablenodeprivate.h"

namespace simplex
{
namespace core
{

DrawableNodePrivate::DrawableNodePrivate(const std::string &name)
    : NodePrivate(name)
    , m_localBoundingBoxDirty(true)
{}

bool &DrawableNodePrivate::isLocalBoundingBoxDirty()
{
    return m_localBoundingBoxDirty;
}

utils::BoundingBox &DrawableNodePrivate::localBoundingBox()
{
    return m_localBoundingBox;
}

std::unordered_set<std::shared_ptr<IDrawable>> &DrawableNodePrivate::drawables()
{
    return m_drawables;
}

std::shared_ptr<ColoredDrawable> &DrawableNodePrivate::localBoundingBoxDrawable()
{
    return m_localBoundingBoxDrawable;
}

void DrawableNodePrivate::dirtyLocalBoundingBox(std::shared_ptr<DrawableNode> node)
{
    node->m().m_localBoundingBoxDirty = true;
    NodePrivate::dirtyBoundingBox(node);
}


}
}
