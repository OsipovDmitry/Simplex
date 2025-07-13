#ifndef IENGINE_H
#define IENGINE_H

#include <core/inamedobject.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class IEngine : public INamedObject
{
public:

    virtual std::shared_ptr<IRenderer> renderer() = 0;
    virtual std::shared_ptr<const IRenderer> renderer() const = 0;

};

}
}

#endif // IENGINE_H
