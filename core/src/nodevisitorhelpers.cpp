#include <utils/logger.h>
#include <utils/epsilon.h>
#include <utils/clipspace.h>

#include <core/scenerootnode.h>
#include <core/visualdrawablenode.h>
#include <core/visualdrawable.h>
#include <core/lightnode.h>

#include "nodeprivate.h"
#include "visualdrawablenodeprivate.h"
#include "lightnodeprivate.h"
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

bool UpdateNodeVisitor::visit(Node &node)
{
    node.m().doUpdate(m_time, m_dt);
    return true;
}

// BeforeTransformChangedNodeVisitor

BeforeTransformChangedNodeVisitor::BeforeTransformChangedNodeVisitor()
    : NodeVisitor()
{
}

bool BeforeTransformChangedNodeVisitor::visit(Node &node)
{
    node.m().doBeforeTransformChanged();
    return true;
}

// AfterTransformChangedNodeVisitor

AfterTransformChangedNodeVisitor::AfterTransformChangedNodeVisitor()
    : NodeVisitor()
{
}

bool AfterTransformChangedNodeVisitor::visit(Node &node)
{
    node.m().doAfterTransformChanged();
    return true;
}

// FindRootNodeVisitor

FindRootNodeVisitor::FindRootNodeVisitor()
    : NodeVisitor()
{
}

bool FindRootNodeVisitor::visit(Node &node)
{
    m_rootNode = node.asNode();
    if (!m_rootNode)
        LOG_CRITICAL << "Node can't be nullptr";

    return true;
}

const std::shared_ptr<Node> &FindRootNodeVisitor::rootNode()
{
    return m_rootNode;
}

// DirtyGlobalTransformNodeVisitor

DirtyGlobalTransformNodeVisitor::DirtyGlobalTransformNodeVisitor()
    : NodeVisitor()
{
}

bool DirtyGlobalTransformNodeVisitor::visit(Node &node)
{
    node.m().isGlobalTransfomDirty() = true;
    return true;
}

// DirtyBoundingBoxNodeVisitor

DirtyBoundingBoxNodeVisitor::DirtyBoundingBoxNodeVisitor()
    : NodeVisitor()
{
}

bool DirtyBoundingBoxNodeVisitor::visit(Node &node)
{
    node.m().isBoundingBoxDirty() = true;
    return true;
}

//  AttachToSceneNodeVisitor

AttachToSceneNodeVisitor::AttachToSceneNodeVisitor(const std::shared_ptr<Scene>& scene)
    : NodeVisitor()
    , m_scene(scene)
{
}

bool AttachToSceneNodeVisitor::visit(Node &node)
{
    node.m().doAttachToScene(m_scene);
    return true;
}

//  DetachFromSceneNodeVisitor

DetachFromSceneNodeVisitor::DetachFromSceneNodeVisitor(const std::shared_ptr<Scene>& scene)
    : NodeVisitor()
    , m_scene(scene)
{
}

bool DetachFromSceneNodeVisitor::visit(Node &node)
{
    node.m().doDetachFromScene(m_scene);
    return true;
}

// FrustumCullingNodeCollector

FrustumCullingNodeVisitor::FrustumCullingNodeVisitor(const utils::Frustum &frustum)
    : NodeVisitor()
    , m_frustum(frustum)
    , m_transformedFrustum(frustum)
{
}

bool FrustumCullingNodeVisitor::visit(Node &node)
{
    m_transformedFrustum = node.globalTransform().inverted() * m_frustum;
    return m_transformedFrustum.classifyBoundingBox(node.boundingBox()) != -1;
}

const utils::Frustum &FrustumCullingNodeVisitor::frustum() const
{
    return m_frustum;
}

// DebugGeometryCollector

DebugGeometryCollector::DebugGeometryCollector(const utils::Frustum &frustum,
                                               bool nodeBoundingBox,
                                               bool visualDrawableNodeLocalBoundingBox,
                                               bool visualDrawableBoundingBox,
                                               bool lightNodeAreaBoundingBox)
    : FrustumCullingNodeVisitor(frustum)
    , m_nodeBoundingBoxesFlag(nodeBoundingBox)
    , m_visualDrawableNodeLocalBoundingBoxesFlag(visualDrawableNodeLocalBoundingBox)
    , m_visualDrawableBoundingBoxesFlag(visualDrawableBoundingBox)
    , m_lightNodeAreaBoundingBoxesFlag(lightNodeAreaBoundingBox)
{
}

bool DebugGeometryCollector::visit(Node &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        if (m_nodeBoundingBoxesFlag)
        {
            const auto &nodeBoundingBox = node.boundingBox();
            m_nodeBoundingBoxes.push_back(node.globalTransform() *
                                          glm::translate(glm::mat4x4(1.f), nodeBoundingBox.center()) *
                                          glm::scale(glm::mat4x4(1.f), nodeBoundingBox.halfSize()));
        }

        if (auto visualDrawableNode = node.asVisualDrawableNode(); visualDrawableNode)
        {
            const auto &visualDrawableNodeLocalBoundingBox = visualDrawableNode->m().localBoundingBox();
            if (m_transformedFrustum.classifyBoundingBox(visualDrawableNodeLocalBoundingBox) != -1)
            {
                if (m_visualDrawableNodeLocalBoundingBoxesFlag)
                {
                    m_visualDrawableNodeLocalBoundingBoxes.push_back(visualDrawableNode->globalTransform() *
                                                                     glm::translate(glm::mat4x4(1.f), visualDrawableNodeLocalBoundingBox.center()) *
                                                                     glm::scale(glm::mat4x4(1.f), visualDrawableNodeLocalBoundingBox.halfSize()));
                }

                if (m_visualDrawableBoundingBoxesFlag)
                {
                    for (auto &visualDrawable : visualDrawableNode->visualDrawables())
                    {
                        const auto &visualDrawableBoundingBox = visualDrawable->boundingBox();
                        if (m_transformedFrustum.classifyBoundingBox(visualDrawableBoundingBox) != -1)
                            m_visualDrawableBoundingBoxes.push_back(visualDrawableNode->globalTransform() *
                                                                    glm::translate(glm::mat4x4(1.f), visualDrawableBoundingBox.center()) *
                                                                    glm::scale(glm::mat4x4(1.f), visualDrawableBoundingBox.halfSize()));
                    }
                }
            }
        }
        else if (auto lightNode = node.asLightNode(); lightNode)
        {
            const auto &lightNodeAreaBoundingBox = lightNode->m().areaBoundingBox();
            if (m_transformedFrustum.classifyBoundingBox(lightNodeAreaBoundingBox) != -1)
            {
                if (m_lightNodeAreaBoundingBoxesFlag)
                {
                    m_lightNodeAreaBoundingBoxes.push_back(lightNode->globalTransform() *
                                                           glm::translate(glm::mat4x4(1.f), lightNodeAreaBoundingBox.center()) *
                                                           glm::scale(glm::mat4x4(1.f), lightNodeAreaBoundingBox.halfSize()));
                }
            }
        }
    }

    return result;
}

const DebugGeometryCollector::Collection &DebugGeometryCollector::nodeBoundingBoxes() const
{
    return const_cast<DebugGeometryCollector*>(this)->nodeBoundingBoxes();
}

DebugGeometryCollector::Collection &DebugGeometryCollector::nodeBoundingBoxes()
{
    return m_nodeBoundingBoxes;
}

const DebugGeometryCollector::Collection &DebugGeometryCollector::visualDrawableNodeLocalBoundingBoxes() const
{
    return const_cast<DebugGeometryCollector*>(this)->visualDrawableNodeLocalBoundingBoxes();
}

DebugGeometryCollector::Collection &DebugGeometryCollector::visualDrawableNodeLocalBoundingBoxes()
{
    return m_visualDrawableNodeLocalBoundingBoxes;
}

const DebugGeometryCollector::Collection &DebugGeometryCollector::visualDrawableBoundingBoxes() const
{
    return const_cast<DebugGeometryCollector*>(this)->visualDrawableBoundingBoxes();
}

DebugGeometryCollector::Collection &DebugGeometryCollector::visualDrawableBoundingBoxes()
{
    return m_visualDrawableBoundingBoxes;
}

const DebugGeometryCollector::Collection &DebugGeometryCollector::lightNodeAreaBoundingBoxes() const
{
    return const_cast<DebugGeometryCollector*>(this)->lightNodeAreaBoundingBoxes();
}

DebugGeometryCollector::Collection &DebugGeometryCollector::lightNodeAreaBoundingBoxes()
{
    return m_lightNodeAreaBoundingBoxes;
}

// VisualDrawablesCollector

VisualDrawablesCollector::VisualDrawablesCollector(const utils::Frustum &frustum)
    : FrustumCullingNodeVisitor(frustum)
{
}

bool VisualDrawablesCollector::visit(Node &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        if (auto visualDrawableNode = node.asVisualDrawableNode(); visualDrawableNode)
        {
            m_visualDrawableNodesCollection.push_back(visualDrawableNode);

            const glm::mat4x4 modelMatrix = visualDrawableNode->globalTransform();
            for (auto &visualDrawable : visualDrawableNode->visualDrawables())
                if (m_transformedFrustum.classifyBoundingBox(visualDrawable->boundingBox()) != -1)
                    m_visualDrawables.push_back(std::make_tuple(visualDrawable, modelMatrix));
        }
    }

    return result;
}

const VisualDrawablesCollector::VisualDrawableNodesCollection &VisualDrawablesCollector::visualDrawableNodesCollection() const
{
    return m_visualDrawableNodesCollection;
}

const VisualDrawablesCollector::VisualDrawablesCollection &VisualDrawablesCollector::visualDrawables() const
{
    return const_cast<VisualDrawablesCollector*>(this)->visualDrawables();
}

VisualDrawablesCollector::VisualDrawablesCollection &VisualDrawablesCollector::visualDrawables()
{
    return m_visualDrawables;
}

// LightNodesCollector

LightNodesCollector::LightNodesCollector(const utils::Frustum &frustum)
    : FrustumCullingNodeVisitor(frustum)
{
}

bool LightNodesCollector::visit(Node &node)
{
    auto lightNode = node.asLightNode();

    if (lightNode)
        if (lightNode->asDirectionalLightNode() || lightNode->asIBLLightNode())
        {
            m_lightNodes.push_back(lightNode);
            return true;
        }

    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result && lightNode)
    {
        if (m_transformedFrustum.classifyBoundingBox(lightNode->m().areaBoundingBox()) != -1)
            m_lightNodes.push_back(lightNode);
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

// FrustumBuilder

std::array<utils::Range(ZRangeCalculator::*)(const Node&, const utils::BoundingBox&), utils::numElementsClipSpaceType()> ZRangeCalculator::s_calculateZRangeForClipSpaceFuncs {
    &ZRangeCalculator::calculateZRangeForOrthoClipSpace,
    &ZRangeCalculator::calculateZRangeForPerspectiveClipSpace,
    &ZRangeCalculator::calculateZRangeForSphericalClipSpace,
};

ZRangeCalculator::ZRangeCalculator(const utils::Transform &viewTransform,
                                   const utils::ClipSpace &clipSpace,
                                   const utils::Range &cullPlanesLimits,
                                   bool visualDrawables, bool lights)
    : FrustumCullingNodeVisitor(utils::Frustum(viewTransform, clipSpace, cullPlanesLimits))
    , m_zRange()
    , m_accountVisualDrawables(visualDrawables)
    , m_accountLights(lights)
{
}

bool ZRangeCalculator::visit(Node &node)
{
    auto result = FrustumCullingNodeVisitor::visit(node);

    if (result)
    {
        utils::BoundingBox boundingBoxOfDrawable;
        if (auto visualDrawableNode = node.asVisualDrawableNode(); m_accountVisualDrawables && visualDrawableNode)
            boundingBoxOfDrawable = visualDrawableNode->m().localBoundingBox();
        else if (auto lightNode = node.asLightNode(); m_accountLights && lightNode)
            boundingBoxOfDrawable = lightNode->m().areaBoundingBox();

        if (!boundingBoxOfDrawable.isEmpty())
        {
            auto func = s_calculateZRangeForClipSpaceFuncs[utils::castFromClipSpaceType(frustum().clipSpace().type())];
            utils::Range zRange = (this->*func)(node, boundingBoxOfDrawable);
            (m_zRange += zRange).setNearValue(glm::max(m_zRange.nearValue(), 0.f));
        }
    }

    return result;
}

utils::Range ZRangeCalculator::resolveZRange() const
{
    const auto &cullPlaneLimits = frustum().zRange();
    utils::Range result(cullPlaneLimits.nearValue(), cullPlaneLimits.nearValue() + 1.f);
    if (!m_zRange.isEmpty())
    {
        static const auto s_nearCoef = 0.99f;
        static const auto s_farCoef = 1.01f;
        result = m_zRange.translated(cullPlaneLimits.nearValue());
        result = cullPlaneLimits * utils::Range(result.nearValue() * s_nearCoef, result.farValue() * s_farCoef);
    }
    return result;
}

utils::Range ZRangeCalculator::calculateZRangeForOrthoClipSpace(const Node &node, const utils::BoundingBox &boundingBoxOfDrawable)
{
    return calculateZRangeForPerspectiveClipSpace(node, boundingBoxOfDrawable);
}

utils::Range ZRangeCalculator::calculateZRangeForPerspectiveClipSpace(const Node &node, const utils::BoundingBox &boundingBoxOfDrawable)
{
    utils::Range result;
    if (int c = m_transformedFrustum.classifyBoundingBox(boundingBoxOfDrawable); c != -1)
    {
        std::vector<glm::vec3> points;
        points.reserve(utils::BoundingBox::numPoints() + utils::BoundingBox::numEdges() + utils::Frustum::numPoints());

        for (const auto &point : boundingBoxOfDrawable.points())
            if (m_transformedFrustum.classifyPoint(point) != -1)
                points.push_back(point);

        for (const auto &edge : boundingBoxOfDrawable.edges())
        {
            static const std::vector<utils::Frustum::PlaneIndex> s_sidePlanes {
                        utils::Frustum::PlaneIndex::Left,
                        utils::Frustum::PlaneIndex::Right,
                        utils::Frustum::PlaneIndex::Bottom,
                        utils::Frustum::PlaneIndex::Top
            };
            for (const auto planeIndex: s_sidePlanes)
            {
                const auto &transformedPlane = m_transformedFrustum.plane(planeIndex);
                if (glm::vec3 intersectionPoint; edge.intersectPlane(transformedPlane, intersectionPoint))
                    points.push_back(intersectionPoint);
            }
        }

        for (const auto &[index, transformedPoint] : m_transformedFrustum.points())
        {
            if (boundingBoxOfDrawable.classifyPoint(transformedPoint) != -1)
                points.push_back(transformedPoint);
        }

        const auto &nodeGlobalTransform = node.globalTransform();
        const auto &nearPlane = m_frustum.plane(utils::Frustum::PlaneIndex::Near);

        for (const auto &point : points)
            if (auto distance = nearPlane.distanceTo(nodeGlobalTransform * point); distance > -utils::epsilon<utils::Frustum::value_type>())
                result += distance;
    }

    return result;
}

utils::Range ZRangeCalculator::calculateZRangeForSphericalClipSpace(const Node &node, const utils::BoundingBox &boundingBoxOfDrawable)
{
    utils::Range result;

    auto transformedFrustumPosition = m_transformedFrustum.viewTransform().translation;
    const auto minPoint = glm::abs(boundingBoxOfDrawable.minPoint() - transformedFrustumPosition);
    const auto maxPoint = glm::abs(boundingBoxOfDrawable.maxPoint() - transformedFrustumPosition);

    for (size_t i = 0; i < utils::BoundingBox::length(); ++i)
    {
        if (minPoint[static_cast<glm::length_t>(i)] * maxPoint[static_cast<glm::length_t>(i)] < 0.f)
            continue;

        result += glm::abs(minPoint[static_cast<glm::length_t>(i)]);
        result += glm::abs(maxPoint[static_cast<glm::length_t>(i)]);
    }

    if (result.isEmpty())
    {
        result.setNearValue(0.f);

        auto farValue = -utils::maximum<float>();
        for (size_t i = 0; i < utils::BoundingBox::length(); ++i)
        {
            farValue = glm::max(farValue, glm::abs(minPoint[static_cast<glm::length_t>(i)]));
            farValue = glm::max(farValue, glm::abs(maxPoint[static_cast<glm::length_t>(i)]));
        }
        result.setFarValue(farValue);
    }

    return result.scale(node.globalTransform().scale);
}

} // namespace
} // namespace
