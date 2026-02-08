#include "scenedata.h"
#include "skeletalanimationprivate.h"

namespace simplex
{
namespace core
{

AnimationPrivate::AnimationPrivate(
    uint32_t duration,
    uint32_t ticksPerSecond,
    const std::vector<AnimationChannel>& channels)
    : m_duration(duration)
    , m_ticksPerSecond(ticksPerSecond)
    , m_channels(channels)
{
}

AnimationPrivate::~AnimationPrivate() = default;

uint32_t& AnimationPrivate::duration()
{
    return m_duration;
}

uint32_t& AnimationPrivate::ticksPerSecond()
{
    return m_ticksPerSecond;
}

std::vector<AnimationChannel>& AnimationPrivate::channels()
{
    return m_channels;
}

SkeletonPrivate::SkeletonPrivate(
    const std::vector<Bone>& bones,
    uint32_t rootBoneID,
    const std::map<std::string, std::shared_ptr<Animation>>& animations)
    : m_bones(bones)
    , m_rootBoneID(rootBoneID)
    , m_animations(animations)
{
}

SkeletonPrivate::~SkeletonPrivate() = default;

std::vector<Bone>& SkeletonPrivate::bones()
{
    return m_bones;
}

uint32_t& SkeletonPrivate::rootBoneID()
{
    return m_rootBoneID;
}

std::map<std::string, std::shared_ptr<Animation>>& SkeletonPrivate::animations()
{
    return m_animations;
}

std::set<std::shared_ptr<SkeletonHandler>>& SkeletonPrivate::handlers()
{
    return m_handlers;
}

void SkeletonPrivate::onChanged()
{
    for (auto& handler : m_handlers)
        if (auto sceneData = handler->sceneData().lock())
            sceneData->onSkeletonChanged(handler->ID(), m_bones, m_rootBoneID, m_animations);
}

}
}
