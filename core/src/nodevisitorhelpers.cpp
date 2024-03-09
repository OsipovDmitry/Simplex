#include <core/scenerootnode.h>
#include <core/drawablenode.h>
#include <core/lightnode.h>

#include "nodeprivate.h"
#include "nodevisitorhelpers.h"

namespace simplex
{
namespace core
{

// UpdateNodeVisitor

UpdateNodeVisitor::UpdateNodeVisitor(uint64_t time, uint32_t dt)
    : NodeVisitor()
    , m_time(time)
    , m_dt(dt)
{}

bool UpdateNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    node->doUpdate(m_time, m_dt);
    return true;
}

// BeforeTransformChangedNodeVisitor

BeforeTransformChangedNodeVisitor::BeforeTransformChangedNodeVisitor()
    : NodeVisitor()
{
}

bool BeforeTransformChangedNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    node->doBeforeTransformChanged();
    return true;
}

// AfterTransformChangedNodeVisitor

AfterTransformChangedNodeVisitor::AfterTransformChangedNodeVisitor()
    : NodeVisitor()
{
}

bool AfterTransformChangedNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    node->doAfterTransformChanged();
    return true;
}

// FindRootNodeVisitor

FindRootNodeVisitor::FindRootNodeVisitor()
    : NodeVisitor()
{
}

bool FindRootNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    m_rootNode = node;
    return true;
}

const std::shared_ptr<Node> &FindRootNodeVisitor::rootNode()
{
    return m_rootNode;
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

// DrawableNodesCollector

DrawableNodesCollector::DrawableNodesCollector(const utils::Frustum &frustum)
    : FrustumCullingNodeVisitor(frustum)
{
}

bool DrawableNodesCollector::visit(const std::shared_ptr<Node> &node)
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

const std::deque<std::shared_ptr<DrawableNode>> &DrawableNodesCollector::drawableNodes() const
{
    return const_cast<DrawableNodesCollector*>(this)->drawableNodes();
}

std::deque<std::shared_ptr<DrawableNode>> &DrawableNodesCollector::drawableNodes()
{
    return m_drawableNodes;
}

// LightNodesCollector

LightNodesCollector::LightNodesCollector(const utils::Frustum &frustum)
    : FrustumCullingNodeVisitor(frustum)
{
}

bool LightNodesCollector::visit(const std::shared_ptr<Node> &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        if (auto lightNode = node->asLightNode(); lightNode)
        {
            if (m_transformedFrustum.contain(lightNode->areaBoundingBox()))
                m_lightNodes.push_back(lightNode);
        }
    }

    return result;
}

const std::deque<std::shared_ptr<LightNode>> &LightNodesCollector::lightNodes() const
{
    return m_lightNodes;
}

std::deque<std::shared_ptr<LightNode>> &LightNodesCollector::lightNodes()
{
    return m_lightNodes;
}

// ZRangeNodeVisitor

ZRangeNodeVisitor::ZRangeNodeVisitor(const utils::OpenFrustum &openFrustum)
    : FrustumCullingNodeVisitor(openFrustum)
    , m_zRange()
{
}

bool ZRangeNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        const auto &transformedNearPlane = m_transformedFrustum.planes[utils::Frustum::castFromPlanes(utils::Frustum::Planes::Near)];
        utils::Range distances;

        if (auto drawableNode = node->asDrawableNode(); drawableNode)
            distances = drawableNode->localBoundingBox().pairDistancesToPlane(transformedNearPlane);
        else if (auto lightNode = node->asLightNode(); lightNode)
            distances = lightNode->areaBoundingBox().pairDistancesToPlane(transformedNearPlane);

        if (distances.farValue() > 0.f)
        {
            const auto &nodeGlobalTransform = node->globalTransform();
            const auto &nearPlane = m_frustum.planes[utils::Frustum::castFromPlanes(utils::Frustum::Planes::Near)];
            for (auto &d : distances)
                d = nearPlane.distanceTo(nodeGlobalTransform * transformedNearPlane.anyPoint(d));

            m_zRange.setFarValue(glm::max(m_zRange.farValue(), distances.farValue()));

            distances.setNearValue(glm::max(distances.nearValue(), 0.f));
            m_zRange.setNearValue(glm::min(m_zRange.nearValue(), distances.nearValue()));
        }
    }

    return result;
}

const utils::Range &ZRangeNodeVisitor::zRange() const
{
    return m_zRange;
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

DirtyBoundingBoxNodeVisitor::DirtyBoundingBoxNodeVisitor(const std::unordered_set<BoundingBoxPolicy> &policies)
    : NodeVisitor()
    , m_policies(policies)
{
}

bool DirtyBoundingBoxNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    if (m_policies.count(node->boundingBoxPolicy()))
        node->m().isBoundingBoxDirty() = true;
    return true;
}


} // namespace
} // namespace
