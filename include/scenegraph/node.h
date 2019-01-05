#ifndef NODE_H
#define NODE_H

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include "scenegraph_global.h"
#include "forwarddecl.h"
#include "../utils/pimpl.h"
#include "../utils/noncopyble.h"
#include "../utils/customdeleter.h"

namespace scenegraph
{

class NodePrivate;
class SCENEGRAPHSHARED_EXPORT Node : public std::enable_shared_from_this<Node>
{
    PIMPL(Node)
    NONCOPYBLE(Node)
    CUSTOMDELETER(Node)

public:
    ScenePtr scene() const;
    NodePtr parent() const;
    const NodeList children() const;

    void detach(NodePtr node);
    void attach(NodePtr node);

    glm::vec3 position() const;
    void setPosition(const glm::vec3& value);

    glm::quat orientation() const;
    void setOrientation(const glm::quat& value) const;

    const glm::mat4x4& localTransform() const;
    const glm::mat4x4& worldTransform() const;

    static NodePtr create(ScenePtr scene);

protected:
    Node(NodePrivate *p);
    virtual ~Node();

    NodePrivate *m_;

private:
    Node(ScenePtr scene);

};

}

#endif // NODE_H
