#include <utils/glm/ext/scalar_constants.hpp>
#include <utils/logger.h>

#include <core/igraphicsrenderer.h>
#include <core/standarddrawable.h>
#include <core/uniform.h>

#include "standarddrawableprivate.h"

namespace simplex
{
namespace core
{

StandardDrawable::StandardDrawable(std::shared_ptr<graphics::IVertexArray> vao)
    : Drawable(std::make_unique<StandardDrawablePrivate>(vao))
{
}

StandardDrawable::~StandardDrawable() = default;

bool StandardDrawable::isTransparent() const
{
    auto result = Drawable::isTransparent();

    result = result || (baseColor().a < 1.f - glm::epsilon<float>());

    if (auto map = baseColorMap(); map)
        result = result || map->hasAlpha();

    return result;
}

const glm::vec4 &StandardDrawable::baseColor() const
{
    auto uni = uniform_cast<glm::vec4>(uniform(graphics::UniformId::BaseColor));
    return uni ? uni->data() : StandardDrawablePrivate::defaultBaseColor();
}

void StandardDrawable::setBaseColor(const glm::vec4 &value)
{
    getOrCreateUniform(graphics::UniformId::BaseColor) = makeUniform(value);
}

float StandardDrawable::metalness() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::Metalness));
    return uni ? uni->data() : StandardDrawablePrivate::defaultMetalness();
}

void StandardDrawable::setMetalness(float value)
{
    getOrCreateUniform(graphics::UniformId::Metalness) = makeUniform(value);
}

float StandardDrawable::roughness() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::Roughness));
    return uni ? uni->data() : StandardDrawablePrivate::defaultRoughness();
}

void StandardDrawable::setRoughness(float value)
{
    getOrCreateUniform(graphics::UniformId::Roughness) = makeUniform(value);
}

graphics::PConstTexture StandardDrawable::baseColorMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::BaseColorMap));
    return uni ? uni->data() : nullptr;
}

void StandardDrawable::setBaseColorMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(graphics::UniformId::BaseColorMap) = makeUniform(value);
}

graphics::PConstTexture StandardDrawable::metalnessMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::MetalnessMap));
    return uni ? uni->data() : nullptr;
}

void StandardDrawable::setMetalnessMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(graphics::UniformId::MetalnessMap) = makeUniform(value);
}

graphics::PConstTexture StandardDrawable::roughnessMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::RoughnessMap));
    return uni ? uni->data() : nullptr;
}

void StandardDrawable::setRoughnessMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(graphics::UniformId::RoughnessMap) = makeUniform(value);
}

graphics::PConstTexture StandardDrawable::normalMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::NormalMap));
    return uni ? uni->data() : nullptr;
}

void StandardDrawable::setNormalMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(graphics::UniformId::NormalMap) = makeUniform(value);
}

}
}
