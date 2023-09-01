#include <core/frustumcullingnodevisitor.h>
#include <core/node.h>

#include "frustumcullingnodevisitorprivate.h"

namespace simplex
{
namespace core
{

FrustumCullingNodeVisitor::FrustumCullingCondition::FrustumCullingCondition(const utils::Frustum &frustum)
    : m_frustum(frustum)
{
}

bool FrustumCullingNodeVisitor::FrustumCullingCondition::operator ()(std::shared_ptr<Node> node)
{
    const auto &nodeBoundingBox = node->boundingBox();
    const auto &nodeTransform = node->globalTransform();

    m_transformedFrustum = nodeTransform.inverted() * m_frustum;
    if (!m_transformedFrustum.contain(nodeBoundingBox))
        return false;

    return true;
}

const utils::Frustum &FrustumCullingNodeVisitor::FrustumCullingCondition::transformedFrustum() const
{
    return m_transformedFrustum;
}

FrustumCullingNodeVisitor::FrustumCullingNodeVisitor(const utils::Frustum &frustum)
    : FrustumCullingNodeVisitor(new FrustumCullingNodeVisitorPrivate(std::make_shared<FrustumCullingCondition>(frustum)))
{
}

FrustumCullingNodeVisitor::~FrustumCullingNodeVisitor()
{
}

bool FrustumCullingNodeVisitor::visit(std::shared_ptr<Node> node)
{
    auto result = ConditionalNodeVisitor::visit(node);

    if (result)
        m().nodes().push_back(node);

    return result;
}

const std::deque<std::shared_ptr<Node> > &FrustumCullingNodeVisitor::nodes() const
{
    return m().nodes();
}

const utils::Frustum &FrustumCullingNodeVisitor::transformedFrustum() const
{
    auto cond = std::dynamic_pointer_cast<const FrustumCullingCondition>(condition());
    assert(cond);

    return cond->transformedFrustum();
}

FrustumCullingNodeVisitor::FrustumCullingNodeVisitor(FrustumCullingNodeVisitorPrivate *mPrivate)
    : ConditionalNodeVisitor(mPrivate)

{

}

}
}
