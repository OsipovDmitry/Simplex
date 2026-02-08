#ifndef CORE_SKELETALANIMATIONPRIVATE_H
#define CORE_SKELETALANIMATIONPRIVATE_H

#include <map>
#include <set>

#include <core/skeletalanimation.h>

namespace simplex
{
namespace core
{

class SkeletonHandler;

class AnimationPrivate
{
public:
    AnimationPrivate(uint32_t duration, uint32_t ticksPerSecond, const std::vector<AnimationChannel>& channels);
    ~AnimationPrivate();

    uint32_t& duration();
    uint32_t& ticksPerSecond();
    std::vector<AnimationChannel>& channels();

private:
    uint32_t m_duration;
    uint32_t m_ticksPerSecond;
    std::vector<AnimationChannel> m_channels;
};

class SkeletonPrivate
{
public:
    SkeletonPrivate(
        const std::vector<Bone>&,
        uint32_t rootBoneID,
        const std::map<std::string, std::shared_ptr<Animation>>&);
    ~SkeletonPrivate();

    std::vector<Bone>& bones();
    uint32_t& rootBoneID();
    std::map<std::string, std::shared_ptr<Animation>>& animations();

    std::set<std::shared_ptr<SkeletonHandler>>& handlers();
    void onChanged();

private:
    std::vector<Bone> m_bones;
    uint32_t m_rootBoneID;
    std::map<std::string, std::shared_ptr<Animation>> m_animations;
    std::set<std::shared_ptr<SkeletonHandler>> m_handlers;
};

}
}

#endif // CORE_SKELETALANIMATIONPRIVATE_H
