#ifndef CORE_DRAWABLENODEVISITORPRIVATE_H
#define CORE_DRAWABLENODEVISITORPRIVATE_H

#include "frustumcullingnodevisitorprivate.h"

namespace simplex
{
namespace core
{

class DrawableNodeVisitorPrivate : public FrustumCullingNodeVisitorPrivate
{
public:
    DrawableNodeVisitorPrivate(std::shared_ptr<ConditionalNodeVisitor::AbstractCondition> condition)
        : FrustumCullingNodeVisitorPrivate(condition)
    {}

    std::deque<std::shared_ptr<DrawableNode>> &drawableNodes() { return m_drawableNodes; }

protected:
    std::deque<std::shared_ptr<DrawableNode>> m_drawableNodes;

};

}
}


#endif // CORE_DRAWABLENODEVISITORPRIVATE_H
