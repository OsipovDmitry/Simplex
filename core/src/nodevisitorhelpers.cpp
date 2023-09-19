#include <core/drawablenode.h>

#include "nodeprivate.h"
#include "nodevisitorhelpers.h"

namespace simplex
{
namespace core
{

// NodeUpdateVisitor

NodeUpdateVisitor::NodeUpdateVisitor(uint64_t time, uint32_t dt)
    : m_time(time)
    , m_dt(dt)
{}

bool NodeUpdateVisitor::visit(const std::shared_ptr<Node> &node)
{
    node->doUpdate(m_time, m_dt);
    return true;
}

// FrustumCullingNodeCollector

FrustumCullingNodeVisitor::FrustumCullingNodeVisitor(const utils::Frustum &frustum)
    : NodeVisitor()
    , m_frustum(frustum)
{
}

bool FrustumCullingNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    m_transformedFrustum = node->globalTransform().inverted() * m_frustum;
    return m_transformedFrustum.contain(node->boundingBox());
}

// DrawableNodeCollector

DrawableNodeCollector::DrawableNodeCollector(const utils::Frustum &frustum)
    : FrustumCullingNodeVisitor(frustum)
{
}

bool DrawableNodeCollector::visit(const std::shared_ptr<Node> &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        if (auto drawableNode = node->asDrawableNode(); drawableNode)
        {
            if (m_transformedFrustum.contain(drawableNode->localBoundingBox()))
                m_drawableNodes.push_back(drawableNode);
        }
    }

    return result;
}

const std::deque<std::shared_ptr<DrawableNode>> &DrawableNodeCollector::drawableNodes() const
{
    return const_cast<DrawableNodeCollector*>(this)->drawableNodes();
}

std::deque<std::shared_ptr<DrawableNode>> &DrawableNodeCollector::drawableNodes()
{
    return m_drawableNodes;
}

// ZNearFarNodeVisitor

ZNearFarNodeVisitor::ZNearFarNodeVisitor(const utils::OpenFrustum &openFrustum)
    : FrustumCullingNodeVisitor(openFrustum)
{
}

bool ZNearFarNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        if (auto drawableNode = node->asDrawableNode(); drawableNode)
        {
            const auto &transformedNearPlane = m_transformedFrustum.planes[utils::Frustum::castFromPlanes(utils::Frustum::Planes::Near)];
            auto distances = drawableNode->localBoundingBox().pairDistancesToPlane(transformedNearPlane);

            if (distances[1u] > 0.f)
            {
                const auto &nodeGlobalTransform = node->globalTransform();
                const auto &nearPlane = m_frustum.planes[utils::Frustum::castFromPlanes(utils::Frustum::Planes::Near)];
                for (auto &d : distances)
                    d = nearPlane.distanceTo(nodeGlobalTransform * transformedNearPlane.anyPoint(d));

                m_zNearFar[1u] = glm::max(m_zNearFar[1u], distances[1u]);

                distances[0u] = glm::max(distances[0u], 0.f);
                m_zNearFar[0u] = glm::min(m_zNearFar[0u], distances[0u]);
            }
        }
    }

    return result;
}

const std::array<float, 2u> &ZNearFarNodeVisitor::zNearFar() const
{
    return m_zNearFar;
}

// DirtyGlobalTransformNodeVisitor

DirtyGlobalTransformNodeVisitor::DirtyGlobalTransformNodeVisitor()
    : NodeVisitor()
{
}

bool DirtyGlobalTransformNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    node->m().isGlobalTransformDirty() = true;
    return true;
}

// DirtyBoundingBoxNodeVisitor

DirtyBoundingBoxNodeVisitor::DirtyBoundingBoxNodeVisitor()
    : NodeVisitor()
{
}

bool DirtyBoundingBoxNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    node->m().isBoundingBoxDirty() = true;
    return true;
}




} // namespace
} // namespace
