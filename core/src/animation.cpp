#include <core/animation.h>

#include "animationprivate.h"

namespace simplex
{
namespace core
{

AnimationChannel::AnimationChannel()
    : m_(std::make_unique<AnimationChannelPrivate>())
{
}

AnimationChannel::~AnimationChannel() = default;

uint32_t AnimationChannel::boneID() const
{
    return m_->boneID();
}

void AnimationChannel::setBoneID(uint32_t value)
{
    m_->boneID() = value;
}

const std::vector<std::pair<uint32_t, float>>& AnimationChannel::scaleKeys() const
{
    return m_->scaleKeys();
}

void AnimationChannel::setScaleKeys(const std::vector<std::pair<uint32_t, float>>& value)
{
    m_->scaleKeys() = value;
}

const std::vector<std::pair<uint32_t, glm::quat>>& AnimationChannel::rotationKeys() const
{
    return m_->rotationKeys();
}

void AnimationChannel::setRotationKeys(std::vector<std::pair<uint32_t, glm::quat>>& value)
{
    m_->rotationKeys() = value;
}

const std::vector<std::pair<uint32_t, glm::vec3>>& AnimationChannel::translationKeys() const
{
    return m_->translationKeys();
}

void AnimationChannel::setTranslationKeys(std::vector<std::pair<uint32_t, glm::vec3>>& value)
{
    m_->translationKeys() = value;
}

Animation::Animation(const std::string& name)
    : m_(std::make_unique<AnimationPrivate>(name))
{
}

Animation::~Animation() = default;

const std::string& Animation::name() const
{
    return m_->name();
}

uint32_t Animation::duration() const
{
    return m_->duration();
}

void Animation::setDuration(uint32_t value)
{
    m_->duration() = value;
}

uint32_t Animation::ticksPerSecond() const
{
    return m_->ticksPerSecond();
}

void Animation::setTicksPerSecond(uint32_t value)
{
    m_->ticksPerSecond() = value;
}

const std::vector<std::shared_ptr<AnimationChannel>>& Animation::channels() const
{
    return m_->channels();
}

void Animation::setChannels(std::vector<std::shared_ptr<AnimationChannel>>& value)
{
    m_->channels() = value;
}

}
}
