#ifndef CORE_PHYSICSENGINEPRIVATE_H
#define CORE_PHYSICSENGINEPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class PhysicsEnginePrivate
{
public:
    PhysicsEnginePrivate(const std::string&);

    std::string& name();
    std::shared_ptr<physics::RendererBase>& renderer();

private:
    std::string m_name;
    std::shared_ptr<physics::RendererBase> m_renderer;
};

} // namespace core
} // namespace simplex

#endif // CORE_PHYSICSENGINEPRIVATE_H
