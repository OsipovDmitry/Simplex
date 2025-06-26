#include <core/igraphicsrenderer.h>

#include "animationprivate.h"

namespace simplex
{
namespace core
{

AnimationPrivate::AnimationPrivate(const std::string &name)
    : m_name(name)
{
}

const std::string &AnimationPrivate::name() const
{
    return m_name;
}

std::vector<std::shared_ptr<Animation::Channel>> &AnimationPrivate::channels()
{
    return m_channels;
}

SkeletonPrivate::SkeletonPrivate()
{
}

std::vector<std::shared_ptr<Skeleton::Bone>> &SkeletonPrivate::bones()
{
    return m_bones;
}

graphics::PBufferRange &SkeletonPrivate::bonesBuffer()
{
    return m_bonesBuffer;
}

void SkeletonPrivate::update()
{
    auto bufferData = m_bonesBuffer->buffer()->map(graphics::IBuffer::MapAccess::WriteOnly);
    auto *p = reinterpret_cast<decltype(Skeleton::Bone::inverseBindMatrix)*>(bufferData.get());
    for (size_t i = 0u; i < m_bones.size(); ++i)
        p[i] = m_bones[i]->inverseBindMatrix;

}

}
}
