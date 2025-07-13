#include "graphicsrendererbaseprivate.h"

namespace simplex
{
namespace core
{
namespace graphics
{

std::weak_ptr<RendererBase> RendererBasePrivate::s_current;

RendererBasePrivate::RendererBasePrivate(const std::string &name)
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

}
}
}