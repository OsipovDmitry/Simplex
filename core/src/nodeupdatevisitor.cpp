#include <core/node.h>

#include "nodeprivate.h"
#include "nodeupdatevisitor.h"


namespace simplex
{
namespace core
{

NodeUpdateVisitor::NodeUpdateVisitor(uint64_t time, uint32_t dt)
        : m_time(time)
        , m_dt(dt)
{}

bool NodeUpdateVisitor::visit(std::shared_ptr<Node> node)
{
    node->m().doUpdate(m_time, m_dt);
    return true;
}

}
}
