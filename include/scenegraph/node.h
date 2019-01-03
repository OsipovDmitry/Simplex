#ifndef NODE_H
#define NODE_H

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
