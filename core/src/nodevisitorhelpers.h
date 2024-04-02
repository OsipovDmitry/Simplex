#ifndef CORE_NODEVISITORHELPERS_H
#define CORE_NODEVISITORHELPERS_H

#include <unordered_set>

#include <utils/forwarddecl.h>
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

    bool visit(const std::shared_ptr<Node>&) override;

private:
    uint64_t m_time;
    uint32_t m_dt;
};

// BeforeTransformChangedNodeVisitor

class BeforeTransformChangedNodeVisitor : public NodeVisitor
{
public:
    BeforeTransformChangedNodeVisitor();

    bool visit(const std::shared_ptr<Node>&) override;
};

// AfterTransformChangedNodeVisitor

class AfterTransformChangedNodeVisitor : public NodeVisitor
{
public:
    AfterTransformChangedNodeVisitor();

    bool visit(const std::shared_ptr<Node>&) override;
};

// FindRootNodeVisitor

class FindRootNodeVisitor : public NodeVisitor
{
public:
    FindRootNodeVisitor();

    bool visit(const std::shared_ptr<Node>&) override;

    const std::shared_ptr<Node> &rootNode();

protected:
    std::shared_ptr<Node> m_rootNode;
};

// FrustumCullingNodeCollector

class FrustumCullingNodeVisitor : public NodeVisitor
{
public:
    FrustumCullingNodeVisitor(const utils::Frustum&);

    bool visit(const std::shared_ptr<Node>&) override;

    const utils::Frustum &frustum() const;
    void setFrustum(const utils::Frustum&);

protected:
    utils::Frustum m_frustum;
    utils::Frustum m_transformedFrustum;
};

// DrawableNodesCollector

class DrawableNodesCollector : public FrustumCullingNodeVisitor
{
public:
    using Collection = std::unordered_set<std::shared_ptr<DrawableNode>>;

    DrawableNodesCollector(const utils::Frustum&);

    bool visit(const std::shared_ptr<Node>&) override;

    const Collection &drawableNodes() const;
    Collection &drawableNodes();

protected:
    Collection m_drawableNodes;
};

// LightNodesCollector

class LightNodesCollector : public FrustumCullingNodeVisitor
{
public:
    using Collection = std::unordered_set<std::shared_ptr<LightNode>>;

    LightNodesCollector(const utils::Frustum&);

    bool visit(const std::shared_ptr<Node>&) override;

    const Collection &lightNodes() const;
    Collection &lightNodes();

protected:
    Collection m_lightNodes;
};

// ZRangeNodeVisitor

class ZRangeNodeVisitor : public FrustumCullingNodeVisitor
{
public:
    ZRangeNodeVisitor(const utils::OpenFrustum&, bool drawables, bool lights);

    bool visit(const std::shared_ptr<Node>&) override;

    //const utils::Frustum &frustum() const = delete;
    //void setFrustum(const utils::Frustum&) = delete;

    const utils::Frustum &openFrustum() const;
    void setOpenFrustum(const utils::OpenFrustum&);

    const utils::Range &zRange() const;
    utils::Range &zRange();

protected:
    utils::Range m_zRange;
    bool m_accountDrawables;
    bool m_accountLights;
};

// DirtyGlobalTransformNodeVisitor

class DirtyGlobalTransformNodeVisitor : public NodeVisitor
{
public:
    DirtyGlobalTransformNodeVisitor();

    bool visit(const std::shared_ptr<Node>&) override;
};

// DirtyBoundingBoxNodeVisitor

class DirtyBoundingBoxNodeVisitor : public NodeVisitor
{
public:
    DirtyBoundingBoxNodeVisitor();

    bool visit(const std::shared_ptr<Node>&) override;

};




} // namespace
} // namespace

#endif // CORE_NODEVISITORHELPERS_H
