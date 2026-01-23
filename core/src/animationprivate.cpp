#include "animationprivate.h"

namespace simplex
{
namespace core
{

AnimationChannelPrivate::AnimationChannelPrivate()
    : m_boneID(0xFFFFFFFFu)
{
}

AnimationChannelPrivate::~AnimationChannelPrivate() = default;

uint32_t& AnimationChannelPrivate::boneID()
{
    return m_boneID;
}

std::vector<std::pair<uint32_t, float>>& AnimationChannelPrivate::scaleKeys()
{
    return m_scaleKeys;
}

std::vector<std::pair<uint32_t, glm::quat>>& AnimationChannelPrivate::rotationKeys()
{
    return m_rotationKeys;
}

std::vector<std::pair<uint32_t, glm::vec3>>& AnimationChannelPrivate::translationKeys()
{
    return m_translationKeys;
}


AnimationPrivate::AnimationPrivate(const std::string& name)
    : m_name(name)
    , m_duration(0u)
    , m_ticksPerSecond(25u)
{
}

AnimationPrivate::~AnimationPrivate() = default;

std::string& AnimationPrivate::name()
{
    return m_name;
}

uint32_t& AnimationPrivate::duration()
{
    return m_duration;
}

uint32_t& AnimationPrivate::ticksPerSecond()
{
    return m_ticksPerSecond;
}

std::vector<std::shared_ptr<AnimationChannel>>& AnimationPrivate::channels()
{
    return m_channels;
}



}
}
