#include <core/lightdrawable.h>
#include <core/igraphicsrenderer.h>
#include <core/uniform.h>

#include "lightdrawableprivate.h"

namespace simplex
{
namespace core
{

LightDrawable::LightDrawable(const std::shared_ptr<graphics::IVertexArray> &vao)
    : Drawable(std::make_unique<LightDrawablePrivate>(vao))
{
    setDoubleSided(true);
}

LightDrawable::~LightDrawable() = default;

DrawableAlphaMode LightDrawable::alphaMode() const
{
    return DrawableAlphaMode::Opaque;
}

void LightDrawable::setShadowDepthMap(const graphics::PConstTexture &value)
{
    static graphics::UniformId s_uniformId = graphics::UniformId::ShadowDepthMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

void LightDrawable::setShadowColorMap(const graphics::PConstTexture &value)
{
    static graphics::UniformId s_uniformId = graphics::UniformId::ShadowColorMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

void LightDrawable::setShadowMatrix(const glm::mat4x4 &value)
{
    getOrCreateUniform(graphics::UniformId::ShadowMatrix) = makeUniform(value);
}

std::unordered_set<LightComponent> LightDrawable::lightComponentsSet() const
{
    return componentsSet<LightComponent>(uniformIdByLightComponent);
}

graphics::UniformId LightDrawable::uniformIdByLightComponent(LightComponent value)
{
    static const std::unordered_map<LightComponent, graphics::UniformId> s_table {
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? graphics::UniformId::Undefined : it->second;
}


}
}
