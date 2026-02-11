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

bool OpenALDevice::isInitialized() const
{
    return m_->device() != nullptr;
}

void OpenALDevice::update(const std::shared_ptr<core::Scene>& scene, uint64_t time, uint32_t dt)
{
    m_->engine()->update(scene, time, dt);
}

std::shared_ptr<core::IEngine> OpenALDevice::engine()
{
    return audioEngine();
}

std::shared_ptr<const core::IEngine> OpenALDevice::engine() const
{
    return audioEngine();
}

std::shared_ptr<core::AudioEngine> OpenALDevice::audioEngine()
{
    return m_->engine();
}

std::shared_ptr<const core::AudioEngine> OpenALDevice::audioEngine() const
{
    return const_cast<OpenALDevice*>(this)->audioEngine();
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
    if (auto it = instances.find_if([&fullName](const std::shared_ptr<OpenALDevice>& value) { return value->name() == fullName; });
        it != instances.end())
    {
        result = it->lock();
    }

    if (!result)
    {
        result = std::shared_ptr<OpenALDevice>(new OpenALDevice(fullName));
        auto& resultPrivate = result->m();

        auto renderer = std::make_shared<OpenAL_1_1_Renderer>(fullName + "Renderer", result);

        auto engine = std::make_shared<core::AudioEngine>(fullName + "Engine", renderer);
        resultPrivate.engine() = engine;

        instances.push_back(result);

        renderer->makeCurrent();
        ALCint major = -1, minor = -1;
        alcGetIntegerv(resultPrivate.device(), ALC_MAJOR_VERSION, 1u, &major);
        alcGetIntegerv(resultPrivate.device(), ALC_MINOR_VERSION, 1u, &minor);
        LOG_INFO << "OpenALDevice has been created (Name: \"" << fullName << "\", OpenAL context ver:" << major << "." << minor << ")";
    }

    return result;
}

OpenALDevice::OpenALDevice(const std::string &deviceName)
    : m_(std::make_unique<OpenALDevicePrivate>(deviceName))
{
    auto device = alcOpenDevice(deviceName.c_str());
    if (!device)
        LOG_CRITICAL << "Failed to create OpenAL device " << deviceName;

    m_->device() = device;
}



}
}
