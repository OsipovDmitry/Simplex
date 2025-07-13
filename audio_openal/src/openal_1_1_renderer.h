#ifndef AUDIO_OPENAL_1_1_RENDERER_H
#define AUDIO_OPENAL_1_1_RENDERER_H

#include <unordered_map>
#include <alc.h>
#include <al.h>

#include <utils/noncopyble.h>

#include <core/audiorendererbase.h>

#include <audio_openal/forwarddecl.h>

#define CURRENT_CONTEXT_INFO \
    private: \
        std::shared_ptr<core::audio::RendererBase> m_renderer; \
    public: \
        std::shared_ptr<core::audio::RendererBase> renderer() const { return m_renderer; };

#define SAVE_CURRENT_CONTEXT \
    m_renderer = core::audio::RendererBase::current(); \
    if (!m_renderer) LOG_CRITICAL << "No current context";

#define CHECK_CURRENT_CONTEXT \
    if (m_renderer != core::audio::RendererBase::current()) \
        LOG_CRITICAL << "Resource was created in anotrher context";

#define CHECK_THIS_CONTEXT \
    if (shared_from_this() != core::audio::RendererBase::current()) \
        LOG_CRITICAL << "This context is not current";

#define CHECK_RESOURCE_CONTEXT(resource) \
    if (shared_from_this() != resource->renderer()) \
        LOG_CRITICAL << "Resource was created in anotrher context";

namespace simplex
{
namespace audio_openal
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
    CURRENT_CONTEXT_INFO
public:
    Buffer_1_1();
    ~Buffer_1_1() override;

    ALuint id() const;

    core::audio::BufferFormat format() const override;
    const void *data() const override;
    size_t dataSize() const override;
    uint16_t frequency() const override;
    void setData(core::audio::BufferFormat, const void *data, size_t dataSize, uint32_t frequency) override;

    static std::shared_ptr<Buffer_1_1> create();

private:
    ALuint m_id;
};

class Source_1_1 : public core::audio::ISource
{
    CURRENT_CONTEXT_INFO
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

    static std::shared_ptr<Source_1_1> create();

private:
    ALuint m_id;
    std::shared_ptr<Buffer_1_1> m_buffer;
};

class Listener_1_1 : public core::audio::IListener
{
    CURRENT_CONTEXT_INFO
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

    static std::shared_ptr<Listener_1_1> create();
};

class OpenAL_1_1_Renderer : public core::audio::RendererBase
{
    NONCOPYBLE(OpenAL_1_1_Renderer)
public:
    OpenAL_1_1_Renderer(const std::string&, const std::weak_ptr<OpenALDevice>&);
    ~OpenAL_1_1_Renderer() override;

    std::shared_ptr<core::audio::IAudioDevice> device() override;
    std::shared_ptr<const core::audio::IAudioDevice> device() const override;

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

protected:
    bool doMakeCurrent() override;
    bool doDoneCurrent() override;

private:
    std::weak_ptr<OpenALDevice> m_device;
    ALCcontext *m_context;
    std::shared_ptr<Listener_1_1> m_listener;

};

}
}

#endif // AUDIO_OPENAL_1_1_RENDERER_H
