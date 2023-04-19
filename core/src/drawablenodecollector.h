#ifndef DRAWABLENODECOLLECTOR_H
#define DRAWABLENODECOLLECTOR_H

#include <core/nodecollector.h>

namespace simplex
{
namespace core
{

class DrawableNodeCollector : public NodeCollector<DrawableNode>
{
public:
    DrawableNodeCollector() : NodeCollector<DrawableNode>() {}

    bool visit(std::shared_ptr<Node> node) override
    {
        return NodeCollector<DrawableNode>::visit(node);
    }
};

}
}

#endif // DRAWABLENODECOLLECTOR_H
