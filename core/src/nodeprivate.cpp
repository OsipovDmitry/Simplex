#include "nodeprivate.h"

#include <utils/orientedboundingbox.h>

#include <core/node.h>

#include "nodevisitorhelpers.h"

namespace simplex
{
namespace core
{

NodePrivate::NodePrivate(Node& node, const std::string& name)
    : d_(node)
    , m_name(name)
{
}

NodePrivate::~NodePrivate() {}

const Node& NodePrivate::node() const
{
    return d_;
}

const std::string& NodePrivate::name() const
{
    return m_name;
}

utils::Transform& NodePrivate::transform()
{
    return m_transform;
}

utils::Transform& NodePrivate::globalTransform()
{
    if (isGlobalTransfomDirty())
    {
        utils::Transform t = transform();
        if (auto parentNode = d_.parent(); parentNode) t = parentNode->globalTransform() * t;

        m_globalTransform = t;
        isGlobalTransfomDirty() = false;
    }

    return m_globalTransform;
}

utils::BoundingBox& NodePrivate::boundingBox()
{
    if (isBoundingBoxDirty())
    {
        m_boundingBox = calculateBoundingBox();
        for (const auto& child : d_.children())
        {
            m_boundingBox +=
                static_cast<utils::BoundingBox>(child->transform() * utils::OrientedBoundingBox(child->boundingBox()));
        }
        isBoundingBoxDirty() = false;
    }

    return m_boundingBox;
}

const utils::BoundingBox& NodePrivate::calculateBoundingBox()
{
    static const auto s_emptyBoundingBox = utils::BoundingBox::empty();
    return s_emptyBoundingBox;
}

void NodePrivate::onUpdate(uint64_t, uint32_t) {}

void NodePrivate::onBeforeTransformChanged() {}

void NodePrivate::onAfterTransformChanged() {}

void NodePrivate::onBoundingBoxChanged() {}

void NodePrivate::onAttachToParent(const std::shared_ptr<Node>&) {}

void NodePrivate::onAttachToScene(const std::shared_ptr<Scene>&) {}

void NodePrivate::onDetachFromParent(const std::shared_ptr<Node>&) {}

void NodePrivate::onDetachFromScene(const std::shared_ptr<Scene>&) {}

bool& NodePrivate::isGlobalTransfomDirty()
{
    return m_isGlobalTransfomDirty;
}

void NodePrivate::dirtyGlobalTransform()
{
    DirtyGlobalTransformNodeVisitor dirtyGlobalTransformNodeVisitor;
    d_.acceptDown(dirtyGlobalTransformNodeVisitor);
}

bool& NodePrivate::isBoundingBoxDirty()
{
    return m_isBoundingBoxDirty;
}

void NodePrivate::dirtyBoundingBox()
{
    DirtyBoundingBoxNodeVisitor dirtyBoundingBoxNodeVisitor;
    d_.acceptUp(dirtyBoundingBoxNodeVisitor);
}

} // namespace core
} // namespace simplex
