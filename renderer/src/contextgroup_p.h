#ifndef CONTEXTGROUP_P_H
#define CONTEXTGROUP_P_H

#include <list>

#include <renderer/forwarddecl.h>

namespace renderer
{

class ContextGroupPrivate
{
public:
    ContextGroupPrivate();
    ~ContextGroupPrivate();

    void add(ContextPtr context);
    void remove(ContextPtr context);
    bool find(ContextPtr context) const;

    std::list<ContextWPtr> contexts;
};

}

#endif // CONTEXTGROUP_P_H
