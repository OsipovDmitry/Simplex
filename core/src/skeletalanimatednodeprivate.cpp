#include <core/skeletalanimatednode.h>

#include "skeletalanimatednodeprivate.h"

namespace simplex
{
namespace core
{

SkeletalAnimatedNodePrivate::SkeletalAnimatedNodePrivate(SkeletalAnimatedNode &skeletalAnimatedNode, const std::string &name)
    : NodePrivate(skeletalAnimatedNode, name)
{
}

SkeletalAnimatedNodePrivate::~SkeletalAnimatedNodePrivate() = default;

std::map<std::string, std::shared_ptr<Animation>>& SkeletalAnimatedNodePrivate::animations()
{
    return m_animations;
}

std::shared_ptr<Animation>& SkeletalAnimatedNodePrivate::currentAnimation()
{
    return m_currentAnimation;
}

}
}
