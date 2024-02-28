#include <core/lightdrawable.h>

#include "lightdrawableprivate.h"

namespace simplex
{
namespace core
{

LightDrawable::LightDrawable(const std::shared_ptr<graphics::IVertexArray> &vao, LightDrawableType type)
    : Drawable(std::make_unique<LightDrawablePrivate>(vao, type))
{
    setDoubleSided(true);
}

LightDrawable::~LightDrawable() = default;

DrawableAlphaMode LightDrawable::alphaMode() const
{
    return DrawableAlphaMode::Opaque;
}

LightDrawableType LightDrawable::type() const
{
    return m().type();
}

graphics::LightComponentsSet LightDrawable::lightComponentsSet(const std::shared_ptr<const LightDrawable> &/*lightDrawable*/)
{
    graphics::LightComponentsSet result;

    return result;
}


}
}
