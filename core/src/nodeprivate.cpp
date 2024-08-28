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
    , m_isBoundingBoxDirty(true)
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

utils::BoundingBox &NodePrivate::boundingBox()
{
    if (isBoundingBoxDirty())
    {
        auto bb = doBoundingBox();
        for (const auto &child : d_.children())
            bb += child->transform() * child->boundingBox();

//        if (asSceneRootNode())
//            bb = utils::Transform::fromScale(1.05f) * bb;

        m_boundingBox = bb;
        isBoundingBoxDirty() = false;
    }

    return m_boundingBox;
}

void NodePrivate::doUpdate(uint64_t, uint32_t)
{
}

utils::BoundingBox NodePrivate::doBoundingBox()
{
    return utils::BoundingBox();
}

void NodePrivate::doBeforeTransformChanged()
{
}

void NodePrivate::doAfterTransformChanged()
{
}

void NodePrivate::doAttachToParent()
{
}

void NodePrivate::doAttachToScene()
{
}

void NodePrivate::doDetachFromParent()
{
}

void NodePrivate::doDetachFromScene()
{
}

bool &NodePrivate::isGlobalTransfomDirty()
{
    return m_isGlobalTransfomDirty;
}

bool &NodePrivate::isBoundingBoxDirty()
{
    return m_isBoundingBoxDirty;
}

void NodePrivate::dirtyGlobalTransform()
{
    DirtyGlobalTransformNodeVisitor dirtyGlobalTransformNodeVisitor;
    d_.acceptDown(dirtyGlobalTransformNodeVisitor);
}

void NodePrivate::dirtyBoundingBox()
{
    DirtyBoundingBoxNodeVisitor dirtyBoundingBoxNodeVisitor;
    d_.acceptUp(dirtyBoundingBoxNodeVisitor);
}

}
}
