#include <core/nodevisitor.h>

namespace simplex
{
namespace core
{

NodeVisitor::NodeVisitor()
{
}

NodeVisitor::~NodeVisitor() = default;

bool NodeVisitor::visit(Node &)
{
    return true;
}

} // namespace
} // namespace
