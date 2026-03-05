#ifndef CORE_IDEVICE_H
#define CORE_IDEVICE_H

#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IDevice : public INamedObject
{
public:
    ~IDevice() override = default;

    virtual bool isInitialized() const = 0;

    virtual void update(const std::shared_ptr<Scene>&, uint64_t time, uint32_t dt) = 0;

    virtual std::shared_ptr<IEngine> engine() = 0;
    virtual std::shared_ptr<const IEngine> engine() const = 0;
};

} // namespace core
} // namespace simplex

#endif // CORE_IDEVICE_H
