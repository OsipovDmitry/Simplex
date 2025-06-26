#ifndef CORE_ANIMATIONPRIVATE_H
#define CORE_ANIMATIONPRIVATE_H

#include <string>
#include <vector>

#include <core/animation.h>

namespace simplex
{
namespace core
{

class AnimationPrivate
{
public:
    AnimationPrivate(const std::string &name);

    const std::string &name() const;

    std::vector<std::shared_ptr<Animation::Channel>> &channels();

private:
    std::string m_name;

    std::vector<std::shared_ptr<Animation::Channel>> m_channels;
};

class SkeletonPrivate
{
public:
    SkeletonPrivate();

    std::vector<std::shared_ptr<Skeleton::Bone>> &bones();

    graphics::PBufferRange &bonesBuffer();

    void update();

private:
    glm::mat4x4 m_inverseBindMatrix;

    std::vector<std::shared_ptr<Skeleton::Bone>> m_bones;
    graphics::PBufferRange m_bonesBuffer;
};

}
}

#endif // CORE_ANIMATIONPRIVATE_H
