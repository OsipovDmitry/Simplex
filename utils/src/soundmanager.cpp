#include <filesystem>

#include <utils/logger.h>
#include <utils/fileextension.h>
#include <utils/sound.h>
#include <utils/soundmanager.h>

#include "wave/file.h"

namespace simplex
{
namespace utils
{

SoundManager::~SoundManager() = default;

static void NoDecrypt(char*, size_t) {}

std::shared_ptr<Sound> SoundManager::loadOrGet(const std::filesystem::path& filename)
{
    const auto absoluteFilename = std::filesystem::absolute(filename);

    if (auto it = m_sounds.find(absoluteFilename); it != m_sounds.end())
        return it->second;

    size_t numFrames;
    uint8_t* data = nullptr;
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

        const auto dataSize = numFrames * numChannels * (bitsPerSample / 8u);
        data = new uint8_t[dataSize];
        file.Read(numFrames, NoDecrypt, data);
    }
    else
        return nullptr;

    auto result = std::shared_ptr<Sound>(new Sound);
    result->m_numFrames = numFrames;
    result->m_data = data;
    result->m_sampleRate = sampleRate;
    result->m_numChannels = numChannels;
    result->m_bitsPerSample = bitsPerSample;

    m_sounds.insert({ absoluteFilename, result });
    return result;
}

SoundManager::SoundManager()
{}


}
}
