#include <core/conditionalnodevisitor.h>

#include "conditionalnodevisitorprivate.h"

namespace simplex
{
namespace core
{

ConditionalNodeVisitor::ConditionalNodeVisitor(std::shared_ptr<AbstractCondition> condition)
    : m_(std::make_unique<ConditionalNodeVisitorPrivate>(condition))
{
}

ConditionalNodeVisitor::~ConditionalNodeVisitor()
{
}

std::shared_ptr<const ConditionalNodeVisitor::AbstractCondition> ConditionalNodeVisitor::condition() const
{
    return const_cast<ConditionalNodeVisitor*>(this)->condition();
}

std::shared_ptr<ConditionalNodeVisitor::AbstractCondition> ConditionalNodeVisitor::condition()
{
    return m_->condition();
}

void ConditionalNodeVisitor::setCondition(std::shared_ptr<AbstractCondition> condition)
{
    m_->condition() = condition;
}

bool ConditionalNodeVisitor::visit(std::shared_ptr<Node> node)
{
    auto &condition = m_->condition();
    return condition ? (*condition)(node) : true;
}

ConditionalNodeVisitor::ConditionalNodeVisitor(ConditionalNodeVisitorPrivate *mPrivate)
    : m_(std::unique_ptr<ConditionalNodeVisitorPrivate>(mPrivate))
{
}

}
}
