#include <utils/frustum.h>

#include <core/frustumcullingvisitor.h>
#include <core/node.h>

namespace simplex
{
namespace core
{

class FrustumCullingCondition : public ConditionalNodeVisitor::AbstractCondition
{
public:
    FrustumCullingCondition(const utils::Frustum &frustum)
        : m_frustum(frustum)
    {
    }

    bool operator ()(std::shared_ptr<Node> node) override
    {
        const auto &nodeBoundingBox = node->boundingBox();
        const auto &nodeTransform = node->globalTransform();

        m_transformedFrustum = nodeTransform.inverted() * m_frustum;
        if (!m_transformedFrustum.contain(nodeBoundingBox))
            return false;

        return true;
    }

    const utils::Frustum &transformedFrustum() const
    {
        return m_transformedFrustum;
    }

private:
    utils::Frustum m_frustum;
    utils::Frustum m_transformedFrustum;
};


FrustumCullingVisitor::FrustumCullingVisitor(const utils::Frustum &frustum)
    : ConditionalNodeVisitor(std::make_shared<FrustumCullingCondition>(frustum))
{
}

FrustumCullingVisitor::~FrustumCullingVisitor()
{
}

const utils::Frustum &FrustumCullingVisitor::transformedFrustum() const
{
    auto cond = std::dynamic_pointer_cast<const FrustumCullingCondition>(condition());
    assert(cond);

    return cond->transformedFrustum();
}

}
}
