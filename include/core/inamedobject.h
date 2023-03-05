#ifndef INAMEDOBJECT_H
#define INAMEDOBJECT_H

#include <string>

namespace simplex
{
namespace core
{

class INamedObject
{
public:
    virtual ~INamedObject() = default;
    virtual std::string name() const = 0;
};

}
}

#endif // INAMEDOBJECT_H
