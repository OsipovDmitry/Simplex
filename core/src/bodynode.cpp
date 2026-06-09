#include <core/bodynode.h>

#include "bodynodeprivate.h"

namespace simplex
{
namespace core
{

BodyNode::~BodyNode() = default;

std::shared_ptr<BodyNode> BodyNode::asBodyNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<BodyNode>(wp.lock());
}

std::shared_ptr<const BodyNode> BodyNode::asBodyNode() const
{
    return const_cast<BodyNode*>(this)->asBodyNode();
}

std::shared_ptr<physics::IBody> BodyNode::body()
{
    return m().body();
}

std::shared_ptr<const physics::IBody> BodyNode::body() const
{
    return const_cast<BodyNode*>(this)->body();
}

std::shared_ptr<StaticBodyNode> BodyNode::asStaticBodyNode()
{
    return nullptr;
}

std::shared_ptr<const StaticBodyNode> BodyNode::asStaticBodyNode() const
{
    return nullptr;
}

std::shared_ptr<KinematicBodyNode> BodyNode::asKinematicBodyNode()
{
    return nullptr;
}

std::shared_ptr<const KinematicBodyNode> BodyNode::asKinematicBodyNode() const
{
    return nullptr;
}

std::shared_ptr<DynamicBodyNode> BodyNode::asDynamicBodyNode()
{
    return nullptr;
}

std::shared_ptr<const DynamicBodyNode> BodyNode::asDynamicBodyNode() const
{
    return nullptr;
}

BodyNode::BodyNode(std::unique_ptr<BodyNodePrivate> bodyNodePrivate)
    : Node(std::move(bodyNodePrivate))
{
}

} // namespace core
} // namespace simplex
