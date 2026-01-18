#ifndef CORE_AUDIORENDERERBASE_H
#define CORE_AUDIORENDERERBASE_H

#include <utils/enumclass.h>
#include <utils/glm/vec3.hpp>
#include <utils/glm/ext/quaternion_float.hpp>
#include <utils/forwarddecl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/irenderer.h>

namespace simplex
{
namespace core
{
namespace audio
{
ENUMCLASS(BufferFormat, uint16_t,
    Mono8,
    Mono16,
    Stereo8,
    Stereo16)

ENUMCLASS(SourceState, uint16_t,
    Initial,
    Playing,
    Paused,
    Stopped)

ENUMCLASS(AttenuationModel, uint16_t,
    None,
    InverseDistance,
    InverseDistanceClamped,
    LinearDistance,
    LinearDistanceClamped,
    ExponentialDistance,
    ExponentialDistanceClamped)

class IBuffer
{
public:
    virtual ~IBuffer() = default;

    virtual BufferFormat format() const = 0;
    virtual const void* data() const = 0;
    virtual size_t dataSize() const = 0;
    virtual uint16_t frequency() const = 0;
    virtual void setData(BufferFormat, const void* data, size_t dataSize, uint32_t frequency) = 0;
};

class ISource
{
public:
    virtual ~ISource() = default;

    virtual std::shared_ptr<core::audio::IBuffer> buffer() = 0;
    virtual std::shared_ptr<const core::audio::IBuffer> buffer() const = 0;
    virtual void setBuffer(const std::shared_ptr<core::audio::IBuffer>&) = 0;

    virtual float pitch() const = 0;
    virtual void setPitch(float) = 0;

    virtual float gain() const = 0;
    virtual void setGain(float) = 0;

    virtual float minGain() const = 0;
    virtual void setMinGain(float) = 0;

    virtual float maxGain() const = 0;
    virtual void setMaxGain(float) = 0;

    virtual float referenceDistance() const = 0;
    virtual void setReferenceDistance(float) = 0;

    virtual float maxDistance() const = 0;
    virtual void setMaxDistance(float) = 0;

    virtual float rolloffFactor() const = 0;
    virtual void setRolloffFactor(float) = 0;

    virtual glm::vec3 position() const = 0;
    virtual void setPosition(const glm::vec3&) = 0;

    virtual glm::vec3 velocity() const = 0;
    virtual void setVelocity(const glm::vec3&) = 0;

    virtual glm::vec3 direction() const = 0;
    virtual void setDirection(const glm::vec3&) = 0;

    virtual float coneInnerAngle() const = 0;
    virtual void setConeInnerAngle(float) = 0;

    virtual float coneOuterAngle() const = 0;
    virtual void setConeOuterAngle(float) = 0;

    virtual float coneOuterGain() const = 0;
    virtual void setConeOuterGain(float) = 0;

    virtual bool looping() const = 0;
    virtual void setLooping(bool) = 0;

};

class IListener
{
public:
    virtual ~IListener() = default;

    virtual float gain() const = 0;
    virtual void setGain(float) = 0;

    virtual glm::vec3 position() const = 0;
    virtual void setPosition(const glm::vec3&) = 0;

    virtual glm::vec3 velocity() const = 0;
    virtual void setVelocity(const glm::vec3&) = 0;

    virtual glm::quat orientation() const = 0;
    virtual void setOrientation(const glm::quat&) = 0;

};

class RendererBasePrivate;
class CORE_SHARED_EXPORT RendererBase : public std::enable_shared_from_this<RendererBase>, public IRenderer
{
public:
    RendererBase(const std::string&);
    ~RendererBase() override;

    const std::string& name() const override;

    bool makeCurrent() override final;
    bool doneCurrent() override final;
    static std::shared_ptr<RendererBase> current();

    virtual std::shared_ptr<IAudioDevice> device() = 0;
    virtual std::shared_ptr<const IAudioDevice> device() const = 0;

    virtual AttenuationModel attenautionModel() const = 0;
    virtual void setAttenautionModel(AttenuationModel) const = 0;

    virtual float dopplerFactor() const = 0;
    virtual void setDopplerFactor(float) = 0;

    virtual float speedOfSound() const = 0;
    virtual void setSpeedOfSound(float) = 0;

    virtual std::shared_ptr<IListener> listener() = 0;
    virtual std::shared_ptr<const IListener> listener() const = 0;

    virtual std::shared_ptr<IBuffer> createBuffer(const std::shared_ptr<utils::Sound> & = nullptr) const = 0;
    virtual std::shared_ptr<ISource> createSource() const = 0;

    virtual SourceState sourceState(const std::shared_ptr<ISource>&) const = 0;
    virtual void playSource(const std::shared_ptr<ISource>&) = 0;
    virtual void pauseSource(const std::shared_ptr<ISource>&) = 0;
    virtual void stopSource(const std::shared_ptr<ISource>&) = 0;
    virtual void rewindSource(const std::shared_ptr<ISource>&) = 0;

protected:
    virtual bool doMakeCurrent() = 0;
    virtual bool doDoneCurrent() = 0;

    std::unique_ptr<RendererBasePrivate> m_;
};

}
}
}

#endif // CORE_AUDIORENDERERBASE_H
