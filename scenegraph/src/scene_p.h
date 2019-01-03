#ifndef SCENE_P_H
#define SCENE_P_H

#include "node_p.h"

namespace scenegraph
{

class ScenePrivate : public NodePrivate
{
public:
    AbstractSceneOptimizerPtr optimizer;

    ScenePrivate(AbstractSceneOptimizerPtr o) :
        NodePrivate(nullptr),
        optimizer(o)
    {}
};

}

#endif // SCENE_P_H
