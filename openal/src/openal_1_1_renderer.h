#ifndef OPENAL_1_1_RENDERER_H
#define OPENAL_1_1_RENDERER_H

#include <unordered_map>
#include <alc.h>
#include <al.h>

#include <utils/noncopyble.h>

#include <core/iaudiorenderer.h>

#include <openal/forwarddecl.h>

namespace simplex
{
namespace openal
{

class Conversions
{
public:
    static core::audio::BufferFormat BitsAndChannels2BufferFormat(uint16_t, uint16_t);
    static ALenum BufferFormat2AL(core::audio::BufferFormat);
    static core::audio::BufferFormat AL2BufferFormat(ALenum);
    static ALenum SourceState2AL(core::audio::SourceState);
    static core::audio::SourceState AL2SourceState(ALenum);
    static ALenum AttenuationModel2AL(core::audio::AttenuationModel);
    static core::audio::AttenuationModel AL2AttenuationModel(ALenum);
};

class Buffer_1_1 : public core::audio::IBuffer
{
public:
    Buffer_1_1();
    ~Buffer_1_1() override;

    ALuint id() const;

    core::audio::BufferFormat format() const override;
    const void *data() const override;
    size_t dataSize() const override;
    uint16_t frequency() const override;
    void setData(core::audio::BufferFormat, const void *data, size_t dataSize, uint32_t frequency) override;

private:
    ALuint m_id;
};

class Source_1_1 : public core::audio::ISource
{
public:
    Source_1_1();
    ~Source_1_1() override;

    ALuint id() const;

    std::shared_ptr<core::audio::IBuffer> buffer() override;
    std::shared_ptr<const core::audio::IBuffer> buffer() const override;
    void setBuffer(const std::shared_ptr<core::audio::IBuffer>&) override;

    float pitch() const override;
    void setPitch(float) override;

    float gain() const override;
    void setGain(float) override;

    float minGain() const override;
    void setMinGain(float) override;

    float maxGain() const override;
    void setMaxGain(float) override;

    float referenceDistance() const override;
    void setReferenceDistance(float) override;

    float maxDistance() const override;
    void setMaxDistance(float) override;

    float rolloffFactor() const override;
    void setRolloffFactor(float) override;

    glm::vec3 position() const override;
    void setPosition(const glm::vec3&) override;

    glm::vec3 velocity() const override;
    void setVelocity(const glm::vec3&) override;

    glm::vec3 direction() const override;
    void setDirection(const glm::vec3&) override;

    float coneInnerAngle() const override;
    void setConeInnerAngle(float) override;

    float coneOuterAngle() const override;
    void setConeOuterAngle(float) override;

    float coneOuterGain() const override;
    void setConeOuterGain(float) override;

    bool looping() const override;
    void setLooping(bool) override;

private:
    ALuint m_id;
    std::shared_ptr<Buffer_1_1> m_buffer;
};

class Listener_1_1 : public core::audio::IListener
{
public:
    Listener_1_1();
    ~Listener_1_1() override;

    float gain() const override;
    void setGain(float) override;

    glm::vec3 position() const override;
    void setPosition(const glm::vec3&) override;

    glm::vec3 velocity() const override;
    void setVelocity(const glm::vec3&) override;

    glm::quat orientation() const override;
    void setOrientation(const glm::quat&) override;
};

class OpenAL_1_1_Renderer : public core::audio::IRenderer
{
    NONCOPYBLE(OpenAL_1_1_Renderer)
public:
    ~OpenAL_1_1_Renderer() override;

    const std::string &name() const override;

    ALCcontext *context() const;

    bool makeCurrent() override;
    bool doneCurrent() override;

    core::audio::AttenuationModel attenautionModel() const override;
    void setAttenautionModel(core::audio::AttenuationModel) const override;

    float dopplerFactor() const override;
    void setDopplerFactor(float) override;

    float speedOfSound() const override;
    void setSpeedOfSound(float) override;

    std::shared_ptr<core::audio::IListener> listener() override;
    std::shared_ptr<const core::audio::IListener> listener() const override;

    std::shared_ptr<core::audio::IBuffer> createBuffer(const std::shared_ptr<utils::Sound>& = nullptr) const override;
    std::shared_ptr<core::audio::ISource> createSource() const override;

    core::audio::SourceState sourceState(const std::shared_ptr<core::audio::ISource>&) const override;
    void playSource(const std::shared_ptr<core::audio::ISource>&) override;
    void pauseSource(const std::shared_ptr<core::audio::ISource>&) override;
    void stopSource(const std::shared_ptr<core::audio::ISource>&) override;
    void rewindSource(const std::shared_ptr<core::audio::ISource>&) override;

    static std::shared_ptr<OpenAL_1_1_Renderer> create(ALCcontext*);
    static std::shared_ptr<OpenAL_1_1_Renderer> instance(ALCcontext* = nullptr);

private:
    OpenAL_1_1_Renderer(ALCcontext*);

    ALCcontext *m_context;
    std::shared_ptr<Listener_1_1> m_listener;

    static std::unordered_map<ALCcontext*, std::weak_ptr<OpenAL_1_1_Renderer>> s_instances;

};

}
}

#endif // OPENAL_1_1_RENDERER_H
