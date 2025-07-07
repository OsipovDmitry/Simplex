#ifndef AUDIO_OPENAL_DEVICE_PRIVATE_H
#define AUDIO_OPENAL_DEVICE_PRIVATE_H

#include <unordered_set>
#include <string>
#include <memory>
#include <alc.h>

#include <utils/weakptrhash.h>

#include <audio_openal/openaldevice.h>

namespace simplex
{
namespace audio_openal
{

using utils::operator ==;

class OpenAL_1_1_Renderer;

class OpenALDevicePrivate
{
public:
    OpenALDevicePrivate(const std::string&);

    std::string &name();
    ALCdevice *&device();
    std::shared_ptr<core::AudioEngine> &engine();

    static std::unordered_set<std::weak_ptr<OpenALDevice>, utils::WeakPtrHash<OpenALDevice>> &instances();

private:
    std::string m_name;
    ALCdevice *m_device;
    std::shared_ptr<core::AudioEngine> m_engine;

    static std::unordered_set<std::weak_ptr<OpenALDevice>, utils::WeakPtrHash<OpenALDevice>> m_instances;
};

}
}

#endif // AUDIO_OPENAL_DEVICE_PRIVATE_H
