#ifndef CONTEXTGROUP_H
#define CONTEXTGROUP_H

#include <list>

#include "../utils/pimpl.h"
#include "renderer_global.h"
#include "forwarddecl.h"

namespace renderer {

class ContextGroupPrivate;
class ContextGroup {
    PIMPL(ContextGroup)

public:
    ContextGroup();
    ~ContextGroup();

    const std::list<ContextWPtr>& contexts() const;

private:
    ContextGroupPrivate *m_;

};

}

#endif // CONTEXTGROUP_H
