#include <core/listenernode.h>

#include "listenernodeprivate.h"

namespace simplex
{
namespace core
{

ListenerNode::~ListenerNode() = default;

std::shared_ptr<ListenerNode> ListenerNode::asListenerNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<ListenerNode>(wp.lock());
}

std::shared_ptr<const ListenerNode> ListenerNode::asListenerNode() const
{
    return const_cast<ListenerNode*>(this)->asListenerNode();
}

ListenerNode::ListenerNode(const std::string& name)
    : Node(std::make_unique<ListenerNodePrivate>(*this, name))
{
}

}
}
