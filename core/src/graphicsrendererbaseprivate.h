#ifndef CORE_GRAPHICSRENDERERBASEPRIVATE_H
#define CORE_GRAPHICSRENDERERBASEPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{
namespace graphics
{

class RendererBasePrivate
{
public:
    RendererBasePrivate(const std::string &name);

    const const std::string& name();

    static std::weak_ptr<RendererBase> &current();

private:
    std::string m_name;
    static std::weak_ptr<RendererBase> s_current;
};

}
}
}

#endif // CORE_GRAPHICSRENDERERBASEPRIVATE_H
