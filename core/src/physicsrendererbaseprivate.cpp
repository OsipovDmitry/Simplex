#include "physicsrendererbaseprivate.h"

namespace simplex
{
namespace core
{
namespace physics
{

std::weak_ptr<RendererBase> RendererBasePrivate::s_current;

RendererBasePrivate::RendererBasePrivate(const std::string& name)
    : m_name(name)
{
}

const std::string& RendererBasePrivate::name()
{
    return m_name;
}

std::weak_ptr<RendererBase>& RendererBasePrivate::current()
{
    return s_current;
}

} // namespace physics
} // namespace core
} // namespace simplex