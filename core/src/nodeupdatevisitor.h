#ifndef NODEUPDATEVISITOR_H
#define NODEUPDATEVISITOR_H

#include <core/nodevisitor.h>

namespace simplex
{
namespace core
{

class NodeUpdateVisitor : public NodeVisitor
{
public:
    NodeUpdateVisitor(uint64_t time, uint32_t dt);

    bool visit(std::shared_ptr<Node> node) override;

private:
    uint64_t m_time;
    uint32_t m_dt;

};

} // namespace
} // namespace

#endif // NODEUPDATEVISITOR_H
