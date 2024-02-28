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

ENUMCLASS(LightDrawableType, uint16_t,
          Point,
          Spot,
          Directional,
          IBL)

class LightDrawablePrivate;
class CORE_SHARED_EXPORT LightDrawable : public Drawable
{
    PIMPL(LightDrawable)
public:
    LightDrawable(const std::shared_ptr<graphics::IVertexArray>&, LightDrawableType);
    ~LightDrawable() override;

    DrawableAlphaMode alphaMode() const override;

    LightDrawableType type() const;

    static graphics::LightComponentsSet lightComponentsSet(const std::shared_ptr<const LightDrawable>&);
};

}
}

#endif // CORE_LIGHTDRAWABLE_H
