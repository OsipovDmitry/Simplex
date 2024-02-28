#include <utils/glm/ext/scalar_constants.hpp>

#include <core/igraphicsrenderer.h>
#include <core/standarddrawable.h>
#include <core/uniform.h>

#include "standarddrawableprivate.h"

namespace simplex
{
namespace core
{

StandardDrawable::StandardDrawable(const std::shared_ptr<graphics::IVertexArray> &vao, const utils::BoundingBox &bb)
    : Drawable(std::make_unique<StandardDrawablePrivate>(vao, bb))
{
    setORMSwizzleMask(StandardDrawablePrivate::defaultORMSwizzleMask());
}

const utils::BoundingBox &StandardDrawable::boundingBox() const
{
    return m().boundingBox();
}

StandardDrawable::~StandardDrawable() = default;

DrawableAlphaMode StandardDrawable::alphaMode() const
{
    auto result = Drawable::alphaMode();

    if (result == DrawableAlphaMode::Opaque) // it's possible to increase alphamode up to tranparent if only it is opaque
    {
        auto isTransparent = false;
        isTransparent = isTransparent || (baseColor().a < 1.f - glm::epsilon<float>());
        if (auto map = baseColorMap(); map)
            isTransparent = isTransparent || map->hasAlpha();

        if (isTransparent)
            result = DrawableAlphaMode::Transparent;
    }

    return result;
}

const glm::u32vec4 &StandardDrawable::ORMSwizzleMask() const
{
    auto uni = uniform_cast<glm::u32vec4>(uniform(graphics::UniformId::ORMSwizzleMask));
    return uni ? uni->data() : StandardDrawablePrivate::defaultORMSwizzleMask();
}

void StandardDrawable::setORMSwizzleMask(const glm::u32vec4 &value)
{
    getOrCreateUniform(graphics::UniformId::ORMSwizzleMask) = makeUniform(value);
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

float StandardDrawable::normalMapScale() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::NormalMapScale));
    return uni ? uni->data() : StandardDrawablePrivate::defaultNormalMapScale();
}

void StandardDrawable::setNormalMapScale(float value)
{
    getOrCreateUniform(graphics::UniformId::NormalMapScale) = makeUniform(value);
}

graphics::PConstTexture StandardDrawable::occlusionMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::OcclusionMap));
    return uni ? uni->data() : nullptr;
}

void StandardDrawable::setOcclusionMap(const graphics::PConstTexture &value)
{
    getOrCreateUniform(graphics::UniformId::OcclusionMap) = makeUniform(value);
}

float StandardDrawable::occlusionMapStrength() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::OcclusionMapStrength));
    return uni ? uni->data() : StandardDrawablePrivate::defaultOcclusionMapStrength();
}

void StandardDrawable::setOcclusionMapStrength(float value)
{
    getOrCreateUniform(graphics::UniformId::OcclusionMapStrength) = makeUniform(value);
}

graphics::PBRComponentsSet StandardDrawable::PBRComponentsSet(const std::shared_ptr<const Drawable> &drawable)
{
    graphics::PBRComponentsSet result;

    for (uint16_t i = 0u; i < graphics::numElementsPBRComponent(); ++i)
    {
        auto pbrComponent = graphics::castToPBRComponent(i);
        if (auto uniformId = graphics::IProgram::uniformIdByPBRComponent(pbrComponent);
                (uniformId != graphics::UniformId::Undefined) && drawable->uniform(uniformId))
            result.insert(pbrComponent);
    }

    return result;
}
}
}
