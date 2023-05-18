#ifndef CORE_CONDITIONALNODEVISITOR_H
#define CORE_CONDITIONALNODEVISITOR_H

#include <core/coreglobal.h>
#include <core/nodevisitor.h>

namespace simplex
{
namespace core
{

class ConditionalNodeVisitorPrivate;
class CORE_SHARED_EXPORT ConditionalNodeVisitor : public NodeVisitor
{
public:
    class AbstractCondition
    {
    public:
        virtual ~AbstractCondition() = default;
        virtual bool operator ()(std::shared_ptr<Node>) = 0;
    };

    ConditionalNodeVisitor(std::shared_ptr<AbstractCondition>);
    ~ConditionalNodeVisitor() override;

    std::shared_ptr<const AbstractCondition> condition() const;
    std::shared_ptr<AbstractCondition> condition();
    void setCondition(std::shared_ptr<AbstractCondition>);

    bool visit(std::shared_ptr<Node>) override;

protected:
    ConditionalNodeVisitor(ConditionalNodeVisitorPrivate*);

    std::unique_ptr<ConditionalNodeVisitorPrivate> m_;

};

} // namespace
} // namespace

#endif // CORE_CONDITIONALNODEVISITOR_H
