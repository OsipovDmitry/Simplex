#include <core/settings.h>
#include <core/uniform.h>

#include "lightdrawable.h"

namespace simplex
{
namespace core
{

LightDrawable::LightDrawable(const std::shared_ptr<graphics::VAOMesh> &vao)
    : Drawable(vao)
{
    setDoubleSided(true);
    setDielecticSpecular(settings::Settings::instance().graphics().pbr().dielectricSpecular());
}

LightDrawable::~LightDrawable() = default;

float LightDrawable::dielectricSpecular() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::LightDielecticSpecular));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().dielectricSpecular();
}

void LightDrawable::setDielecticSpecular(float value)
{
    getOrCreateUniform(UniformId::LightDielecticSpecular) = makeUniform(value);
}

void LightDrawable::setShadowDepthMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::ShadowDepthMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

void LightDrawable::setShadowColorMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::ShadowColorMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

void LightDrawable::setShadowViewMatrix(const glm::mat4x4 &vm)
{
    getOrCreateUniform(UniformId::ShadowViewMatrix) = makeUniform(vm);
}

void LightDrawable::setShadowProjectionMatrix(const glm::mat4x4 &pm)
{
    getOrCreateUniform(UniformId::ShadowProjectionMatrix) = makeUniform(pm);
}

void LightDrawable::setShadowDepthBias(float value)
{
    getOrCreateUniform(UniformId::ShadowDepthBias) = makeUniform(value);
}

const DrawableComponentSet &LightDrawable::componentSet()
{
    static const DrawableComponentSet s_set {
    };
    return s_set;
}


}
}
