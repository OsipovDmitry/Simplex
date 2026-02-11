#include <utils/fileextension.h>
#include <utils/sound.h>

#include "wave/file.h"

namespace simplex
{
namespace utils
{

Sound::Sound(size_t numFrames, uint32_t sampleRate, uint16_t numChannels, uint16_t bitsPerSample, const uint8_t* data)
    : m_framesCount(numFrames)
    , m_sampleRate(sampleRate)
    , m_channelsCount(numChannels)
    , m_bitsPerSample(bitsPerSample)
    , m_data(nullptr)
{
    auto dataSize = m_framesCount * m_channelsCount * (m_bitsPerSample / 8u);
    m_data = reinterpret_cast<uint8_t*>(std::malloc(dataSize));

    if (data)
        std::memcpy(m_data, data, dataSize);
}

Sound::~Sound()
{
    std::free(m_data);
}

size_t Sound::framesCount() const
{
    return m_framesCount;
}

uint32_t Sound::sampleRate() const
{
    return m_sampleRate;
}

uint16_t Sound::channelsCount() const
{
    return m_channelsCount;
}

uint16_t Sound::bitsPerSample() const
{
    return m_bitsPerSample;
}

const uint8_t* Sound::data() const
{
    return m_data;
}

static void NoDecrypt(char*, size_t) {}

std::shared_ptr<Sound> Sound::loadFromFile(const std::filesystem::path& fileName)
{
    const auto absoluteFilename = std::filesystem::canonical(fileName);

    size_t framesCount;
    uint8_t* data = nullptr;
    uint32_t sampleRate;
    uint16_t channelsCount, bitsPerSample;

    const auto fileExt = fileExtension(absoluteFilename);
    if (fileExt == L".wav")
    {
        wave::File file;
        wave::Error err = file.Open(absoluteFilename.string(), wave::kIn);
        if (err)
            return nullptr;

        framesCount = file.frame_number();
        sampleRate = file.sample_rate();
        channelsCount = file.channel_number();
        bitsPerSample = file.bits_per_sample();

        const auto dataSize = framesCount * channelsCount * (bitsPerSample / 8u);
        data = new uint8_t[dataSize];
        file.Read(framesCount, NoDecrypt, data);
    }
    else
        return nullptr;

    return std::make_shared<Sound>(framesCount, sampleRate, channelsCount, bitsPerSample, data);
}

}
}
