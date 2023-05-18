#include <core/node.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

std::shared_ptr<IGraphicsRenderer::VertexArray> NodePrivate::s_boundingBoxVertexArray;

NodePrivate::NodePrivate(const std::string &name)
    : m_name(name)
    , m_globalTransfomDirty(true)
    , m_boundingBoxDirty(true)
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

bool &NodePrivate::isBoundingBoxDirty()
{
    return m_boundingBoxDirty;
}

utils::Transform &NodePrivate::transform()
{
    return m_transform;
}

utils::Transform &NodePrivate::globalTransform()
{
    return m_globalTransform;
}

utils::BoundingBox &NodePrivate::globalBoundingBox()
{
    return m_boundingBox;
}

void NodePrivate::doUpdate(uint64_t, uint32_t)
{
}

std::shared_ptr<ColoredDrawable> &NodePrivate::boundingBoxDrawable()
{
    return m_boundingBoxDrawable;
}

void NodePrivate::dirtyGlobalTransform(std::shared_ptr<Node> node)
{
    node->m().isGlobalTransformDirty() = true;
    for (auto &child : node->children())
        dirtyGlobalTransform(child);
}

void NodePrivate::dirtyBoundingBox(std::shared_ptr<Node> node)
{
    node->m().isBoundingBoxDirty() = true;
    if (auto parent = node->parent(); parent)
        dirtyBoundingBox(parent);
}

std::shared_ptr<IGraphicsRenderer::VertexArray> &NodePrivate::boundingBoxVertexArray()
{
    return s_boundingBoxVertexArray;
}

}
}
