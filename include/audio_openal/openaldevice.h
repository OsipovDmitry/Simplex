#ifndef AUDIO_OPENAL_DEVICE_H
#define AUDIO_OPENAL_DEVICE_H

#include <memory>
#include <string>
#include <list>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/iaudiodevice.h>

#include <audio_openal/openalglobal.h>

namespace simplex
{
namespace audio_openal
{

class OpenALDevicePrivate;
class AUDIO_OPENAL_SHARED_EXPORT OpenALDevice : public core::audio::IAudioDevice
{
    NONCOPYBLE(OpenALDevice)
    PRIVATE_IMPL(OpenALDevice)

public:
    ~OpenALDevice() override;

    const std::string &name() const override;

    bool isInitialized() const override;

    void update(const std::shared_ptr<core::Scene>&, uint64_t time, uint32_t dt) override;

    std::shared_ptr<core::IEngine> engine() override;
    std::shared_ptr<const core::IEngine> engine() const override;

    std::shared_ptr<core::AudioEngine> audioEngine() override;
    std::shared_ptr<const core::AudioEngine> audioEngine() const override;

    static const std::string &defaultDeviceName();
    static const std::list<std::string> &devicesNamesList();

    static std::shared_ptr<OpenALDevice> getOrCreate(const std::string& = "");

private:
    OpenALDevice(const std::string&);

    std::unique_ptr<OpenALDevicePrivate> m_;
};

}
}

#endif // AUDIO_OPENAL_DEVICE_H
