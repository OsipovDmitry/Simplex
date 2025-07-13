#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/logger.h>
#include <utils/sound.h>

#include <audio_openal/openaldevice.h>

#include "openaldeviceprivate.h"
#include "openal_1_1_renderer.h"

namespace simplex
{
namespace audio_openal
{

core::audio::BufferFormat Conversions::BitsAndChannels2BufferFormat(uint16_t bits, uint16_t channels)
{
    auto result = core::audio::BufferFormat::Count;

    switch (channels)
    {
    case 1u:
    {
        switch (bits)
        {
        case 8u: result = core::audio::BufferFormat::Mono8; break;
        case 16u: result = core::audio::BufferFormat::Mono16; break;
        default: break;
        }
        break;
    }
    case 2u:
    {
        switch (bits)
        {
        case 8u: result = core::audio::BufferFormat::Stereo8; break;
        case 16u: result = core::audio::BufferFormat::Stereo16; break;
        default: break;
        }
        break;
    }
    default: break;
    }

    return result;
}

ALenum Conversions::BufferFormat2AL(core::audio::BufferFormat value)
{
    static std::array<ALenum, core::audio::numElementsBufferFormat()> s_table {
        AL_FORMAT_MONO8,
        AL_FORMAT_MONO16,
        AL_FORMAT_STEREO8,
        AL_FORMAT_STEREO16
    };

    return s_table[core::audio::castFromBufferFormat(value)];
}

core::audio::BufferFormat Conversions::AL2BufferFormat(ALenum value)
{
    static std::unordered_map<ALenum, core::audio::BufferFormat> s_table {
        { AL_FORMAT_MONO8, core::audio::BufferFormat::Mono8 },
        { AL_FORMAT_MONO16, core::audio::BufferFormat::Mono16 },
        { AL_FORMAT_STEREO8, core::audio::BufferFormat::Stereo8 },
        { AL_FORMAT_STEREO16, core::audio::BufferFormat::Stereo16 },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::audio::BufferFormat::Count : it->second;
}

ALenum Conversions::SourceState2AL(core::audio::SourceState value)
{
    static std::array<ALenum, core::audio::numElementsSourceState()> s_table {
        AL_INITIAL,
        AL_PLAYING,
        AL_PAUSED,
        AL_STOPPED
    };

    return s_table[core::audio::castFromSourceState(value)];
}

core::audio::SourceState Conversions::AL2SourceState(ALenum value)
{
    static std::unordered_map<ALenum, core::audio::SourceState> s_table {
        { AL_INITIAL, core::audio::SourceState::Initial },
        { AL_PLAYING, core::audio::SourceState::Playing },
        { AL_PAUSED, core::audio::SourceState::Paused },
        { AL_STOPPED, core::audio::SourceState::Stopped },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::audio::SourceState::Count : it->second;
}

ALenum Conversions::AttenuationModel2AL(core::audio::AttenuationModel value)
{
    static std::array<ALenum, core::audio::numElementsAttenuationModel()> s_table {
        AL_NONE,
        AL_INVERSE_DISTANCE,
        AL_INVERSE_DISTANCE_CLAMPED,
        AL_LINEAR_DISTANCE,
        AL_LINEAR_DISTANCE_CLAMPED,
        AL_EXPONENT_DISTANCE,
        AL_EXPONENT_DISTANCE_CLAMPED
    };

    return s_table[core::audio::castFromAttenuationModel(value)];
}

core::audio::AttenuationModel Conversions::AL2AttenuationModel(ALenum value)
{
    static std::unordered_map<ALenum, core::audio::AttenuationModel> s_table {
        { AL_NONE, core::audio::AttenuationModel::None },
        { AL_INVERSE_DISTANCE, core::audio::AttenuationModel::InverseDistance },
        { AL_INVERSE_DISTANCE_CLAMPED, core::audio::AttenuationModel::InverseDistanceClamped },
        { AL_LINEAR_DISTANCE, core::audio::AttenuationModel::LinearDistance },
        { AL_LINEAR_DISTANCE_CLAMPED, core::audio::AttenuationModel::LinearDistanceClamped },
        { AL_EXPONENT_DISTANCE, core::audio::AttenuationModel::ExponentialDistance },
        { AL_EXPONENT_DISTANCE_CLAMPED, core::audio::AttenuationModel::ExponentialDistanceClamped },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::audio::AttenuationModel::Count : it->second;
}

Buffer_1_1::Buffer_1_1()
{
    SAVE_CURRENT_CONTEXT
    alGenBuffers(1u, &m_id);
}

Buffer_1_1::~Buffer_1_1()
{
    CHECK_CURRENT_CONTEXT
    alDeleteBuffers(1u, &m_id);
}

ALuint Buffer_1_1::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

core::audio::BufferFormat Buffer_1_1::format() const
{
    CHECK_CURRENT_CONTEXT
    ALint bits, channels;
    alGetBufferiv(m_id, AL_BITS, &bits);
    alGetBufferiv(m_id, AL_CHANNELS, &channels);
    return Conversions::BitsAndChannels2BufferFormat(static_cast<uint16_t>(bits), static_cast<uint16_t>(channels));
}

const void *Buffer_1_1::data() const
{
    CHECK_CURRENT_CONTEXT
    // AL_DATA is not longer supported
    return nullptr;
}

size_t Buffer_1_1::dataSize() const
{
    CHECK_CURRENT_CONTEXT
    ALint result;
    alGetBufferiv(m_id, AL_SIZE, &result);
    return static_cast<size_t>(result);
}

uint16_t Buffer_1_1::frequency() const
{
    CHECK_CURRENT_CONTEXT
    ALint result;
    alGetBufferiv(m_id, AL_FREQUENCY, &result);
    return static_cast<uint16_t>(result);
}

void Buffer_1_1::setData(core::audio::BufferFormat format, const void *data, size_t dataSize, uint32_t frequency)
{
    CHECK_CURRENT_CONTEXT
    alBufferData(m_id,
                 Conversions::BufferFormat2AL(format),
                 data,
                 static_cast<ALsizei>(dataSize),
                 static_cast<ALsizei>(frequency));
}

std::shared_ptr<Buffer_1_1> Buffer_1_1::create()
{
    return std::make_shared<Buffer_1_1>();
}

Source_1_1::Source_1_1()
{
    SAVE_CURRENT_CONTEXT
    alGenSources(1u, &m_id);
}

Source_1_1::~Source_1_1()
{
    CHECK_CURRENT_CONTEXT
    alDeleteSources(1u, &m_id);
}

ALuint Source_1_1::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

std::shared_ptr<core::audio::IBuffer> Source_1_1::buffer()
{
    CHECK_CURRENT_CONTEXT
    return m_buffer;
}

std::shared_ptr<const core::audio::IBuffer> Source_1_1::buffer() const
{
    CHECK_CURRENT_CONTEXT
    return const_cast<Source_1_1*>(this)->buffer();
}

void Source_1_1::setBuffer(const std::shared_ptr<core::audio::IBuffer> &value)
{
    CHECK_CURRENT_CONTEXT
    m_buffer = std::dynamic_pointer_cast<Buffer_1_1>(value);

    ALint bufferId = 0u;
    if (m_buffer)
        bufferId = static_cast<ALint>(m_buffer->id());

    alSourcei(m_id, AL_BUFFER, bufferId);
}

float Source_1_1::pitch() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_PITCH, &result);
    return result;
}

void Source_1_1::setPitch(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value <= 0.f)
        LOG_CRITICAL << "Pitch can't be less or equal than 0.0";

    alSourcef(m_id, AL_PITCH, value);
}

float Source_1_1::gain() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_GAIN, &result);
    return result;
}

void Source_1_1::setGain(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f)
        LOG_CRITICAL << "Gain can't be less than 0.0";

    alSourcef(m_id, AL_GAIN, value);
}

float Source_1_1::minGain() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_MIN_GAIN, &result);
    return result;
}

void Source_1_1::setMinGain(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f || value > 1.f)
        LOG_CRITICAL << "Min gain must be in [0.0..1.0]";

    alSourcef(m_id, AL_MIN_GAIN, value);
}

float Source_1_1::maxGain() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_MAX_GAIN, &result);
    return result;
}

void Source_1_1::setMaxGain(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f || value > 1.f)
        LOG_CRITICAL << "Max gain must be in [0.0..1.0]";

    alSourcef(m_id, AL_MAX_GAIN, value);
}

float Source_1_1::referenceDistance() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_REFERENCE_DISTANCE, &result);
    return result;
}

void Source_1_1::setReferenceDistance(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f)
        LOG_CRITICAL << "Reference Distance can't be less than 0.0";

    alSourcef(m_id, AL_REFERENCE_DISTANCE, value);
}

float Source_1_1::maxDistance() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_MAX_DISTANCE, &result);
    return result;
}

void Source_1_1::setMaxDistance(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f)
        LOG_CRITICAL << "Max Distance can't be less than 0.0";

    alSourcef(m_id, AL_MAX_DISTANCE, value);
}

float Source_1_1::rolloffFactor() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_ROLLOFF_FACTOR, &result);
    return result;
}

void Source_1_1::setRolloffFactor(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f)
        LOG_CRITICAL << "Rolloff Factor can't be less than 0.0";

    alSourcef(m_id, AL_ROLLOFF_FACTOR, value);
}

glm::vec3 Source_1_1::position() const
{
    CHECK_CURRENT_CONTEXT
    glm::vec3 result;
    alGetSourcefv(m_id, AL_POSITION, glm::value_ptr(result));
    return result;
}

void Source_1_1::setPosition(const glm::vec3 &value)
{
    CHECK_CURRENT_CONTEXT
    alSourcefv(m_id, AL_POSITION, glm::value_ptr(value));
}

glm::vec3 Source_1_1::velocity() const
{
    CHECK_CURRENT_CONTEXT
    glm::vec3 result;
    alGetSourcefv(m_id, AL_VELOCITY, glm::value_ptr(result));
    return result;
}

void Source_1_1::setVelocity(const glm::vec3 &value)
{
    CHECK_CURRENT_CONTEXT
    alSourcefv(m_id, AL_VELOCITY, glm::value_ptr(value));
}

glm::vec3 Source_1_1::direction() const
{
    CHECK_CURRENT_CONTEXT
    glm::vec3 result;
    alGetSourcefv(m_id, AL_DIRECTION, glm::value_ptr(result));
    return result;
}

void Source_1_1::setDirection(const glm::vec3 &value)
{
    CHECK_CURRENT_CONTEXT
    alSourcefv(m_id, AL_DIRECTION, glm::value_ptr(value));
}

float Source_1_1::coneInnerAngle() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_CONE_INNER_ANGLE, &result);
    return result;
}

void Source_1_1::setConeInnerAngle(float value)
{
    CHECK_CURRENT_CONTEXT
    alSourcef(m_id, AL_CONE_INNER_ANGLE, value);
}

float Source_1_1::coneOuterAngle() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_CONE_OUTER_ANGLE, &result);
    return result;
}

void Source_1_1::setConeOuterAngle(float value)
{
    CHECK_CURRENT_CONTEXT
    alSourcef(m_id, AL_CONE_OUTER_ANGLE, value);
}

float Source_1_1::coneOuterGain() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetSourcef(m_id, AL_CONE_OUTER_GAIN, &result);
    return result;
}

void Source_1_1::setConeOuterGain(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f)
        LOG_CRITICAL << "Cone Outer Gain can't be less than 0.0";

    alSourcef(m_id, AL_CONE_OUTER_GAIN, value);
}

bool Source_1_1::looping() const
{
    CHECK_CURRENT_CONTEXT
    ALint result;
    alGetSourcei(m_id, AL_LOOPING, &result);
    return result != AL_FALSE;
}

void Source_1_1::setLooping(bool value)
{
    CHECK_CURRENT_CONTEXT
    alSourcei(m_id, AL_LOOPING, value ? AL_TRUE : AL_FALSE);
}

std::shared_ptr<Source_1_1> Source_1_1::create()
{
    return std::make_shared<Source_1_1>();
}

Listener_1_1::Listener_1_1()
{
    SAVE_CURRENT_CONTEXT
}

Listener_1_1::~Listener_1_1()
{
    CHECK_CURRENT_CONTEXT
}

float Listener_1_1::gain() const
{
    CHECK_CURRENT_CONTEXT
    float result;
    alGetListenerf(AL_GAIN, &result);
    return result;
}

void Listener_1_1::setGain(float value)
{
    CHECK_CURRENT_CONTEXT
    if (value < 0.f)
        LOG_CRITICAL << "Gain can't be less than 0.0";

    alListenerf(AL_GAIN, value);
}

glm::vec3 Listener_1_1::position() const
{
    CHECK_CURRENT_CONTEXT
    glm::vec3 result;
    alGetListenerfv(AL_POSITION, glm::value_ptr(result));
    return result;
}

void Listener_1_1::setPosition(const glm::vec3 &value)
{
    CHECK_CURRENT_CONTEXT
    alListenerfv(AL_POSITION, glm::value_ptr(value));
}

glm::vec3 Listener_1_1::velocity() const
{
    CHECK_CURRENT_CONTEXT
    glm::vec3 result;
    alGetListenerfv(AL_VELOCITY, glm::value_ptr(result));
    return result;
}

void Listener_1_1::setVelocity(const glm::vec3 &value)
{
    CHECK_CURRENT_CONTEXT
    alListenerfv(AL_VELOCITY, glm::value_ptr(value));
}

glm::quat Listener_1_1::orientation() const
{
    CHECK_CURRENT_CONTEXT
    std::array<float, 6u> ori;
    alGetListenerfv(AL_ORIENTATION, ori.data());
    return glm::quatLookAt(*reinterpret_cast<glm::vec3*>(ori.data() + 0u), *reinterpret_cast<glm::vec3*>(ori.data() + 3u));
}

void Listener_1_1::setOrientation(const glm::quat &value)
{
    CHECK_CURRENT_CONTEXT
    std::array<float, 6u> ori;
    *reinterpret_cast<glm::vec3*>(ori.data() + 0u) = value * glm::vec3(0.f, 0.f, -1.f);
    *reinterpret_cast<glm::vec3*>(ori.data() + 3u) = value * glm::vec3(0.f, 1.f, 0.f);
    alListenerfv(AL_ORIENTATION, ori.data());
}

std::shared_ptr<Listener_1_1> Listener_1_1::create()
{
    return std::make_shared<Listener_1_1>();
}

OpenAL_1_1_Renderer::OpenAL_1_1_Renderer(const std::string &name, const std::weak_ptr<OpenALDevice> &device)
    : core::audio::RendererBase(name)
    , m_device(device)
{
    if (m_device.expired())
        LOG_CRITICAL << "OpenALDevice can't be nullptr";

    auto openALDevice = m_device.lock();
    if (!openALDevice)
        LOG_CRITICAL << "OpenALDevice can't be nullptr";

    m_context = alcCreateContext(openALDevice->m().device(), nullptr);
    if (!m_context)
        LOG_CRITICAL << "Failed to create OpenAL context";

    m_listener = Listener_1_1::create();

    LOG_INFO << "Audio renderer \"" << OpenAL_1_1_Renderer::name() << "\" has been created";
}

OpenAL_1_1_Renderer::~OpenAL_1_1_Renderer()
{
    alcDestroyContext(m_context);

    LOG_INFO << "Audio renderer \"" << OpenAL_1_1_Renderer::name() << "\" has been destroyed";
}

std::shared_ptr<core::audio::IAudioDevice> OpenAL_1_1_Renderer::device()
{
    // No CHECK_THIS_CONTEXT because of recurion calls
    return m_device.expired() ? nullptr : m_device.lock();
}

std::shared_ptr<const core::audio::IAudioDevice> OpenAL_1_1_Renderer::device() const
{
    // No CHECK_THIS_CONTEXT because of recurion calls
    return const_cast<OpenAL_1_1_Renderer*>(this)->device();
}

core::audio::AttenuationModel OpenAL_1_1_Renderer::attenautionModel() const
{
    CHECK_THIS_CONTEXT
    ALint result;
    alGetIntegerv(AL_DISTANCE_MODEL, &result);
    return Conversions::AL2AttenuationModel(result);
}

void OpenAL_1_1_Renderer::setAttenautionModel(core::audio::AttenuationModel value) const
{
    CHECK_THIS_CONTEXT
    alDistanceModel(Conversions::AttenuationModel2AL(value));
}

float OpenAL_1_1_Renderer::dopplerFactor() const
{
    CHECK_THIS_CONTEXT
    ALfloat result;
    alGetFloatv(AL_DOPPLER_FACTOR, &result);
    return result;
}

void OpenAL_1_1_Renderer::setDopplerFactor(float value)
{
    CHECK_THIS_CONTEXT
    alDopplerFactor(value);
}

float OpenAL_1_1_Renderer::speedOfSound() const
{
    CHECK_THIS_CONTEXT
    ALfloat result;
    alGetFloatv(AL_SPEED_OF_SOUND, &result);
    return result;
}

void OpenAL_1_1_Renderer::setSpeedOfSound(float value)
{
    CHECK_THIS_CONTEXT
    alSpeedOfSound(value);
}

std::shared_ptr<core::audio::IListener> OpenAL_1_1_Renderer::listener()
{
    CHECK_THIS_CONTEXT
    return m_listener;
}

std::shared_ptr<const core::audio::IListener> OpenAL_1_1_Renderer::listener() const
{
    CHECK_THIS_CONTEXT
    return const_cast<OpenAL_1_1_Renderer*>(this)->listener();
}

std::shared_ptr<core::audio::IBuffer> OpenAL_1_1_Renderer::createBuffer(const std::shared_ptr<utils::Sound> &sound) const
{
    CHECK_THIS_CONTEXT
    auto result = Buffer_1_1::create();
    if (sound)
    {
        result->setData(Conversions::BitsAndChannels2BufferFormat(sound->bitsPerSample(), sound->numChannels()),
                        sound->data(),
                        sound->numFrames() * sound->numChannels() * (sound->bitsPerSample() / 8u),
                        sound->sampleRate());
    }

    return result;
}

std::shared_ptr<core::audio::ISource> OpenAL_1_1_Renderer::createSource() const
{
    CHECK_THIS_CONTEXT
    return Source_1_1::create();
}

core::audio::SourceState OpenAL_1_1_Renderer::sourceState(const std::shared_ptr<core::audio::ISource> &source) const
{
    CHECK_THIS_CONTEXT
    auto source_1_1 = std::dynamic_pointer_cast<Source_1_1>(source);
    if (!source_1_1)
        LOG_CRITICAL << "Source can't be nullptr";

    CHECK_RESOURCE_CONTEXT(source_1_1)

    ALint result;
    alGetSourcei(source_1_1->id(), AL_SOURCE_STATE, &result);
    return Conversions::AL2SourceState(result);
}

void OpenAL_1_1_Renderer::playSource(const std::shared_ptr<core::audio::ISource> &source)
{
    CHECK_THIS_CONTEXT
    auto source_1_1 = std::dynamic_pointer_cast<Source_1_1>(source);
    if (!source_1_1)
        LOG_CRITICAL << "Source can't be nullptr";

    CHECK_RESOURCE_CONTEXT(source_1_1)

    alSourcePlay(source_1_1->id());
}

void OpenAL_1_1_Renderer::pauseSource(const std::shared_ptr<core::audio::ISource> &source)
{
    CHECK_THIS_CONTEXT
    auto source_1_1 = std::dynamic_pointer_cast<Source_1_1>(source);
    if (!source_1_1)
        LOG_CRITICAL << "Source can't be nullptr";

    CHECK_RESOURCE_CONTEXT(source_1_1)

    alSourcePause(source_1_1->id());
}

void OpenAL_1_1_Renderer::stopSource(const std::shared_ptr<core::audio::ISource> &source)
{
    CHECK_THIS_CONTEXT
    auto source_1_1 = std::dynamic_pointer_cast<Source_1_1>(source);
    if (!source_1_1)
        LOG_CRITICAL << "Source can't be nullptr";

    CHECK_RESOURCE_CONTEXT(source_1_1)

    alSourceStop(source_1_1->id());
}

void OpenAL_1_1_Renderer::rewindSource(const std::shared_ptr<core::audio::ISource> &source)
{
    CHECK_THIS_CONTEXT
    auto source_1_1 = std::dynamic_pointer_cast<Source_1_1>(source);
    if (!source_1_1)
        LOG_CRITICAL << "Source can't be nullptr";

    CHECK_RESOURCE_CONTEXT(source_1_1)

    alSourceRewind(source_1_1->id());
}

bool OpenAL_1_1_Renderer::doMakeCurrent()
{
    if (m_context == alcGetCurrentContext())
        return true;

    bool result = alcMakeContextCurrent(m_context) != 0;
    if (!result)
        LOG_ERROR << "Failed to make OpenAL context current";

    return result;
}

bool OpenAL_1_1_Renderer::doDoneCurrent()
{
    bool result = alcMakeContextCurrent(nullptr) != 0;
    if (!result)
        LOG_ERROR << "Failed to done OpenAL context current";

    return result;
}

}
}
