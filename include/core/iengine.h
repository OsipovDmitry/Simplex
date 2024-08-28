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
    virtual void update(const std::shared_ptr<IRenderWidget>&,
                        const std::shared_ptr<Scene>&,
                        uint64_t time,
                        uint32_t dt,
                        debug::SceneInformation&) = 0;
};

}
}

#endif // IENGINE_H
