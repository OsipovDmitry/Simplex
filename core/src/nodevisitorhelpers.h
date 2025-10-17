#ifndef CORE_NODEVISITORHELPERS_H
#define CORE_NODEVISITORHELPERS_H

#include <deque>

#include <utils/forwarddecl.h>
#include <utils/clipspace.h>
#include <utils/frustum.h>

#include <core/nodevisitor.h>

namespace simplex
{
namespace core
{

// UpdateNodeVisitor

class UpdateNodeVisitor : public NodeVisitor
{
public:
    UpdateNodeVisitor(uint64_t, uint32_t);

    bool visit(Node&) override;

private:
    uint64_t m_time;
    uint32_t m_dt;
};

// BeforeTransformChangedNodeVisitor

class BeforeTransformChangedNodeVisitor : public NodeVisitor
{
public:
    BeforeTransformChangedNodeVisitor();

    bool visit(Node&) override;
};

// AfterTransformChangedNodeVisitor

class AfterTransformChangedNodeVisitor : public NodeVisitor
{
public:
    AfterTransformChangedNodeVisitor();

    bool visit(Node&) override;
};

// FindRootNodeVisitor

class FindRootNodeVisitor : public NodeVisitor
{
public:
    FindRootNodeVisitor();

    bool visit(Node&) override;

    const std::shared_ptr<Node> &rootNode();

protected:
    std::shared_ptr<Node> m_rootNode;
};

// DirtyGlobalTransformNodeVisitor

class DirtyGlobalTransformNodeVisitor : public NodeVisitor
{
public:
    DirtyGlobalTransformNodeVisitor();

    bool visit(Node&) override;
};

// DirtyBoundingBoxNodeVisitor

class DirtyBoundingBoxNodeVisitor : public NodeVisitor
{
public:
    DirtyBoundingBoxNodeVisitor();

    bool visit(Node&) override;

};

// AttachToSceneNodeVisitor

class AttachToSceneNodeVisitor : public NodeVisitor
{
public:
    AttachToSceneNodeVisitor(const std::shared_ptr<Scene>&);

    bool visit(Node&) override;

private:
    std::shared_ptr<Scene> m_scene;
};

// DetachFromSceneNodeVisitor

class DetachFromSceneNodeVisitor : public NodeVisitor
{
public:
    DetachFromSceneNodeVisitor(const std::shared_ptr<Scene>&);

    bool visit(Node&) override;

private:
    std::shared_ptr<Scene> m_scene;
};

// FrustumCullingNodeCollector

class FrustumCullingNodeVisitor : public NodeVisitor
{
public:
    FrustumCullingNodeVisitor(const utils::Frustum&);

    bool visit(Node&) override;

    const utils::Frustum &frustum() const;

protected:
    utils::Frustum m_frustum;
    utils::Frustum m_transformedFrustum;
};

// DebugCollector

class DebugGeometryCollector : public FrustumCullingNodeVisitor
{
public:
    using Collection = std::deque<glm::mat4x4>;

    DebugGeometryCollector(const utils::Frustum&,
                           bool nodeBoundingBox,
                           bool drawableNodeLocalBoundingBox,
                           bool drawableBoundingBox,
                           bool lightNodeAreaBoundingBox);

    bool visit(Node&) override;

    const Collection &nodeBoundingBoxes() const;
    Collection &nodeBoundingBoxes();

    const Collection &drawableNodeLocalBoundingBoxes() const;
    Collection &drawableNodeLocalBoundingBoxes();

    const Collection &drawableBoundingBoxes() const;
    Collection &drawableBoundingBoxes();

    const Collection &lightNodeAreaBoundingBoxes() const;
    Collection &lightNodeAreaBoundingBoxes();

protected:
    Collection m_nodeBoundingBoxes;
    Collection m_drawableNodeLocalBoundingBoxes;
    Collection m_drawableBoundingBoxes;
    Collection m_lightNodeAreaBoundingBoxes;

    bool m_nodeBoundingBoxesFlag;
    bool m_drawableNodeLocalBoundingBoxesFlag;
    bool m_drawableBoundingBoxesFlag;
    bool m_lightNodeAreaBoundingBoxesFlag;
};

// DrawablesCollector

class DrawablesCollector : public FrustumCullingNodeVisitor
{
public:
    using DrawableNodesCollection = std::deque<std::shared_ptr<DrawableNode>>;
    using DrawablesCollection = std::deque<std::tuple<std::shared_ptr<Drawable>, glm::mat4x4>>;

    DrawablesCollector(const utils::Frustum&);

    bool visit(Node&) override;

    const DrawableNodesCollection &drawableNodesCollection() const;

    const DrawablesCollection &drawables() const;
    DrawablesCollection &drawables();

protected:
    DrawableNodesCollection m_drawableNodesCollection;
    DrawablesCollection m_drawables;
};

// LightNodesCollector

class LightNodesCollector : public FrustumCullingNodeVisitor
{
public:
    using Collection = std::deque<std::shared_ptr<LightNode>>;

    LightNodesCollector(const utils::Frustum&);

    bool visit(Node&) override;

    const Collection &lightNodes() const;
    Collection &lightNodes();

protected:
    Collection m_lightNodes;
};

// FrustumBuilder

class ZRangeCalculator : public FrustumCullingNodeVisitor
{
public:
    ZRangeCalculator(const utils::Transform &viewTransform,
                     const utils::ClipSpace &clipSpace,
                     const utils::Range &cullPlanesLimits,
                     bool drawables, bool lights);

    bool visit(Node&) override;

    utils::Range resolveZRange() const;

protected:
    utils::Range m_zRange;
    bool m_accountDrawables;
    bool m_accountLights;

    utils::Range calculateZRangeForOrthoClipSpace(const Node&, const utils::BoundingBox&);
    utils::Range calculateZRangeForPerspectiveClipSpace(const Node&, const utils::BoundingBox&);
    utils::Range calculateZRangeForSphericalClipSpace(const Node&, const utils::BoundingBox&);
    static std::array<utils::Range(ZRangeCalculator::*)(const Node&, const utils::BoundingBox&), utils::numElementsClipSpaceType()> s_calculateZRangeForClipSpaceFuncs;
};

} // namespace
} // namespace

#endif // CORE_NODEVISITORHELPERS_H
