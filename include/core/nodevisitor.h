#ifndef CORE_NODEVISITOR_H
#define CORE_NODEVISITOR_H

#include <memory>

#include <utils/noncopyble.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class NodeVisitor
{
    NONCOPYBLE(NodeVisitor)
public:
    NodeVisitor();
    virtual ~NodeVisitor();

    virtual bool visit(std::shared_ptr<Node>);

};

} // namespace
} // namespace

#endif // CORE_NODEVISITOR_H
