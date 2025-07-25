#include <utils/sound.h>
#include <utils/fileextension.h>

namespace simplex
{
namespace utils
{

Sound::Sound(size_t numFrames, uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample, const uint8_t* data)
    : m_numFrames(numFrames)
    , m_sampleRate(sampleRate)
    , m_numChannels(numChannels)
    , m_bitsPerSample(bitsPerSample)
    , m_data(nullptr)
{
    auto dataSize = m_numFrames * m_numChannels * (m_bitsPerSample / 8u);
    m_data = reinterpret_cast<uint8_t*>(std::malloc(dataSize));

    if (data)
        std::memcpy(m_data, data, dataSize);
}

Sound::~Sound()
{
    std::free(m_data);
}

size_t Sound::numFrames() const
{
    return m_numFrames;
}

uint32_t Sound::sampleRate() const
{
    return m_sampleRate;
}

uint16_t Sound::numChannels() const
{
    return m_numChannels;
}

uint16_t Sound::bitsPerSample() const
{
    return m_bitsPerSample;
}

const uint8_t* Sound::data() const
{
    return m_data;
}

Sound::Sound()
    : m_numFrames(0u)
    , m_sampleRate(0u)
    , m_numChannels(0u)
    , m_bitsPerSample(0u)
    , m_data(nullptr)
{}

}
}
