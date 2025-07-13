#ifndef CORE_IAUDIODEVICE_H
#define CORE_IAUDIODEVICE_H

#include <core/idevice.h>

namespace simplex
{
namespace core
{
namespace audio
{

class IAudioDevice : public IDevice
{
public:
    ~IAudioDevice() override = default;

    virtual std::shared_ptr<AudioEngine> audioEngine() = 0;
    virtual std::shared_ptr<const AudioEngine> audioEngine() const = 0;
};

}
}
}

#endif // CORE_IAUDIODEVICE_H
