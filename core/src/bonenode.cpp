#include <core/bonenode.h>

#include "bonenodeprivate.h"

namespace simplex
{
namespace core
{

BoneNode::BoneNode(const std::string &name, uint32_t boneID)
    : Node(std::make_unique<BoneNodePrivate>(*this, name, boneID))
{
}

BoneNode::~BoneNode() = default;

std::shared_ptr<BoneNode> BoneNode::asBoneNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<BoneNode>(wp.lock());
}

std::shared_ptr<const BoneNode> BoneNode::asBoneNode() const
{
    return const_cast<BoneNode*>(this)->asBoneNode();
}

uint32_t BoneNode::boneID()
{
    return m().boneID();
}

std::shared_ptr<SkeletalAnimatedNode> BoneNode::asSkeletalAnimatedNode()
{
    return nullptr;
}

std::shared_ptr<const SkeletalAnimatedNode> BoneNode::asSkeletalAnimatedNode() const
{
    return nullptr;
}

BoneNode::BoneNode(std::unique_ptr<BoneNodePrivate>&& boneNodePrivate)
    : Node(std::move(boneNodePrivate))
{}

}
}
