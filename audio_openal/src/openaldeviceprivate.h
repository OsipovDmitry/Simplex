#ifndef AUDIO_OPENAL_DEVICE_PRIVATE_H
#define AUDIO_OPENAL_DEVICE_PRIVATE_H

#include <unordered_set>
#include <string>
#include <memory>
#include <alc.h>

#include <utils/weakptrlist.h>

#include <audio_openal/openaldevice.h>

namespace simplex
{
namespace audio_openal
{

class OpenALDevicePrivate
{
public:
    OpenALDevicePrivate(const std::string&);

    std::string &name();
    std::shared_ptr<core::AudioEngine> &engine();

    ALCdevice*& device();

    static utils::WeakPtrList<OpenALDevice> &instances();

private:
    std::string m_name;
    std::shared_ptr<core::AudioEngine> m_engine;

    ALCdevice* m_device;

    static utils::WeakPtrList<OpenALDevice> m_instances;
};

}
}

#endif // AUDIO_OPENAL_DEVICE_PRIVATE_H
