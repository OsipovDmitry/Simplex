#include "openaldeviceprivate.h"

namespace simplex
{
namespace audio_openal
{

utils::WeakPtrList<OpenALDevice> OpenALDevicePrivate::m_instances;

OpenALDevicePrivate::OpenALDevicePrivate(const std::string &name)
    : m_name(name)
    , m_device(nullptr)
{
}

std::string &OpenALDevicePrivate::name()
{
    return m_name;
}

ALCdevice *&OpenALDevicePrivate::device()
{
    return m_device;
}

std::shared_ptr<core::AudioEngine>& OpenALDevicePrivate::engine()
{
    return m_engine;
}

std::shared_ptr<core::Scene>& OpenALDevicePrivate::scene()
{
    return m_scene;
}

utils::WeakPtrList<OpenALDevice>&OpenALDevicePrivate::instances()
{
    return m_instances;
}

}
}
