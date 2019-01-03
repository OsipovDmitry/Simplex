#ifndef NODE_P_H
#define NODE_P_H

#include <scenegraph/forwarddecl.h>

namespace scenegraph
{

class NodePrivate
{
public:
    SceneWPtr scene;
    NodeWPtr parent;
    NodeList children;

    NodePrivate(ScenePtr s) :
        scene(s)
    {}
};

}

#endif // NODE_P_H
