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
    virtual const std::string& name() const = 0;
};

} // namespace core
} // namespace simplex

#endif // INAMEDOBJECT_H
