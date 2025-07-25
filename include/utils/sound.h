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
    Sound(size_t, uint32_t, uint16_t, uint16_t, const uint8_t* = nullptr);
    ~Sound();

    size_t numFrames() const;
    uint32_t sampleRate() const;
    uint16_t numChannels() const;
    uint16_t bitsPerSample() const;
    const uint8_t* data() const;

private:
    Sound();

    size_t m_numFrames;
    uint32_t m_sampleRate;
    uint16_t m_numChannels;
    uint16_t m_bitsPerSample;
    uint8_t* m_data;

    friend class SoundManager;
};

}
}

#endif // UTILS_SOUND_H
