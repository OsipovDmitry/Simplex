#include <core/physicsrendererbase.h>
#include <core/staticbodynode.h>

#include "staticbodynodeprivate.h"

namespace simplex
{
namespace core
{

StaticBodyNode::StaticBodyNode(const std::string& name)
    : BodyNode(std::make_unique<StaticBodyNodePrivate>(*this, name))
{
    auto physicsRenderer = physics::RendererBase::current();
    if (!physicsRenderer)
    {
        LOG_CRITICAL << "Physics renderer can't be nullptr";
        return;
    }

    m().staticBody() = physicsRenderer->createStaticBody();
}

StaticBodyNode::~StaticBodyNode() = default;

std::shared_ptr<StaticBodyNode> StaticBodyNode::asStaticBodyNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<StaticBodyNode>(wp.lock());
}

std::shared_ptr<const StaticBodyNode> StaticBodyNode::asStaticBodyNode() const
{
    return const_cast<StaticBodyNode*>(this)->asStaticBodyNode();
}

std::shared_ptr<physics::IStaticBody> StaticBodyNode::staticBody()
{
    return m().staticBody();
}

std::shared_ptr<const physics::IStaticBody> StaticBodyNode::staticBody() const
{
    return const_cast<StaticBodyNode*>(this)->staticBody();
}

} // namespace core
} // namespace simplex
