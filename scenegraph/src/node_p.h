#ifndef NODE_P_H
#define NODE_P_H

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <scenegraph/forwarddecl.h>

namespace scenegraph
{

class NodePrivate
{
public:
    Node *pPublic;

    glm::mat4x4 cacheLocalTransform, cacheWorldTransform;

    SceneWPtr scene;
    NodeWPtr parent;
    NodeList children;
    DrawableList drawables;

    glm::quat orientation;
    glm::vec3 position;
    bool needUpdateTransform;

    NodePrivate(Node *pPublicNode, ScenePtr s);

    void updateTransform();
    void recalcTransform();
};

}

#endif // NODE_P_H
