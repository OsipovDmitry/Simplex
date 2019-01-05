#ifndef ABSTRACTSCENEOPTIMIZER_H
#define ABSTRACTSCENEOPTIMIZER_H

#include "forwarddecl.h"
#include "scenegraph_global.h"
#include "../utils/enumclass.h"
#include "../utils/noncopyble.h"
#include "../utils/pimpl.h"

namespace scenegraph
{

class AbstractSceneOptimizerPrivate;
class SCENEGRAPHSHARED_EXPORT AbstractSceneOptimizer {
    PIMPL(AbstractSceneOptimizer)
    NONCOPYBLE(AbstractSceneOptimizer)

public:
    AbstractSceneOptimizer();
    virtual ~AbstractSceneOptimizer();

    virtual void nodeCreated(Node*) {}
    virtual void nodeDestroyed(Node*) {}
    virtual void nodeAttached(Node*) {}
    virtual void nodeDetached(Node*) {}
    virtual void nodeTransformationChanged(Node*) {}
    virtual void nodeModelChanged(Node*) {}

private:
    AbstractSceneOptimizerPrivate *m_;

};

}


#endif // ABSTRACTSCENEOPTIMIZER_H
