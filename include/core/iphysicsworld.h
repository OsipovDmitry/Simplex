#ifndef CORE_IPHYSICSWORLD_H
#define CORE_IPHYSICSWORLD_H

#include <core/idevice.h>

namespace simplex
{
namespace core
{
namespace physics
{

class IPhysicsWorld : public IDevice
{
public:
    ~IPhysicsWorld() override = default;

    virtual std::shared_ptr<PhysicsEngine> physicsEngine() = 0;
    virtual std::shared_ptr<const PhysicsEngine> physicsEngine() const = 0;
};

} // namespace physics
} // namespace core
} // namespace simplex

#endif // CORE_IPHYSICSWORLD_H
