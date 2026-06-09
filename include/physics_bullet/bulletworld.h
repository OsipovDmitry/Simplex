#ifndef PHYSICS_BULLET_WORLD_H
#define PHYSICS_BULLET_WORLD_H

#include <memory>
#include <string>

#include <utils/noncopyble.h>
#include <utils/pimpl.h>

#include <core/iphysicsworld.h>

#include <physics_bullet/bulletglobal.h>

namespace simplex
{
namespace physics_bullet
{

class BulletWorldPrivate;
class PHYSICS_BULLET_SHARED_EXPORT BulletWorld : public core::physics::IPhysicsWorld
{
    NONCOPYBLE(BulletWorld)
    PRIVATE_IMPL(BulletWorld)

public:
    ~BulletWorld() override;

    const std::string& name() const override;

    bool isInitialized() const override;

    void update(const std::shared_ptr<core::Scene>&, uint64_t time, uint32_t dt) override;

    std::shared_ptr<core::IEngine> engine() override;
    std::shared_ptr<const core::IEngine> engine() const override;

    std::shared_ptr<core::PhysicsEngine> physicsEngine() override;
    std::shared_ptr<const core::PhysicsEngine> physicsEngine() const override;

    static std::shared_ptr<BulletWorld> getOrCreate(const std::string& = "");

private:
    BulletWorld(const std::string&);

    std::unique_ptr<BulletWorldPrivate> m_;
};

} // namespace physics_bullet
} // namespace simplex

#endif // PHYSICS_BULLET_WORLD_H
