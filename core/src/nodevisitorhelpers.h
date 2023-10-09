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

// NodeUpdateVisitor

class NodeUpdateVisitor : public NodeVisitor
{
public:
    NodeUpdateVisitor(uint64_t, uint32_t);

    bool visit(const std::shared_ptr<Node>&) override;

private:
    uint64_t m_time;
    uint32_t m_dt;
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

// DrawableNodeCollector

class DrawableNodeCollector : public FrustumCullingNodeVisitor
{
public:
    DrawableNodeCollector(const utils::Frustum&);

    bool visit(const std::shared_ptr<Node>&) override;

    const std::deque<std::shared_ptr<DrawableNode>> &drawableNodes() const;
    std::deque<std::shared_ptr<DrawableNode>> &drawableNodes();

protected:
    std::deque<std::shared_ptr<DrawableNode>> m_drawableNodes;
};

// ZNearFarNodeVisitor

class ZNearFarNodeVisitor : public FrustumCullingNodeVisitor
{
public:
    ZNearFarNodeVisitor(const utils::OpenFrustum&); // zNear of input frustum must be 0.0

    bool visit(const std::shared_ptr<Node>&) override;

    const std::array<float, 2u> &zNearFar() const;
    bool isEmpty() const;

protected:
    std::array<float, 2u> m_zNearFar;
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
