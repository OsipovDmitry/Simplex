#include <core/node.h>

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

NodePrivate::NodePrivate(const std::string &name)
    : m_name(name)
    , m_boundingBoxPolicy(BoundingBoxPolicy::Standard)
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

utils::BoundingBox &NodePrivate::boundingBox()
{
    return m_boundingBox;
}

BoundingBoxPolicy &NodePrivate::boundingBoxPolicy()
{
    return m_boundingBoxPolicy;
}

}
}
