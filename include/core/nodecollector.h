#ifndef CORE_COLLECTORVISITOR_H
#define CORE_COLLECTORVISITOR_H

#include <deque>
#include <optional>

#include <core/nodevisitor.h>
#include <core/node.h>

namespace simplex
{
namespace core
{

template <typename NodeClass>
class CORE_SHARED_EXPORT NodeCollector : public NodeVisitor
{
public:
    using Collection = std::deque<std::shared_ptr<NodeClass>>;

    NodeCollector() : NodeVisitor() {}
    NodeCollector(const std::string &nodeName) : NodeVisitor(), m_nodeName(nodeName) {}

    bool visit(Node &node) override {
        if (auto castedNode = std::dynamic_pointer_cast<NodeClass>(node.asNode()); castedNode)
            if (!m_nodeName.has_value() || (m_nodeName.value() == node.name()))
                m_nodes.push_back(castedNode);
        return true;
    }

    const Collection &nodes() const { return m_nodes; }
    Collection &nodes() { return m_nodes; }

protected:
    std::optional<std::string> m_nodeName;
    Collection m_nodes;

};

}
}

#endif // CORE_COLLECTORVISITOR_H
