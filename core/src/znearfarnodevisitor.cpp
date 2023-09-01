#include <limits>

#include <utils/frustum.h>

#include <core/node.h>
#include <core/drawablenode.h>

#include "znearfarnodevisitor.h"

namespace simplex
{
namespace core
{

class ZNearFarNodeVisitorCondition : public ConditionalNodeVisitor::AbstractCondition
{
public:
    ZNearFarNodeVisitorCondition(const utils::OpenFrustum &openFrustumWithZeroNearPlane);

    bool operator ()(std::shared_ptr<Node> node) override;
    const std::array<float, 2> &zNearFar() const;

private:
    std::array<float, 2> m_zNearFar;
    utils::OpenFrustum m_openFrustum;
};

ZNearFarNodeVisitorCondition::ZNearFarNodeVisitorCondition(const utils::OpenFrustum &openFrustumWithZeroNearPlane)
    : m_zNearFar({ std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() })
    , m_openFrustum(openFrustumWithZeroNearPlane)
{
}

bool ZNearFarNodeVisitorCondition::operator ()(std::shared_ptr<Node> node)
{
    const auto &nodeTransform = node->globalTransform();
    auto transformedFrustum = nodeTransform.inverted() * m_openFrustum;
    if (!transformedFrustum.contain(node->boundingBox()))
        return false;

    if (auto drawableNode = node->asDrawableNode(); drawableNode)
    {
        const auto &transformedNearPlane = transformedFrustum.planes[utils::Frustum::castFromPlanes(utils::Frustum::Planes::Near)];
        auto distances = drawableNode->localBoundingBox().pairDistancesToPlane(transformedNearPlane);

        if (distances[1] > 0.f)
        {
            const auto &nearPlane = m_openFrustum.planes[utils::Frustum::castFromPlanes(utils::Frustum::Planes::Near)];
            for (auto &d : distances)
                d = nearPlane.distanceTo(nodeTransform * transformedNearPlane.anyPoint(d));

            m_zNearFar[1] = glm::max(m_zNearFar[1], distances[1]);

            distances[0] = glm::max(distances[0], 0.f);
            m_zNearFar[0] = glm::min(m_zNearFar[0], distances[0]);
        }
    }

    return true;
}

const std::array<float, 2> &ZNearFarNodeVisitorCondition::zNearFar() const
{
    return m_zNearFar;
}

ZNearFarNodeVisitor::ZNearFarNodeVisitor(const utils::OpenFrustum &openFrustumWithZeroNearPlane)
    : ConditionalNodeVisitor(std::make_shared<ZNearFarNodeVisitorCondition>(openFrustumWithZeroNearPlane))
{
}

ZNearFarNodeVisitor::~ZNearFarNodeVisitor()
{
}

const std::array<float, 2> &ZNearFarNodeVisitor::zNearFar() const
{
    auto cond = std::dynamic_pointer_cast<const ZNearFarNodeVisitorCondition>(condition());
    assert(cond);

    return cond->zNearFar();
}


} // namespace
} // namespace
