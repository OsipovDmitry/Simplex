#ifndef CORE_SKELETALANIMATION_H
#define CORE_SKELETALANIMATION_H

#include <map>
#include <memory>
#include <vector>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>
#include <utils/transform.h>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

struct AnimationChannel
{
public:
    uint32_t boneID;
    std::vector<std::pair<uint32_t, float>> scaleKeys;
    std::vector<std::pair<uint32_t, glm::quat>> rotationKeys;
    std::vector<std::pair<uint32_t, glm::vec3>> translationKeys;
};

class AnimationPrivate;
class CORE_SHARED_EXPORT Animation
{
    NONCOPYBLE(Animation)
public:
    Animation(uint32_t duration, uint32_t ticksPerSecond, const std::vector<AnimationChannel>& channels);
    ~Animation();

    uint32_t duration() const;
    uint32_t ticksPerSecond() const;
    const std::vector<AnimationChannel>& channels() const;

private:
    std::unique_ptr<AnimationPrivate> m_;
};

struct Bone
{
    utils::Transform offsetTransform;
    utils::Transform transform;
    uint32_t parentID;
    std::vector<uint32_t> childrenIDs;
};

class SkeletonPrivate;
class CORE_SHARED_EXPORT Skeleton
{
    NONCOPYBLE(Skeleton)
    PRIVATE_IMPL(Skeleton)
public:
    Skeleton(
        const std::vector<Bone>& bones,
        uint32_t rootBoneID,
        const std::map<std::string, std::shared_ptr<Animation>>& animations);

    ~Skeleton();

    const std::vector<Bone>& bones() const;
    uint32_t rootBoneID() const;
    const std::map<std::string, std::shared_ptr<Animation>>& animations() const;

private:
    std::unique_ptr<SkeletonPrivate> m_;

};

}
}

#endif // CORE_SKELETALANIMATION_H
