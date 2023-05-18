#ifndef CORE_INODEVISITOR_H
#define CORE_INODEVISITOR_H

#include <memory>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class INodeVisitor
{
public:
    virtual ~INodeVisitor() = default;
    virtual bool visit(std::shared_ptr<Node> node) = 0;

};

} // namespace
} // namespace

#endif // CORE_INODEVISITOR_H
