#ifndef CORE_LISTENERNODE_H
#define CORE_LISTENERNODE_H
#include <utils/pimpl.h>

#include <core/forwarddecl.h>
#include <core/node.h>


namespace simplex
{
namespace core
{

class ListenerNodePrivate;
class CORE_SHARED_EXPORT ListenerNode : public Node
{
    PRIVATE_IMPL(ListenerNode)

public:
    ~ListenerNode() override;

    std::shared_ptr<ListenerNode> asListenerNode() override;
    std::shared_ptr<const ListenerNode> asListenerNode() const override;

protected:
    ListenerNode(const std::string& name);

    friend class Scene;
};

}
}

#endif // CORE_LISTENERNODE_H
