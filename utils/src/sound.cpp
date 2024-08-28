#include <utils/sound.h>
#include <utils/fileextension.h>

#include "wave/file.h"

namespace simplex
{
namespace utils
{

Sound::Sound()
    : m_numFrames(0u)
    , m_data(nullptr)
    , m_sampleRate(0u)
    , m_numChannels(0u)
    , m_bitsPerSample(0u)
{
}

Sound::~Sound()
{
    delete [] m_data;
}

size_t Sound::numFrames() const
{
    return m_numFrames;
}

const uint8_t *Sound::data() const
{
    return m_data;
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

void NoDecrypt(char*, size_t) {}

std::shared_ptr<Sound> Sound::loadFromFile(const std::filesystem::path &filename)
{
    size_t numFrames;
    uint8_t *data = nullptr;
    uint32_t sampleRate;
    uint16_t numChannels, bitsPerSample;

    const auto fileExt = fileExtension(filename);
    if (fileExt == L".wav")
    {
        wave::File file;
        wave::Error err = file.Open(filename.string(), wave::kIn);
        if (err)
            return nullptr;

        numFrames = file.frame_number();
        sampleRate = file.sample_rate();
        numChannels = file.channel_number();
        bitsPerSample = file.bits_per_sample();

        auto dataSize = numFrames * numChannels * (bitsPerSample / 8u);
        data = new uint8_t[dataSize];
        file.Read(numFrames, NoDecrypt, data);
    }
    else
        return nullptr;

    auto result = std::make_shared<Sound>();
    result->m_numFrames = numFrames;
    result->m_data = data;
    result->m_sampleRate = sampleRate;
    result->m_numChannels = numChannels;
    result->m_bitsPerSample = bitsPerSample;
    return result;
}

}
}
