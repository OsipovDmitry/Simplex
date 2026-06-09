#ifndef CORE_PHYSICSRENDERERBASEPRIVATE_H
#define CORE_PHYSICSRENDERERBASEPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{
namespace physics
{

class RendererBasePrivate
{
public:
    RendererBasePrivate(const std::string& name);

    const const std::string& name();

    static std::weak_ptr<RendererBase>& current();

private:
    std::string m_name;
    static std::weak_ptr<RendererBase> s_current;
};

} // namespace physics
} // namespace core
} // namespace simplex

#endif // CORE_PHYSICSRENDERERBASEPRIVATE_H
