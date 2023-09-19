#include <core/idrawable.h>
#include <core/drawablenode.h>

#include "drawablenodeprivate.h"
#include "nodevisitorhelpers.h"

namespace simplex
{
namespace core
{

DrawableNode::DrawableNode(const std::string &name)
    : Node(std::make_unique<DrawableNodePrivate>(name))
{
}

DrawableNode::~DrawableNode()
{
}

std::shared_ptr<DrawableNode> DrawableNode::asDrawableNode()
{
    return std::dynamic_pointer_cast<DrawableNode>(shared_from_this());
}

std::shared_ptr<const DrawableNode> DrawableNode::asDrawableNode() const
{
    return std::dynamic_pointer_cast<const DrawableNode>(shared_from_this());
}

utils::BoundingBox DrawableNode::doBoundingBox() const
{
    return localBoundingBox();
}

const std::unordered_set<std::shared_ptr<IDrawable>> &DrawableNode::drawables() const
{
    return m().drawables();
}

void DrawableNode::addDrawable(std::shared_ptr<IDrawable> drawable)
{
    m().drawables().insert(drawable);
    recalculateLocalBoundingBox();
}

void DrawableNode::removeDrawable(std::shared_ptr<IDrawable> drawable)
{
    m().drawables().erase(drawable);
    recalculateLocalBoundingBox();
}

const utils::BoundingBox &DrawableNode::localBoundingBox() const
{
    auto &mPrivate = m();
    if (mPrivate.isLocalBoundingBoxDirty())
    {
        auto &bb = mPrivate.localBoundingBox();
        bb = utils::BoundingBox();
        for (auto &drawable : mPrivate.drawables())
            bb += drawable->calculateBoundingBox();
        mPrivate.isLocalBoundingBoxDirty() = false;
    }
    return mPrivate.localBoundingBox();
}

void DrawableNode::recalculateLocalBoundingBox()
{
    m().isLocalBoundingBoxDirty() = true;

    DirtyBoundingBoxNodeVisitor dirtyBoundingBoxNodeVisitor;
    acceptUp(dirtyBoundingBoxNodeVisitor);
}

}
}
