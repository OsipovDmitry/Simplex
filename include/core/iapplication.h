#ifndef IAPPLICATION_H
#define IAPPLICATION_H

#include <inttypes.h>

#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IApplication : public INamedObject
{
public:
    ~IApplication() override = default;

    virtual void shutDown() = 0;
    virtual void update(uint64_t time, uint32_t dt) = 0;
};

}
}

#endif // IAPPLICATION_H
