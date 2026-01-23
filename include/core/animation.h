#ifndef CORE_ANIMATION_H
#define CORE_ANIMATION_H

#include <memory>
#include <vector>

#include <utils/glm/vec3.hpp>
#include <utils/glm/gtx/quaternion.hpp>
#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class AnimationChannelPrivate;
class CORE_SHARED_EXPORT AnimationChannel
{
    NONCOPYBLE(AnimationChannel)
public:
    AnimationChannel();
    ~AnimationChannel();

    uint32_t boneID() const;
    void setBoneID(uint32_t);

    const std::vector<std::pair<uint32_t, float>>& scaleKeys() const;
    void setScaleKeys(const std::vector<std::pair<uint32_t, float>>&);

    const std::vector<std::pair<uint32_t, glm::quat>>& rotationKeys() const;
    void setRotationKeys(std::vector<std::pair<uint32_t, glm::quat>>&);
    
    const std::vector<std::pair<uint32_t, glm::vec3>>& translationKeys() const;
    void setTranslationKeys(std::vector<std::pair<uint32_t, glm::vec3>>&);

private:
    std::unique_ptr<AnimationChannelPrivate> m_;
};

class AnimationPrivate;
class CORE_SHARED_EXPORT Animation : public INamedObject
{
    NONCOPYBLE(Animation)
public:
    Animation(const std::string&);
    ~Animation() override;

    const std::string& name() const override;

    uint32_t duration() const;
    void setDuration(uint32_t);

    uint32_t ticksPerSecond() const;
    void setTicksPerSecond(uint32_t);

    const std::vector<std::shared_ptr<AnimationChannel>>& channels() const;
    void setChannels(std::vector<std::shared_ptr<AnimationChannel>>&);


private:
    std::unique_ptr<AnimationPrivate> m_;
};

}
}

#endif // CORE_ANIMATION_H
