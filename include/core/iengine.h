#ifndef IENGINE_H
#define IENGINE_H

#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IEngine : public INamedObject
{
public:
    virtual void update() = 0;
};

}
}

#endif // IENGINE_H
