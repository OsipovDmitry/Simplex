#include <core/skeletalanimation.h>

#include "skeletalanimationprivate.h"


namespace simplex
{
namespace core
{

Animation::Animation(uint32_t duration, uint32_t ticksPerSecond, const std::vector<AnimationChannel>& channels)
    : m_(std::make_unique<AnimationPrivate>(duration, ticksPerSecond, channels))
{
}

Animation::~Animation() = default;

uint32_t Animation::duration() const
{
    return m_->duration();
}

uint32_t Animation::ticksPerSecond() const
{
    return m_->ticksPerSecond();
}

const std::vector<AnimationChannel>& Animation::channels() const
{
    return m_->channels();
}

Skeleton::Skeleton(
    const std::vector<Bone>& bones,
    uint32_t rootBoneID,
    const std::map<std::string, std::shared_ptr<Animation>>& animations)
    : m_(std::make_unique<SkeletonPrivate>(bones, rootBoneID, animations))
{
}
Skeleton::~Skeleton() = default;

const std::vector<Bone>& Skeleton::bones() const
{
    return m_->bones();
}

uint32_t Skeleton::rootBoneID() const
{
    return m_->rootBoneID();
}

const std::map<std::string, std::shared_ptr<Animation>>& Skeleton::animations() const
{
    return m_->animations();
}

}
}
