#ifndef AUDIO_OPENAL_DEVICE_PRIVATE_H
#define AUDIO_OPENAL_DEVICE_PRIVATE_H

#include <memory>
#include <string>

#include <utils/weakptrlist.h>

#include <physics_bullet/bulletworld.h>

namespace simplex
{
namespace physics_bullet
{

class BulletWorldPrivate
{
public:
    BulletWorldPrivate(const std::string&);

    std::string& name();
    std::shared_ptr<core::PhysicsEngine>& engine();

    static utils::WeakPtrList<BulletWorld>& instances();

private:
    std::string m_name;
    std::shared_ptr<core::PhysicsEngine> m_engine;

    static utils::WeakPtrList<BulletWorld> m_instances;
};

} // namespace physics_bullet
} // namespace simplex

#endif // AUDIO_OPENAL_DEVICE_PRIVATE_H
