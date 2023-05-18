#ifndef CORE_CONDITIONALNODEVISITORPRIVATE_H
#define CORE_CONDITIONALNODEVISITORPRIVATE_H

#include <core/conditionalnodevisitor.h>

namespace simplex
{
namespace core
{

class ConditionalNodeVisitorPrivate {
public:
    ConditionalNodeVisitorPrivate(std::shared_ptr<ConditionalNodeVisitor::AbstractCondition> condition)
        : m_condition(condition)
    {}
    virtual ~ConditionalNodeVisitorPrivate() = default;

    std::shared_ptr<ConditionalNodeVisitor::AbstractCondition> &condition() { return m_condition; }

protected:
    std::shared_ptr<ConditionalNodeVisitor::AbstractCondition> m_condition;

};

} // namespace
} // namespace


#endif // CORE_CONDITIONALNODEVISITORPRIVATE_H
