#include <core/node.h>

#include "nodeprivate.h"
#include "nodevisitorhelpers.h"

namespace simplex
{
namespace core
{

NodePrivate::NodePrivate(Node &node, const std::string &name)
    : d_(node)
    , m_name(name)
    , m_isGlobalTransfomDirty(true)
{}

NodePrivate::~NodePrivate()
{
}

const Node &NodePrivate::node() const
{
    return d_;
}

const std::string &NodePrivate::name() const
{
    return m_name;
}

utils::Transform &NodePrivate::transform()
{
    return m_transform;
}

utils::Transform &NodePrivate::globalTransform()
{
    if (isGlobalTransfomDirty())
    {
        utils::Transform t = transform();
        if (auto parentNode = d_.parent(); parentNode)
            t = parentNode->globalTransform() * t;

        m_globalTransform = t;
        isGlobalTransfomDirty() = false;
    }

    return m_globalTransform;
}

void NodePrivate::doUpdate(uint64_t, uint32_t)
{
}

void NodePrivate::doBeforeTransformChanged()
{
}

void NodePrivate::doAfterTransformChanged()
{
}

void NodePrivate::doAttachToParent(const std::shared_ptr<Node>&)
{
}

void NodePrivate::doAttachToScene(const std::shared_ptr<Scene>&)
{
}

void NodePrivate::doDetachFromParent(const std::shared_ptr<Node>&)
{
}

void NodePrivate::doDetachFromScene(const std::shared_ptr<Scene>&)
{
}

bool &NodePrivate::isGlobalTransfomDirty()
{
    return m_isGlobalTransfomDirty;
}

void NodePrivate::dirtyGlobalTransform()
{
    DirtyGlobalTransformNodeVisitor dirtyGlobalTransformNodeVisitor;
    d_.acceptDown(dirtyGlobalTransformNodeVisitor);
}

}
}
