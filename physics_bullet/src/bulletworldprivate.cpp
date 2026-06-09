#include "bulletworldprivate.h"

namespace simplex
{
namespace physics_bullet
{

utils::WeakPtrList<BulletWorld> BulletWorldPrivate::m_instances;

BulletWorldPrivate::BulletWorldPrivate(const std::string& name)
    : m_name(name)
{
}

std::string& BulletWorldPrivate::name()
{
    return m_name;
}

std::shared_ptr<core::PhysicsEngine>& BulletWorldPrivate::engine()
{
    return m_engine;
}

utils::WeakPtrList<BulletWorld>& BulletWorldPrivate::instances()
{
    return m_instances;
}

} // namespace physics_bullet
} // namespace simplex
