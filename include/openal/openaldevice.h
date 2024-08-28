#ifndef OPENALDEVICE_H
#define OPENALDEVICE_H

#include <memory>
#include <string>
#include <list>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/forwarddecl.h>
#include <core/inamedobject.h>

#include <openal/openalglobal.h>

namespace simplex
{
namespace openal
{

class OpenALDevicePrivate;
class OPENAL_SHARED_EXPORT OpenALDevice : public core::INamedObject
{
    NONCOPYBLE(OpenALDevice)
    PRIVATE_IMPL(OpenALDevice)

public:
    ~OpenALDevice() override;

    const std::string &name() const override;

    std::shared_ptr<core::audio::IRenderer> renderer();
    std::shared_ptr<const core::audio::IRenderer> renderer() const;

    static const std::string &defaultDeviceName();
    static const std::list<std::string> &devicesNamesList();

    static std::shared_ptr<OpenALDevice> getOrCreate(const std::string& = "");

private:
    OpenALDevice(const std::string&);

    std::unique_ptr<OpenALDevicePrivate> m_;
};

}
}

#endif // OPENALDEVICE_H
