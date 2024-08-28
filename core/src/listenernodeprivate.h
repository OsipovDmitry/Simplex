#ifndef CORE_LISTENERNODEPRIVATE_H
#define CORE_LISTENERNODEPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class ListenerNodePrivate : public NodePrivate
{
public:
    ListenerNodePrivate(ListenerNode&, const std::string&);
    ~ListenerNodePrivate() override;

};

}
}

#endif // CORE_LISTENERNODEPRIVATE_H
