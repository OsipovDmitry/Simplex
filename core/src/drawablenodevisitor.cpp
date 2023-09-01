#include <utils/frustum.h>

#include <core/node.h>
#include <core/drawablenode.h>

#include "drawablenodevisitor.h"
#include "drawablenodevisitorprivate.h"

namespace simplex
{
namespace core
{

DrawableNodeVisitor::DrawableNodeVisitor(const utils::Frustum &frustum)
    : FrustumCullingNodeVisitor(new DrawableNodeVisitorPrivate(std::make_shared<FrustumCullingCondition>(frustum)))
{
}

bool DrawableNodeVisitor::visit(std::shared_ptr<Node> node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
        if (auto drawableNode = node->asDrawableNode(); drawableNode && transformedFrustum().contain(drawableNode->localBoundingBox()))
            m().drawableNodes().push_back(drawableNode);

    return result;
}

const std::deque<std::shared_ptr<DrawableNode> > &DrawableNodeVisitor::drawableNodes() const
{
    return m().drawableNodes();
}

}
}
