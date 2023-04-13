#include <core/drawablenode.h>

#include "drawablenodeprivate.h"

namespace simplex
{
namespace core
{

DrawableNode::DrawableNode(const std::string &name)
    : Node(new DrawableNodePrivate(name))
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

const std::unordered_set<std::shared_ptr<Drawable>> &DrawableNode::drawables() const
{
    return m().drawables();
}

void DrawableNode::addDrawable(std::shared_ptr<Drawable> drawable)
{
    m().drawables().insert(drawable);
}

void DrawableNode::removeDrawable(std::shared_ptr<Drawable> drawable)
{
    m().drawables().erase(drawable);
}

}
}
