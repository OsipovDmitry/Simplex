#ifndef CORE_IAUDIODEVICE_H
#define CORE_IAUDIODEVICE_H

#include <core/inamedobject.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class IAudioDevice : public INamedObject
{
public:
    ~IAudioDevice() override = default;

    virtual std::shared_ptr<AudioEngine> engine() = 0;
    virtual std::shared_ptr<const AudioEngine> engine() const = 0;
};

}
}

#endif // CORE_IAUDIODEVICE_H
