#ifndef CORE_AUDIORENDERERBASEPRIVATE_H
#define CORE_AUDIORENDERERBASEPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{
namespace audio
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

#endif // CORE_AUDIORENDERERBASEPRIVATE_H
