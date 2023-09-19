#ifndef CORE_COLLECTORVISITOR_H
#define CORE_COLLECTORVISITOR_H

#include <deque>

#include <core/nodevisitor.h>

namespace simplex
{
namespace core
{

template <typename NodeClass>
class CORE_SHARED_EXPORT NodeCollector : public NodeVisitor
{
public:
    NodeCollector() : NodeVisitor() {}

    bool visit(const std::shared_ptr<Node> &node) override {
        if (auto castedNode = std::dynamic_pointer_cast<NodeClass>(node); castedNode)
            m_nodes.push_back(castedNode);
        return true;
    }

    const std::deque<std::shared_ptr<NodeClass>> &nodes() const { return m_nodes; }
    std::deque<std::shared_ptr<NodeClass>> &nodes() { return m_nodes; }

protected:
    std::deque<std::shared_ptr<NodeClass>> m_nodes;

};

}
}

#endif // CORE_COLLECTORVISITOR_H
