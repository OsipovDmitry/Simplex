#ifndef UTILS_SOUND_H
#define UTILS_SOUND_H

#include <memory>
#include <filesystem>

#include <utils/utilsglobal.h>
#include <utils/noncopyble.h>

namespace simplex
{
namespace utils
{

class UTILS_SHARED_EXPORT Sound final
{
    NONCOPYBLE(Sound)
public:
    Sound();
    ~Sound();

    size_t numFrames() const;
    const uint8_t *data() const;
    uint32_t sampleRate() const;
    uint16_t numChannels() const;
    uint16_t bitsPerSample() const;

    static std::shared_ptr<Sound> loadFromFile(const std::filesystem::path&);

private:
    size_t m_numFrames;
    uint8_t *m_data;
    uint32_t m_sampleRate;
    uint16_t m_numChannels;
    uint16_t m_bitsPerSample;
};

}
}

#endif // UTILS_SOUND_H
