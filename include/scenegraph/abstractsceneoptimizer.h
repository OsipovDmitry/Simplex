#ifndef ABSTRACTSCENEOPTIMIZER_H
#define ABSTRACTSCENEOPTIMIZER_H

#include "forwarddecl.h"
#include "scenegraph_global.h"
#include "../utils/enumclass.h"
#include "../utils/noncopyble.h"
#include "../utils/pimpl.h"

#include <logger/logger.h>

namespace scenegraph
{

class AbstractSceneOptimizerPrivate;
class SCENEGRAPHSHARED_EXPORT AbstractSceneOptimizer {
    PIMPL(AbstractSceneOptimizer)
    NONCOPYBLE(AbstractSceneOptimizer)

public:
    AbstractSceneOptimizer();
    virtual ~AbstractSceneOptimizer();

    //virtual SceneOptimizerType type() const = 0;

    virtual void nodeCreated(Node*) { LOG_INFO("Node created"); }
    virtual void nodeDestroyed(Node*) { LOG_INFO("Node destroyed"); }
    virtual void nodeAttached(Node*) { LOG_INFO("Node attached"); }
    virtual void nodeDetached(Node*) { LOG_INFO("Node detached"); }
    virtual void nodeTransformationChanged(Node*) {}
    virtual void nodeModelChanged(Node*) {}

private:
    AbstractSceneOptimizerPrivate *m_;

};

}


#endif // ABSTRACTSCENEOPTIMIZER_H
