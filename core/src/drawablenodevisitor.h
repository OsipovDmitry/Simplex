#ifndef CORE_DRAWABLENODEVISITOR_H
#define CORE_DRAWABLENODEVISITOR_H

#include <utils/pimpl.h>

#include <core/frustumcullingnodevisitor.h>

namespace simplex
{
namespace core
{

class DrawableNodeVisitorPrivate;
class DrawableNodeVisitor : public FrustumCullingNodeVisitor
{
    PIMPL(DrawableNodeVisitor)
public:
    DrawableNodeVisitor(const utils::Frustum&);

    bool visit(std::shared_ptr<Node>) override;

    const std::deque<std::shared_ptr<DrawableNode>> &drawableNodes() const;

};

}
}

#endif // CORE_DRAWABLENODEVISITOR_H
