#ifndef CORE_DRAWABLENODE_H
#define CORE_DRAWABLENODE_H

#include <unordered_set>

#include <utils/pimpl.h>

#include <core/node.h>

namespace simplex
{
namespace core
{

class DrawableNodePrivate;
class CORE_SHARED_EXPORT DrawableNode : public Node
{
    PRIVATE_IMPL(DrawableNode)
public:
    DrawableNode(const std::string&);
    ~DrawableNode() override;

    std::shared_ptr<DrawableNode> asDrawableNode() override;
    std::shared_ptr<const DrawableNode> asDrawableNode() const override;

    const std::unordered_set<std::shared_ptr<Drawable>> &drawables() const;
    void addDrawable(const std::shared_ptr<Drawable>&);
    void removeDrawable(const std::shared_ptr<Drawable>&);
};

}
}

#endif // CORE_DRAWABLENODE_H
