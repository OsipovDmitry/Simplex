#include "drawablenodeprivate.h"

namespace simplex
{
namespace core
{

DrawableNodePrivate::DrawableNodePrivate(const std::string &name)
    : NodePrivate(name)
    , m_localBoundingBoxDirty(true)
{}

DrawableNodePrivate::~DrawableNodePrivate() = default;

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

}
}
