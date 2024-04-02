#include <utils/logger.h>

#include <core/backgrounddrawable.h>
#include <core/igraphicsrenderer.h>
#include <core/uniform.h>

#include "backgrounddrawableprivate.h"

namespace simplex
{
namespace core
{

BackgroundDrawable::BackgroundDrawable(const std::shared_ptr<graphics::IVertexArray> &vao)
    : Drawable(std::make_unique<BackgroundDrawablePrivate>(vao))
{
    if (BackgroundDrawablePrivate::defaultColorMap().expired())
        LOG_CRITICAL << "Background default color map is expired";

    setColorMap(BackgroundDrawablePrivate::defaultColorMap().lock());
}

BackgroundDrawable::~BackgroundDrawable() = default;

DrawableAlphaMode BackgroundDrawable::alphaMode() const
{
    return DrawableAlphaMode::Opaque;
}

graphics::PConstTexture BackgroundDrawable::colorMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::BackgroundColorMap));
    return uni ? uni->data() : nullptr;
}

void BackgroundDrawable::setColorMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(graphics::UniformId::BackgroundColorMap) = makeUniform(value);
}

const glm::vec3 &BackgroundDrawable::baseColor() const
{
    auto uni = uniform_cast<glm::vec3>(uniform(graphics::UniformId::BackgroundBaseColor));
    return uni ? uni->data() : BackgroundDrawablePrivate::defaultBaseColor();
}

void BackgroundDrawable::setBaseColor(const glm::vec3 &value)
{
    getOrCreateUniform(graphics::UniformId::BackgroundBaseColor) = makeUniform(value);
}

float BackgroundDrawable::roughness() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::BackgroundRoughness));
    return uni ? uni->data() : BackgroundDrawablePrivate::defaultRoughness();
}

void BackgroundDrawable::setRoughness(float value)
{
    getOrCreateUniform(graphics::UniformId::BackgroundRoughness) = makeUniform(value);
}

std::unordered_set<BackgroundComponent> BackgroundDrawable::backgroundComponentsSet() const
{
    return componentsSet<BackgroundComponent>(uniformIdByBackgroundComponent);
}

graphics::UniformId BackgroundDrawable::uniformIdByBackgroundComponent(BackgroundComponent value)
{
    static const std::unordered_map<BackgroundComponent, graphics::UniformId> s_table {
        { BackgroundComponent::ColorMap, graphics::UniformId::BackgroundColorMap },
        { BackgroundComponent::BaseColor, graphics::UniformId::BackgroundBaseColor },
        { BackgroundComponent::Roughness, graphics::UniformId::BackgroundRoughness },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? graphics::UniformId::Undefined : it->second;
}

}
}
