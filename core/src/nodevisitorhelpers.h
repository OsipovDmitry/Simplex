#ifndef CORE_NODEVISITORHELPERS_H
#define CORE_NODEVISITORHELPERS_H

#include <utils/forwarddecl.h>

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

// BoundingBoxChangedNodeVisitor

class BoundingBoxChangedNodeVisitor : public NodeVisitor
{
public:
    BoundingBoxChangedNodeVisitor();

    bool visit(Node&) override;
};

// FindRootNodeVisitor

class FindRootNodeVisitor : public NodeVisitor
{
public:
    FindRootNodeVisitor();

    bool visit(Node&) override;

    const std::shared_ptr<Node>& rootNode();

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

} // namespace core
} // namespace simplex

#endif // CORE_NODEVISITORHELPERS_H
