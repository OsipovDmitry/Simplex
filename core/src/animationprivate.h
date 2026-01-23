#ifndef CORE_ANIMATIONPRIVATE_H
#define CORE_ANIMATIONPRIVATE_H

#include "nodeprivate.h"

namespace simplex
{
namespace core
{

class AnimationChannelPrivate
{
public:
    AnimationChannelPrivate();
    ~AnimationChannelPrivate();

    uint32_t& boneID();
    std::vector<std::pair<uint32_t, float>>& scaleKeys();
    std::vector<std::pair<uint32_t, glm::quat>>& rotationKeys();
    std::vector<std::pair<uint32_t, glm::vec3>>& translationKeys();

private:
    uint32_t m_boneID;
    std::vector<std::pair<uint32_t, float>> m_scaleKeys;
    std::vector<std::pair<uint32_t, glm::quat>> m_rotationKeys;
    std::vector<std::pair<uint32_t, glm::vec3>> m_translationKeys;

};

class AnimationPrivate
{
public:
    AnimationPrivate(const std::string&);
    ~AnimationPrivate();

    std::string& name();
    uint32_t& duration();
    uint32_t& ticksPerSecond();
    std::vector<std::shared_ptr<AnimationChannel>>& channels();

private:
    std::string m_name;
    uint32_t m_duration;
    uint32_t m_ticksPerSecond;
    std::vector<std::shared_ptr<AnimationChannel>> m_channels;
};

}
}

#endif // CORE_ANIMATIONPRIVATE_H
