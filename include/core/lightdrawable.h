#ifndef CORE_LIGHTDRAWABLE_H
#define CORE_LIGHTDRAWABLE_H

#include <utils/forwarddecl.h>
#include <utils/enumclass.h>
#include <utils/pimpl.h>

#include <core/drawable.h>

namespace simplex
{
namespace core
{

ENUMCLASS(LightComponent, uint16_t,
          None)

class LightDrawablePrivate;
class CORE_SHARED_EXPORT LightDrawable : public Drawable
{
    PIMPL(LightDrawable)
public:
    LightDrawable(const std::shared_ptr<graphics::IVertexArray>&);
    ~LightDrawable() override;

    DrawableAlphaMode alphaMode() const override;

    void setShadowDepthMap(const graphics::PConstTexture&);
    void setShadowColorMap(const graphics::PConstTexture&);

    void setShadowMatrix(const glm::mat4x4&);

    std::unordered_set<LightComponent> lightComponentsSet() const;

    static graphics::UniformId uniformIdByLightComponent(LightComponent);
};

}
}

#endif // CORE_LIGHTDRAWABLE_H
