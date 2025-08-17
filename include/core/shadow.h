#ifndef CORE_SHADOW_H
#define CORE_SHADOW_H

#include <memory>
#include <vector>

#include <utils/glm/vec2.hpp>
#include <utils/pimpl.h>
#include <utils/enumclass.h>
#include <utils/forwarddecl.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

ENUMCLASS(ShadingMode, uint16_t,
          Disabled, // no shading
          Opaque, // opaque geometries cast balck shadow
          OpaqueAndTransparent, // all the geometries cast black shadow
          Color) // opaque geometries cast black shadow, transparent geometies cast color shadow

ENUMCLASS(ShadingFilter, uint16_t,
          Point,
          PCF,
          VSM)

class ShadowPrivate;
class CORE_SHARED_EXPORT Shadow
{
    PRIVATE_IMPL(Shadow)
public:
    Shadow(LightType);
    ~Shadow();

    ShadingMode mode() const;
    void setMode(ShadingMode);

    ShadingFilter filter() const;
    void setFilter(ShadingFilter);

    float depthBias() const;
    void setDepthBias(float);

    const glm::uvec2 &mapSize() const;
    void setMapSize(const glm::uvec2&);

    const utils::Range &cullPlanesLimits() const;
    void setCullPlanesLimits(const utils::Range&);

private:
    std::unique_ptr<ShadowPrivate> m_;
};

}
}

#endif // CORE_SHADOW_H
