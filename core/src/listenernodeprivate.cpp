#include <core/listenernode.h>

#include "listenernodeprivate.h"

namespace simplex
{
namespace core
{

ListenerNodePrivate::ListenerNodePrivate(ListenerNode &listenerNode, const std::string &name)
    : NodePrivate(listenerNode, name)
{
}

ListenerNodePrivate::~ListenerNodePrivate() = default;

}
}
