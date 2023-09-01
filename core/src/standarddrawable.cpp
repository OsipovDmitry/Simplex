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

StandardDrawable::StandardDrawable(std::shared_ptr<graphics::IVertexArray> vao,
                                   const glm::vec4& baseColor,
                                   float metalness,
                                   float roughness,
                                   graphics::PTexture baseColorMap,
                                   graphics::PTexture metalnessMap,
                                   graphics::PTexture roughnessMap,
                                   graphics::PTexture normalMap)
    : DrawableBase(std::make_unique<StandardDrawablePrivate>(vao))
{
    getOrCreateUniform(graphics::UniformId::BaseColor) = makeUniform<glm::vec4>(baseColor);
    getOrCreateUniform(graphics::UniformId::Metalness) = makeUniform<float>(metalness);
    getOrCreateUniform(graphics::UniformId::Roughness) = makeUniform<float>(roughness);

    if (baseColorMap)
        getOrCreateUniform(graphics::UniformId::BaseColorMap) = makeUniform<graphics::PTexture>(baseColorMap);

    if (metalnessMap)
        getOrCreateUniform(graphics::UniformId::MetalnessMap) = makeUniform<graphics::PTexture>(metalnessMap);

    if (roughnessMap)
        getOrCreateUniform(graphics::UniformId::RoughnessMap) = makeUniform<graphics::PTexture>(roughnessMap);

    if (normalMap)
        getOrCreateUniform(graphics::UniformId::NormalMap) = makeUniform<graphics::PTexture>(normalMap);
}

bool StandardDrawable::isTransparent() const
{
    auto result = DrawableBase::isTransparent();

    result = result || (baseColor().a < 1.f - glm::epsilon<float>());

    if (auto map = baseColorMap(); map)
        result = result || map->hasAlpha();

    return result;
}

StandardDrawable::~StandardDrawable() = default;

const glm::vec4 &StandardDrawable::baseColor() const
{
    return uniform_cast<glm::vec4>(uniform(graphics::UniformId::BaseColor))->data();
}

void StandardDrawable::setBaseColor(const glm::vec4 &value)
{
    uniform_cast<glm::vec4>(uniform(graphics::UniformId::BaseColor))->data() = value;
}

float StandardDrawable::metalness() const
{
    return uniform_cast<float>(uniform(graphics::UniformId::Metalness))->data();
}

void StandardDrawable::setMetalness(float value)
{
    uniform_cast<float>(uniform(graphics::UniformId::Metalness))->data() = value;
}

float StandardDrawable::roughness() const
{
    return uniform_cast<float>(uniform(graphics::UniformId::Roughness))->data();
}

void StandardDrawable::setRoughness(float value)
{
    uniform_cast<float>(uniform(graphics::UniformId::Roughness))->data() = value;
}

graphics::PTexture StandardDrawable::baseColorMap() const
{
    auto uni = uniform_cast<graphics::PTexture>(uniform(graphics::UniformId::BaseColorMap));
    return uni ? uni->data() : nullptr;
}

graphics::PTexture StandardDrawable::metalnessMap() const
{
    auto uni = uniform_cast<graphics::PTexture>(uniform(graphics::UniformId::MetalnessMap));
    return uni ? uni->data() : nullptr;
}

graphics::PTexture StandardDrawable::roughnessMap() const
{
    auto uni = uniform_cast<graphics::PTexture>(uniform(graphics::UniformId::RoughnessMap));
    return uni ? uni->data() : nullptr;
}

graphics::PTexture StandardDrawable::normalMap() const
{
    auto uni = uniform_cast<graphics::PTexture>(uniform(graphics::UniformId::NormalMap));
    return uni ? uni->data() : nullptr;
}

}
}
