#ifndef CORE_BACKGROUNDDRAWABLE_H
#define CORE_BACKGROUNDDRAWABLE_H

#include <utils/forwarddecl.h>
#include <utils/pimpl.h>
#include <utils/glm/vec3.hpp>

#include <core/drawable.h>

namespace simplex
{
namespace core
{

ENUMCLASS(BackgroundComponent, uint16_t,
          ColorMap,
          BaseColor,
          Roughness)

class BackgroundDrawablePrivate;
class CORE_SHARED_EXPORT BackgroundDrawable : public Drawable
{
    PIMPL(BackgroundDrawable)
public:
    BackgroundDrawable(const std::shared_ptr<graphics::IVertexArray>&);
    ~BackgroundDrawable() override;

    DrawableAlphaMode alphaMode() const override;

    graphics::PConstTexture colorMap() const;
    void setColorMap(const graphics::PConstTexture&);

    const glm::vec3 &baseColor() const;
    void setBaseColor(const glm::vec3&);

    float roughness() const;
    void setRoughness(float);

    std::unordered_set<BackgroundComponent> backgroundComponentsSet() const;

    static graphics::UniformId uniformIdByBackgroundComponent(BackgroundComponent);
};

}
}
#endif // CORE_BACKGROUNDDRAWABLE_H
