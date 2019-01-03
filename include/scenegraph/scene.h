#ifndef SCENE_H
#define SCENE_H

#include "node.h"

namespace scenegraph
{

class ScenePrivate;
class SCENEGRAPHSHARED_EXPORT Scene : public Node
{
public:
    static ScenePtr create(AbstractSceneOptimizerPtr optimizer);

private:
    Scene(AbstractSceneOptimizerPtr optimizer);
    ~Scene();
};

}

#endif // SCENE_H
