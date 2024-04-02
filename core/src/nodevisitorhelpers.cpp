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

const utils::Frustum &FrustumCullingNodeVisitor::frustum() const
{
    return m_frustum;
}

void FrustumCullingNodeVisitor::setFrustum(const utils::Frustum &value)
{
    m_frustum = value;
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
                m_drawableNodes.insert(drawableNode);
        }
    }

    return result;
}

const DrawableNodesCollector::Collection &DrawableNodesCollector::drawableNodes() const
{
    return const_cast<DrawableNodesCollector*>(this)->drawableNodes();
}

DrawableNodesCollector::Collection &DrawableNodesCollector::drawableNodes()
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
    if (auto lightNode = node->asLightNode(); lightNode)
        if (lightNode->asDirectionalLightNode() || lightNode->asIBLLightNode())
        {
            m_lightNodes.insert(lightNode);
            return true;
        }

    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        if (auto lightNode = node->asLightNode(); lightNode)
        {
            if (m_transformedFrustum.contain(lightNode->areaBoundingBox()))
                m_lightNodes.insert(lightNode);
        }
    }

    return result;
}

const LightNodesCollector::Collection &LightNodesCollector::lightNodes() const
{
    return m_lightNodes;
}

LightNodesCollector::Collection &LightNodesCollector::lightNodes()
{
    return m_lightNodes;
}

// ZRangeNodeVisitor

ZRangeNodeVisitor::ZRangeNodeVisitor(const utils::OpenFrustum &openFrustum, bool drawables, bool lights)
    : FrustumCullingNodeVisitor(openFrustum)
    , m_zRange()
    , m_accountDrawables(drawables)
    , m_accountLights(lights)
{
}

bool ZRangeNodeVisitor::visit(const std::shared_ptr<Node> &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        const auto &transformedNearPlane = m_transformedFrustum.planes[utils::Frustum::castFromPlanes(utils::Frustum::Planes::Near)];
        utils::Range distances;

        if (auto drawableNode = node->asDrawableNode(); m_accountDrawables && drawableNode)
            distances = drawableNode->localBoundingBox().pairDistancesToPlane(transformedNearPlane);
        else if (auto lightNode = node->asLightNode(); m_accountLights && lightNode)
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

const utils::Frustum &ZRangeNodeVisitor::openFrustum() const
{
    return frustum();
}

void ZRangeNodeVisitor::setOpenFrustum(const utils::OpenFrustum &value)
{
    setFrustum(value);
}

const utils::Range &ZRangeNodeVisitor::zRange() const
{
    return m_zRange;
}

utils::Range &ZRangeNodeVisitor::zRange()
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
