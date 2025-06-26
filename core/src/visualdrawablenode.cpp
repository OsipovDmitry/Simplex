#include <core/visualdrawablenode.h>

#include "visualdrawablenodeprivate.h"

namespace simplex
{
namespace core
{

VisualDrawableNode::VisualDrawableNode(const std::string &name)
    : Node(std::make_unique<VisualDrawableNodePrivate>(*this, name))
{
}

VisualDrawableNode::~VisualDrawableNode()
{
}

std::shared_ptr<VisualDrawableNode> VisualDrawableNode::asVisualDrawableNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<VisualDrawableNode>(wp.lock());
}

std::shared_ptr<const VisualDrawableNode> VisualDrawableNode::asVisualDrawableNode() const
{
    return const_cast<VisualDrawableNode*>(this)->asVisualDrawableNode();
}

const std::unordered_set<std::shared_ptr<VisualDrawable>> &VisualDrawableNode::visualDrawables() const
{
    return m().visualDrawables();
}

void VisualDrawableNode::addVisualDrawable(std::shared_ptr<VisualDrawable> drawable)
{
    auto &mPrivate = m();
    mPrivate.visualDrawables().insert(drawable);
    mPrivate.dirtyLocalBoundingBox();
}

void VisualDrawableNode::removeVisualDrawable(std::shared_ptr<VisualDrawable> drawable)
{
    auto &mPrivate = m();
    mPrivate.visualDrawables().erase(drawable);
    mPrivate.dirtyLocalBoundingBox();
}

std::shared_ptr<Skeleton> VisualDrawableNode::skeleton()
{
    return m().skeleton();
}

std::shared_ptr<const Skeleton> VisualDrawableNode::skeleton() const
{
    return const_cast<VisualDrawableNode*>(this)->skeleton();
}

void VisualDrawableNode::setSkeleton(const std::shared_ptr<Skeleton> &value)
{
    m().skeleton() = value;
}

}
}
