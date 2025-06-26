#ifndef CORE_ANIMATION_H
#define CORE_ANIMATION_H

#include <memory>
#include <utility>
#include <vector>

#include <utils/glm/vec4.hpp>
#include <utils/enumclass.h>
#include <utils/noncopyble.h>
#include <utils/transform.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class AnimationPrivate;
class CORE_SHARED_EXPORT Animation : public INamedObject
{
    NONCOPYBLE(Animation)
public:
    struct Channel
    {
        NONCOPYBLE(Channel)

        ENUMCLASS(InterpolationType, uint16_t, Step, Linear, CubicSpline)
        ENUMCLASS(TargetTypeId, uint16_t, Rotation, Translation, Scale)
        union TargetType { utils::Transform::QuatType rotation; utils::Transform::PointType translation; utils::Transform::value_type scale; };

        std::weak_ptr<Node> targetNode;
        InterpolationType interpolationType;
        TargetTypeId targetTypeId;
        std::vector<float> timestamps;
        std::vector<TargetType> values;
        std::vector<std::pair<TargetType, TargetType>> tangents;

        Channel() {}
    };


    Animation(const std::string&);
    ~Animation() override;

    const std::string &name() const override;

    const std::vector<std::shared_ptr<Channel>> &channels() const;
    std::shared_ptr<Channel> addChannel();
    void removeChannel(const std::shared_ptr<Channel>&);

private:
    std::unique_ptr<AnimationPrivate> m_;
};

class SkeletonPrivate;
class CORE_SHARED_EXPORT Skeleton
{
    NONCOPYBLE(Skeleton)
public:
    struct Bone
    {
        NONCOPYBLE(Bone)

        std::weak_ptr<Node> targetNode;
        glm::mat4x4 inverseBindMatrix;

        Bone() : inverseBindMatrix(1.f) {}
    };

    Skeleton();
    ~Skeleton();

    const std::vector<std::shared_ptr<Bone>> &bones() const;
    void setBones(const std::vector<std::shared_ptr<Bone>>&);

    graphics::PConstBufferRange bonesBuffer() const;

private:
    std::unique_ptr<SkeletonPrivate> m_;
};


}
}

#endif // CORE_ANIMATION_H
