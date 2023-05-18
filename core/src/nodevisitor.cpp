#include <core/nodevisitor.h>

namespace simplex
{
namespace core
{

NodeVisitor::NodeVisitor()
{
}

NodeVisitor::~NodeVisitor()
{
}

bool NodeVisitor::visit(std::shared_ptr<Node>)
{
    return true;
}

} // namespace
} // namespace
