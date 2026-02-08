#include <utils/logger.h>

#include <core/skeletalanimation.h>
#include <core/skeletalanimatednode.h>

#include "skeletalanimatednodeprivate.h"

namespace simplex
{
namespace core
{

SkeletalAnimatedNode::SkeletalAnimatedNode(const std::string &name, uint32_t boneID, const std::shared_ptr<Skeleton>& skeleton)
    : BoneNode(std::make_unique<SkeletalAnimatedNodePrivate>(*this, name, boneID, skeleton))
{
    setCurrentAnimation(/*"Take 001"*/"mixamorig_Hips");
}

SkeletalAnimatedNode::~SkeletalAnimatedNode() = default;

std::shared_ptr<SkeletalAnimatedNode> SkeletalAnimatedNode::asSkeletalAnimatedNode()
{
    auto wp = weak_from_this();
    return wp.expired() ? nullptr : std::dynamic_pointer_cast<SkeletalAnimatedNode>(wp.lock());
}

std::shared_ptr<const SkeletalAnimatedNode> SkeletalAnimatedNode::asSkeletalAnimatedNode() const
{
    return const_cast<SkeletalAnimatedNode*>(this)->asSkeletalAnimatedNode();
}

const std::shared_ptr<Skeleton>& SkeletalAnimatedNode::skeleton() const
{
    return m().skeleton();
}

const std::string& SkeletalAnimatedNode::currentAnimation() const
{
    return m().currentAnimation();
}

bool SkeletalAnimatedNode::setCurrentAnimation(const std::string& value)
{
    auto& mPrivate = m();

    auto& animations = mPrivate.skeleton()->animations();
    auto animationIter = animations.find(value);

    if (animationIter == animations.end())
    {
        LOG_ERROR << "Skeletal animatioed node \"" << name() << "\" doesn't have an animation \"" << value << "\"";
        return false;
    }

    mPrivate.currentAnimation() = value;
    return true;
}


}
}
