#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <memory>

#include <utils/noncopyble.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/iengine.h>

namespace simplex
{
namespace core
{

class CORE_SHARED_EXPORT PhysicsEngine : public IEngine
{
    NONCOPYBLE(PhysicsEngine)
public:
    PhysicsEngine(std::shared_ptr<IPhysicsRenderer>);

    void update() override;
};

}
}

#endif // PHYSICSENGINE_H
