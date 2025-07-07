#include "openaldeviceprivate.h"

namespace simplex
{
namespace audio_openal
{

std::unordered_set<std::weak_ptr<OpenALDevice>, utils::WeakPtrHash<OpenALDevice>> OpenALDevicePrivate::m_instances;

OpenALDevicePrivate::OpenALDevicePrivate(const std::string &name)
    : m_name(name)
    , m_device(nullptr)
{
}

std::string &OpenALDevicePrivate::name()
{
    std::hash<std::string> i;
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

std::unordered_set<std::weak_ptr<OpenALDevice>, utils::WeakPtrHash<OpenALDevice>> &OpenALDevicePrivate::instances()
{
    return m_instances;
}

}
}
