#include <core/kinematicbodynode.h>
#include <core/physicsrendererbase.h>

#include "kinematicbodynodeprivate.h"

namespace simplex
{
namespace core
{

KinematicBodyNode::KinematicBodyNode(const std::string& name)
    : BodyNode(std::make_unique<KinematicBodyNodePrivate>(*this, name))
{
    auto physicsRenderer = physics::RendererBase::current();
    if (!physicsRenderer)
    {
        LOG_CRITICAL << "Physics renderer can't be nullptr";
        return;
    }

    m().kinematicBody() = physicsRenderer->createKinematicBody();
}

KinematicBodyNode::~KinematicBodyNode() = default;

std::shared_ptr<KinematicBodyNode> KinematicBodyNode::asKinematicBodyNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<KinematicBodyNode>(wp.lock());
}

std::shared_ptr<const KinematicBodyNode> KinematicBodyNode::asKinematicBodyNode() const
{
    return const_cast<KinematicBodyNode*>(this)->asKinematicBodyNode();
}

std::shared_ptr<physics::IKinematicBody> KinematicBodyNode::kinematicBody()
{
    return m().kinematicBody();
}

std::shared_ptr<const physics::IKinematicBody> KinematicBodyNode::kinematicBody() const
{
    return const_cast<KinematicBodyNode*>(this)->kinematicBody();
}

} // namespace core
} // namespace simplex
