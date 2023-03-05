#ifndef IAPPLICATION_H
#define IAPPLICATION_H

#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IApplication : public INamedObject
{
public:
    virtual ~IApplication() = default;

    virtual void update() = 0;
};

}
}

#endif // IAPPLICATION_H
