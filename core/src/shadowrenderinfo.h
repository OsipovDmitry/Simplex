#ifndef CORE_SHADOWRENDERINFO_H
#define CORE_SHADOWRENDERINFO_H

#include <core/forwarddecl.h>
#include <core/stateset.h>

namespace simplex
{
namespace core
{

class ShadowRenderInfo : public StateSet
{
public:
    ShadowRenderInfo();
    ~ShadowRenderInfo() override;

    void setViewportSize(const glm::uvec2&);

    void setLayeredShadowMatricesBuffer(const graphics::PConstBuffer&);

};

}
}

#endif // CORE_SHADOWRENDERINFO_H
