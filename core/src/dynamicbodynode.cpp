#include <core/dynamicbodynode.h>
#include <core/physicsrendererbase.h>

#include "dynamicbodynodeprivate.h"

namespace simplex
{
namespace core
{

DynamicBodyNode::DynamicBodyNode(const std::string& name)
    : BodyNode(std::make_unique<DynamicBodyNodePrivate>(*this, name))
{
    auto physicsRenderer = physics::RendererBase::current();
    if (!physicsRenderer)
    {
        LOG_CRITICAL << "Physics renderer can't be nullptr";
        return;
    }

    m().dynamicBody() = physicsRenderer->createDynamicBody();
}

DynamicBodyNode::~DynamicBodyNode() = default;

std::shared_ptr<DynamicBodyNode> DynamicBodyNode::asDynamicBodyNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<DynamicBodyNode>(wp.lock());
}

std::shared_ptr<const DynamicBodyNode> DynamicBodyNode::asDynamicBodyNode() const
{
    return const_cast<DynamicBodyNode*>(this)->asDynamicBodyNode();
}

std::shared_ptr<physics::IDynamicBody> DynamicBodyNode::dynamicBody()
{
    return m().dynamicBody();
}

std::shared_ptr<const physics::IDynamicBody> DynamicBodyNode::dynamicBody() const
{
    return const_cast<DynamicBodyNode*>(this)->dynamicBody();
}

} // namespace core
} // namespace simplex
