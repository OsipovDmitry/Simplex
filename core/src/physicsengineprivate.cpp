#include "physicsengineprivate.h"

namespace simplex
{
namespace core
{

PhysicsEnginePrivate::PhysicsEnginePrivate(const std::string& name)
    : m_name(name)
{
}

std::string& PhysicsEnginePrivate::name()
{
    return m_name;
}

std::shared_ptr<physics::RendererBase>& PhysicsEnginePrivate::renderer()
{
    return m_renderer;
}

} // namespace core
} // namespace simplex
