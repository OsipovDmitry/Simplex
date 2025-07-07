#include <cstring>

#include <utils/logger.h>

#include <audio_openal/openaldevice.h>

#include <core/audioengine.h>

#include "openal_1_1_renderer.h"
#include "openaldeviceprivate.h"

namespace simplex
{
namespace audio_openal
{

OpenALDevice::~OpenALDevice()
{
    m_->engine() = nullptr;

    const auto &deviceName = OpenALDevice::name();
    if (!alcCloseDevice(m_->device()))
        LOG_ERROR << "Failed to destroy OpenAL device \"" << deviceName << "\"";
    else
        LOG_INFO << "OpenALDevice \"" << deviceName << "\" has been destroyed";
}

const std::string &OpenALDevice::name() const
{
    return m_->name();
}

std::shared_ptr<core::AudioEngine> OpenALDevice::engine()
{
    return std::shared_ptr<core::AudioEngine>();
}

std::shared_ptr<const core::AudioEngine> OpenALDevice::engine() const
{
    return const_cast<OpenALDevice*>(this)->engine();
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
    for (auto it = instances.begin(); it != instances.end(); ++it)
    {
        if (it->expired())
            continue;

        if (auto device = it->lock(); device->name() == fullName)
        {
            result = device;
            break;
        }
    }

    if (!result)
    {
        result = std::shared_ptr<OpenALDevice>(new OpenALDevice(fullName));

        auto renderer = std::make_shared<OpenAL_1_1_Renderer>(fullName + "Renderer", result);

        auto engine = std::make_shared<core::AudioEngine>(fullName + "Engine", renderer);
        result->m().engine() = engine;

        renderer->makeCurrent();
        ALCint major = -1, minor = -1;
        alcGetIntegerv(result->m().device(), ALC_MAJOR_VERSION, 1u, &major);
        alcGetIntegerv(result->m().device(), ALC_MINOR_VERSION, 1u, &minor);
        LOG_INFO << "OpenALDevice has been created (Name: \"" << deviceName << "\", OpenAL context ver:" << major << "." << minor << ")";

        OpenALDevicePrivate::instances().insert(result);
    }

    return result;
}

OpenALDevice::OpenALDevice(const std::string &deviceName)
    : core::IAudioDevice()
    , m_(std::make_unique<OpenALDevicePrivate>(deviceName))
{
    auto device = alcOpenDevice(deviceName.c_str());
    if (!device)
        LOG_CRITICAL << "Failed to create OpenAL device " << deviceName;

    m_->device() = device;
}



}
}
