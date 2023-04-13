#ifndef CORE_NODECOLLECTOR_H
#define CORE_NODECOLLECTOR_H

#include <vector>

#include <core/nodevisitor.h>

namespace simplex
{
namespace core
{

template <typename NodeClass>
class NodeCollector : public NodeVisitor
{
public:
    NodeCollector() {}

    bool visit(std::shared_ptr<Node> node) override
    {
        if (auto collectedNode = std::dynamic_pointer_cast<NodeClass>(node); collectedNode)
            m_nodes.push_back(collectedNode);
        return true;
    }

    const std::vector<std::shared_ptr<NodeClass>> &nodes() const { return m_nodes; }
    void clear() { m_nodes.clear(); }

private:
    std::vector<std::shared_ptr<NodeClass>> m_nodes;
};

}
}

#endif // CORE_NODECOLLECTOR_H
