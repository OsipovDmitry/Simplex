#ifndef CORE_NODEVISITORHELPERS_H
#define CORE_NODEVISITORHELPERS_H

#include <deque>

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

protected:
    utils::Frustum m_frustum;
    utils::Frustum m_transformedFrustum;
};

// DrawableNodesCollector

class DrawableNodesCollector : public FrustumCullingNodeVisitor
{
public:
    DrawableNodesCollector(const utils::Frustum&);

    bool visit(const std::shared_ptr<Node>&) override;

    const std::deque<std::shared_ptr<DrawableNode>> &drawableNodes() const;
    std::deque<std::shared_ptr<DrawableNode>> &drawableNodes();

protected:
    std::deque<std::shared_ptr<DrawableNode>> m_drawableNodes;
};

// LightNodesCollector

class LightNodesCollector : public FrustumCullingNodeVisitor
{
public:
    LightNodesCollector(const utils::Frustum&);

    bool visit(const std::shared_ptr<Node>&) override;

    const std::deque<std::shared_ptr<LightNode>> &lightNodes() const;
    std::deque<std::shared_ptr<LightNode>> &lightNodes();

protected:
    std::deque<std::shared_ptr<LightNode>> m_lightNodes;
};



// ZNearFarNodeVisitor

class ZNearFarNodeVisitor : public FrustumCullingNodeVisitor
{
public:
    ZNearFarNodeVisitor(const utils::OpenFrustum&); // zNear of input frustum must be 0.0

    bool visit(const std::shared_ptr<Node>&) override;

    const utils::Range &zNearFar() const;
    bool isEmpty() const;

protected:
    utils::Range m_zNearFar;
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
    DirtyBoundingBoxNodeVisitor(const std::unordered_set<BoundingBoxPolicy>&);

    bool visit(const std::shared_ptr<Node>&) override;

protected:
    std::unordered_set<BoundingBoxPolicy> m_policies;

};




} // namespace
} // namespace

#endif // CORE_NODEVISITORHELPERS_H
