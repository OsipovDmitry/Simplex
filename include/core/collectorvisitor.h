#ifndef CORE_COLLECTORVISITOR_H
#define CORE_COLLECTORVISITOR_H

#include <vector>

#include <core/conditionalnodevisitor.h>

namespace simplex
{
namespace core
{

template <typename NodeClass>
class CollectorVisitor : public ConditionalNodeVisitor
{
public:

    class CollectorVisitorCondition : public ConditionalNodeVisitor::AbstractCondition
    {
    public:
        CollectorVisitorCondition() : ConditionalNodeVisitor::AbstractCondition() {}

        bool operator ()(std::shared_ptr<Node> node) override
        {
            if (auto castedNode = std::dynamic_pointer_cast<NodeClass>(node); castedNode)
                m_nodes.push_back(castedNode);

            return true;
        }

        std::vector<std::shared_ptr<NodeClass>> &nodes() { return m_nodes; }

    private:
        std::vector<std::shared_ptr<NodeClass>> m_nodes;

    };

    CollectorVisitor() : ConditionalNodeVisitor(std::make_shared<CollectorVisitorCondition>()) {}

    void clear() { std::dynamic_pointer_cast<CollectorVisitorCondition>(condition())->nodes().clear(); }

    std::vector<std::shared_ptr<NodeClass>> &nodes() { return std::dynamic_pointer_cast<CollectorVisitorCondition>(condition())->nodes(); }
    const std::vector<std::shared_ptr<NodeClass>> &nodes() const
    {
        return const_cast<CollectorVisitor<NodeClass>>(this)->nodes();
    }

};

}
}

#endif // CORE_COLLECTORVISITOR_H
