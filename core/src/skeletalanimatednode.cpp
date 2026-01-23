#include <utils/logger.h>

#include <core/animation.h>
#include <core/skeletalanimatednode.h>

#include "skeletalanimatednodeprivate.h"

namespace simplex
{
namespace core
{

SkeletalAnimatedNode::SkeletalAnimatedNode(const std::string &name)
    : Node(std::make_unique<SkeletalAnimatedNodePrivate>(*this, name))
{
    
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

const std::map<std::string, std::shared_ptr<Animation>>& SkeletalAnimatedNode::animations() const
{
    return m().animations();
}

bool SkeletalAnimatedNode::addAnimation(const std::shared_ptr<Animation>& value)
{
    if (!value)
    {
        LOG_ERROR << "Animation can't be nullptr";
        return false;
    }

    auto& animations = m().animations();
    const auto& animationName = value->name();

    if (animations.count(animationName))
    {
        LOG_ERROR << "Skeletal animatioed node \"" << name() << "\" already has an animation \"" << animationName << "\"";
        return false;
    }

    animations.insert({ animationName, value });
    return true;
}

bool SkeletalAnimatedNode::removeAnimation(const std::string& value)
{
    auto& mPrivate = m();

    auto& animations = mPrivate.animations();
    auto animationIter = animations.find(value);

    if (animationIter == animations.end())
    {
        LOG_ERROR << "Skeletal animatioed node \"" << name() << "\" doesn't have an animation \"" << value << "\"";
        return false;
    }

    animations.erase(animationIter);

    if (auto& currentAnimation = mPrivate.currentAnimation(); currentAnimation == animationIter->second)
        currentAnimation = nullptr;

    return true;
}

const std::shared_ptr<Animation> SkeletalAnimatedNode::currentAnimation() const
{
    return m().currentAnimation();
}

bool SkeletalAnimatedNode::setCurrentAnimation(const std::string& value)
{
    auto& mPrivate = m();

    auto& animations = mPrivate.animations();
    auto animationIter = animations.find(value);

    if (animationIter == animations.end())
    {
        LOG_ERROR << "Skeletal animatioed node \"" << name() << "\" doesn't have an animation \"" << value << "\"";
        return false;
    }

    mPrivate.currentAnimation() = animationIter->second;
    return true;
}


}
}
