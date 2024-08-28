#include <cstring>

#include <utils/logger.h>

#include <openal/openaldevice.h>

#include "openal_1_1_renderer.h"
#include "openaldeviceprivate.h"

namespace simplex
{
namespace openal
{

OpenALDevice::~OpenALDevice()
{
    m_->renderer() = nullptr;

    const auto deviceName = OpenALDevice::name();

    if (!alcCloseDevice(m_->device()))
        LOG_ERROR << "Failed to destroy OpenAL device \"" << deviceName << "\"";
    else
        LOG_INFO << "OpenAL device \"" << deviceName << "\" has been destroyed";
}

const std::string &OpenALDevice::name() const
{
    return m_->name();
}

std::shared_ptr<core::audio::IRenderer> OpenALDevice::renderer()
{
    return m_->renderer();
}

std::shared_ptr<const core::audio::IRenderer> OpenALDevice::renderer() const
{
    return const_cast<OpenALDevice*>(this)->renderer();
}

const std::string &OpenALDevice::defaultDeviceName()
{
    static const std::string s_defaultDeviceName = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
    return s_defaultDeviceName;
}

const std::list<std::string> &OpenALDevice::devicesNamesList()
{
    static std::list<std::string> s_devicesNamesList;
    if (s_devicesNamesList.empty())
    {
        for (auto devices = static_cast<const char*>(alcGetString(nullptr, ALC_DEVICE_SPECIFIER));
             devices && *devices != 0;
             devices += strlen(devices) + 1u)
        {
            s_devicesNamesList.push_back(devices);
        }
    }
    return s_devicesNamesList;
}

std::shared_ptr<OpenALDevice> OpenALDevice::getOrCreate(const std::string &deviceName)
{
    auto fullName = deviceName.empty() ? defaultDeviceName() : deviceName;
    std::shared_ptr<OpenALDevice> result;

    auto &instances = OpenALDevicePrivate::instances();
    for (auto it = instances.begin(); it != instances.end(); )
    {
        if (it->expired())
            it = instances.erase(it);
        else if (auto device = it->lock(); device->name() == fullName)
        {
            result = device;
            break;
        }
        else
            ++it;
    }

    if (!result)
    {
        result = std::shared_ptr<OpenALDevice>(new OpenALDevice(fullName));
        OpenALDevicePrivate::instances().insert(result);
    }

    return result;
}

OpenALDevice::OpenALDevice(const std::string &deviceName)
    : m_(std::make_unique<OpenALDevicePrivate>(deviceName))
{
    auto device = alcOpenDevice(deviceName.c_str());
    if (!device)
        LOG_CRITICAL << "Failed to create OpenAL device " << deviceName;

    auto context = alcCreateContext(device, nullptr);
    if (!context)
        LOG_CRITICAL << "Failed to create OpenAL context";

    auto renderer = OpenAL_1_1_Renderer::create(context);

    m_->device() = device;
    m_->renderer() = renderer;

    m_->renderer()->makeCurrent();

    ALCint major = -1, minor = -1;
    alcGetIntegerv(device, ALC_MAJOR_VERSION, 1u, &major);
    alcGetIntegerv(device, ALC_MINOR_VERSION, 1u, &minor);

    LOG_INFO << "OpenAL device has been created (Name: \"" << deviceName << "\", OpenAL context ver:" << major << "." << minor << ")";
}



}
}
