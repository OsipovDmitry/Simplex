#ifndef AUDIO_OPENAL_DEVICE_PRIVATE_H
#define AUDIO_OPENAL_DEVICE_PRIVATE_H

#include <memory>
#include <string>

#include <utils/weakptrlist.h>

#include <audio_openal/openaldevice.h>

class ALCdevice;

namespace simplex
{
namespace audio_openal
{

class OpenALDevicePrivate
{
public:
    OpenALDevicePrivate(const std::string&);

    std::string& name();
    std::shared_ptr<core::AudioEngine>& engine();

    ALCdevice*& device();

    static utils::WeakPtrList<OpenALDevice>& instances();

private:
    std::string m_name;
    std::shared_ptr<core::AudioEngine> m_engine;

    ALCdevice* m_device;

    static utils::WeakPtrList<OpenALDevice> m_instances;
};

} // namespace audio_openal
} // namespace simplex

#endif // AUDIO_OPENAL_DEVICE_PRIVATE_H
