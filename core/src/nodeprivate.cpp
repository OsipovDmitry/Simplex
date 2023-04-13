#include <core/node.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

NodePrivate::NodePrivate(const std::string &name)
    : m_name(name)
{}

NodePrivate::~NodePrivate()
{
}

const std::string &NodePrivate::name() const
{
    return m_name;
}

bool &NodePrivate::isGlobalTransformDirty()
{
    return m_globalTransfomDirty;
}

bool &NodePrivate::isGlobalBoundingBoxDirty()
{
    return m_globalBoundingBoxDirty;
}

utils::Transform &NodePrivate::localTransform()
{
    return m_localTransform;
}

utils::Transform &NodePrivate::globalTransform()
{
    return m_globalTransform;
}

utils::BoundingBox &NodePrivate::minimalBoundingBox()
{
    return m_minimalBoundingBox;
}

utils::BoundingBox &NodePrivate::localBoundingBox()
{
    static utils::BoundingBox s_localBoundingBox;
    return s_localBoundingBox;
}

utils::BoundingBox &NodePrivate::globalBoundingBox()
{
    return m_globalBoundingBox;
}

void NodePrivate::doUpdate(uint64_t, uint32_t)
{
}

void NodePrivate::dirtyGlobalTransform(std::shared_ptr<Node> node)
{
    node->m().isGlobalTransformDirty() = true;
    for (auto child : node->children())
        dirtyGlobalTransform(child);
}

void NodePrivate::dirtyGlobalBoundingBox(std::shared_ptr<Node> node)
{
    node->m().isGlobalBoundingBoxDirty() = true;
    if (auto parent = node->parent(); parent)
        dirtyGlobalBoundingBox(parent);
}

}
}
