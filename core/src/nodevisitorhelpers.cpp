#include "nodevisitorhelpers.h"

#include <core/node.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

// UpdateNodeVisitor

UpdateNodeVisitor::UpdateNodeVisitor(uint64_t time, uint32_t dt)
    : NodeVisitor()
    , m_time(time)
    , m_dt(dt)
{
}

bool UpdateNodeVisitor::visit(Node& node)
{
    node.m().onUpdate(m_time, m_dt);
    return true;
}

// BeforeTransformChangedNodeVisitor

BeforeTransformChangedNodeVisitor::BeforeTransformChangedNodeVisitor()
    : NodeVisitor()
{
}

bool BeforeTransformChangedNodeVisitor::visit(Node& node)
{
    node.m().onBeforeTransformChanged();
    return true;
}

// AfterTransformChangedNodeVisitor

AfterTransformChangedNodeVisitor::AfterTransformChangedNodeVisitor()
    : NodeVisitor()
{
}

bool AfterTransformChangedNodeVisitor::visit(Node& node)
{
    node.m().onAfterTransformChanged();
    return true;
}

// BoundingBoxChangedNodeVisitor

BoundingBoxChangedNodeVisitor::BoundingBoxChangedNodeVisitor()
    : NodeVisitor()
{
}

bool BoundingBoxChangedNodeVisitor::visit(Node& node)
{
    node.m().onBoundingBoxChanged();
    return true;
}

// FindRootNodeVisitor

FindRootNodeVisitor::FindRootNodeVisitor()
    : NodeVisitor()
{
}

bool FindRootNodeVisitor::visit(Node& node)
{
    m_rootNode = node.asNode();
    return true;
}

const std::shared_ptr<Node>& FindRootNodeVisitor::rootNode()
{
    return m_rootNode;
}

// DirtyGlobalTransformNodeVisitor

DirtyGlobalTransformNodeVisitor::DirtyGlobalTransformNodeVisitor()
    : NodeVisitor()
{
}

bool DirtyGlobalTransformNodeVisitor::visit(Node& node)
{
    node.m().isGlobalTransfomDirty() = true;
    return true;
}

// DirtyBoundingBoxNodeVisitor

DirtyBoundingBoxNodeVisitor::DirtyBoundingBoxNodeVisitor()
    : NodeVisitor()
{
}

bool DirtyBoundingBoxNodeVisitor::visit(Node& node)
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

bool AttachToSceneNodeVisitor::visit(Node& node)
{
    node.m().onAttachToScene(m_scene);
    return true;
}

//  DetachFromSceneNodeVisitor

DetachFromSceneNodeVisitor::DetachFromSceneNodeVisitor(const std::shared_ptr<Scene>& scene)
    : NodeVisitor()
    , m_scene(scene)
{
}

bool DetachFromSceneNodeVisitor::visit(Node& node)
{
    node.m().onDetachFromScene(m_scene);
    return true;
}

} // namespace core
} // namespace simplex
