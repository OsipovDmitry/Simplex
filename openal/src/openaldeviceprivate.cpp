#include "openaldeviceprivate.h"

namespace simplex
{
namespace openal
{

std::unordered_set<std::weak_ptr<OpenALDevice>, OpenALDeviceHash> OpenALDevicePrivate::m_instances;

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

std::shared_ptr<OpenAL_1_1_Renderer> &OpenALDevicePrivate::renderer()
{
    return m_renderer;
}

std::unordered_set<std::weak_ptr<OpenALDevice>, OpenALDeviceHash> &OpenALDevicePrivate::instances()
{
    return m_instances;
}

}
}
