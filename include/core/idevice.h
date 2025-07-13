#ifndef CORE_IDEVICE_H
#define CORE_IDEVICE_H

#include <core/inamedobject.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class IDevice : public INamedObject
{
public:
    ~IDevice() override = default;
    
    virtual bool isInitialized() const = 0;

    virtual void update(uint64_t time, uint32_t dt, debug::SceneInformation&) = 0;

    virtual std::shared_ptr<IEngine> engine() = 0;
    virtual std::shared_ptr<const IEngine> engine() const = 0;

    virtual std::shared_ptr<Scene> scene() = 0;
    virtual std::shared_ptr<const Scene> scene() const = 0;
    virtual void setScene(const std::shared_ptr<Scene>&) = 0;
};

}
}

#endif // CORE_IDEVICE_H
