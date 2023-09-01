#ifndef CORE_FRUSTUMCULLINGNODEVISITORPRIVATE_H
#define CORE_FRUSTUMCULLINGNODEVISITORPRIVATE_H

#include <deque>

#include <core/forwarddecl.h>

#include "conditionalnodevisitorprivate.h"

namespace simplex
{
namespace core
{

class FrustumCullingNodeVisitorPrivate : public ConditionalNodeVisitorPrivate {
public:
    FrustumCullingNodeVisitorPrivate(std::shared_ptr<ConditionalNodeVisitor::AbstractCondition> condition)
        : ConditionalNodeVisitorPrivate(condition)
    {}

    std::deque<std::shared_ptr<Node>> &nodes() { return m_nodes; }

protected:
    std::deque<std::shared_ptr<Node>> m_nodes;

};

}
}

#endif // CORE_FRUSTUMCULLINGNODEVISITORPRIVATE_H
