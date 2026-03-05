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

Skeleton::Skeleton(const std::vector<Bone>& bones, const std::map<std::string, std::shared_ptr<Animation>>& animations)
    : m_(std::make_unique<SkeletonPrivate>(bones, animations))
{
    auto& mPrivate = m();
    auto& mPrivateBones = mPrivate.bones();

    std::vector<Bone::ID> rootBonesIDs;
    for (size_t boneID = 0u; boneID < mPrivateBones.size(); ++boneID)
    {
        if (mPrivateBones[boneID].parentID == Bone::UndefinedID) rootBonesIDs.push_back(boneID);
    }

    auto rootBoneID = Bone::UndefinedID;

    if (rootBonesIDs.size() > 1u)
    {
        rootBoneID = static_cast<Bone::ID>(mPrivateBones.size());
        mPrivateBones.push_back({utils::Transform(), utils::Transform(), rootBoneID, rootBonesIDs});

        for (auto ID : rootBonesIDs)
            mPrivateBones[ID].parentID = rootBoneID;
    }
    else if (rootBonesIDs.size() == 1u)
    {
        rootBoneID = rootBonesIDs.front();
    }

    mPrivate.rootBoneID() = rootBoneID;
}

Skeleton::~Skeleton() = default;

const std::vector<Bone>& Skeleton::bones() const
{
    return m_->bones();
}

Bone::ID Skeleton::rootBoneID() const
{
    return m_->rootBoneID();
}

const std::map<std::string, std::shared_ptr<Animation>>& Skeleton::animations() const
{
    return m_->animations();
}

} // namespace core
} // namespace simplex
