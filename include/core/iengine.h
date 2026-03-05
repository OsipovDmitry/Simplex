#ifndef IENGINE_H
#define IENGINE_H

#include <core/forwarddecl.h>
#include <core/inamedobject.h>

namespace simplex
{
namespace core
{

class IEngine : public INamedObject
{
public:
    ~IEngine() override = default;

    virtual std::shared_ptr<IRenderer> renderer() = 0;
    virtual std::shared_ptr<const IRenderer> renderer() const = 0;
};

} // namespace core
} // namespace simplex

#endif // IENGINE_H
