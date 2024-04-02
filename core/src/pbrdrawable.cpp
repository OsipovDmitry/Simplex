#include <utils/glm/ext/scalar_constants.hpp>

#include <core/igraphicsrenderer.h>
#include <core/pbrdrawable.h>
#include <core/uniform.h>

#include "pbrdrawableprivate.h"

namespace simplex
{
namespace core
{

PBRDrawable::PBRDrawable(const std::shared_ptr<graphics::IVertexArray> &vao, const utils::BoundingBox &bb)
    : Drawable(std::make_unique<PBRDrawablePrivate>(vao, bb))
{
    setORMSwizzleMask(PBRDrawablePrivate::defaultORMSwizzleMask());
}

const utils::BoundingBox &PBRDrawable::boundingBox() const
{
    return m().boundingBox();
}

PBRDrawable::~PBRDrawable() = default;

DrawableAlphaMode PBRDrawable::alphaMode() const
{
    auto result = Drawable::alphaMode();

    if (baseColor().a < 1.f - glm::epsilon<float>())
        result = DrawableAlphaMode::Transparent;

    if (auto map = baseColorMap(); map)
        if (map->hasAlpha())
            result = DrawableAlphaMode::Transparent;

    return result;
}

const glm::u32vec4 &PBRDrawable::ORMSwizzleMask() const
{
    auto uni = uniform_cast<glm::u32vec4>(uniform(graphics::UniformId::ORMSwizzleMask));
    return uni ? uni->data() : PBRDrawablePrivate::defaultORMSwizzleMask();
}

void PBRDrawable::setORMSwizzleMask(const glm::u32vec4 &value)
{
    getOrCreateUniform(graphics::UniformId::ORMSwizzleMask) = makeUniform(value);
}

const glm::vec4 &PBRDrawable::baseColor() const
{
    auto uni = uniform_cast<glm::vec4>(uniform(graphics::UniformId::BaseColor));
    return uni ? uni->data() : PBRDrawablePrivate::defaultBaseColor();
}

void PBRDrawable::setBaseColor(const glm::vec4 &value)
{
    getOrCreateUniform(graphics::UniformId::BaseColor) = makeUniform(value);
}

float PBRDrawable::metalness() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::Metalness));
    return uni ? uni->data() : PBRDrawablePrivate::defaultMetalness();
}

void PBRDrawable::setMetalness(float value)
{
    getOrCreateUniform(graphics::UniformId::Metalness) = makeUniform(value);
}

float PBRDrawable::roughness() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::Roughness));
    return uni ? uni->data() : PBRDrawablePrivate::defaultRoughness();
}

void PBRDrawable::setRoughness(float value)
{
    getOrCreateUniform(graphics::UniformId::Roughness) = makeUniform(value);
}

graphics::PConstTexture PBRDrawable::baseColorMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::BaseColorMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setBaseColorMap(const graphics::PConstTexture &value)
{
    static graphics::UniformId s_uniformId = graphics::UniformId::BaseColorMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

graphics::PConstTexture PBRDrawable::metalnessMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::MetalnessMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setMetalnessMap(const graphics::PConstTexture &value)
{
    static graphics::UniformId s_uniformId = graphics::UniformId::MetalnessMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

graphics::PConstTexture PBRDrawable::roughnessMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::RoughnessMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setRoughnessMap(const graphics::PConstTexture &value)
{
    static graphics::UniformId s_uniformId = graphics::UniformId::RoughnessMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

graphics::PConstTexture PBRDrawable::normalMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::NormalMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setNormalMap(const graphics::PConstTexture &value)
{
    static graphics::UniformId s_uniformId = graphics::UniformId::NormalMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

float PBRDrawable::normalMapScale() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::NormalMapScale));
    return uni ? uni->data() : PBRDrawablePrivate::defaultNormalMapScale();
}

void PBRDrawable::setNormalMapScale(float value)
{
    getOrCreateUniform(graphics::UniformId::NormalMapScale) = makeUniform(value);
}

graphics::PConstTexture PBRDrawable::occlusionMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(graphics::UniformId::OcclusionMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setOcclusionMap(const graphics::PConstTexture &value)
{
    static graphics::UniformId s_uniformId = graphics::UniformId::OcclusionMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

float PBRDrawable::occlusionMapStrength() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::OcclusionMapStrength));
    return uni ? uni->data() : PBRDrawablePrivate::defaultOcclusionMapStrength();
}

void PBRDrawable::setOcclusionMapStrength(float value)
{
    getOrCreateUniform(graphics::UniformId::OcclusionMapStrength) = makeUniform(value);
}

std::unordered_set<PBRComponent> PBRDrawable::PBRComponentsSet(const std::shared_ptr<const Drawable> &drawable)
{
    return drawable->componentsSet<PBRComponent>(uniformIdByPBRComponent);
}

graphics::UniformId PBRDrawable::uniformIdByPBRComponent(PBRComponent value)
{
    static const std::unordered_map<PBRComponent, graphics::UniformId> s_table {
        { PBRComponent::BaseColor, graphics::UniformId::BaseColor },
        { PBRComponent::BaseColorMap, graphics::UniformId::BaseColorMap },
        { PBRComponent::Metalness, graphics::UniformId::Metalness },
        { PBRComponent::MetalnessMap, graphics::UniformId::MetalnessMap },
        { PBRComponent::Roughness, graphics::UniformId::Roughness },
        { PBRComponent::RoughnessMap, graphics::UniformId::RoughnessMap },
        { PBRComponent::NormalMap, graphics::UniformId::NormalMap },
        { PBRComponent::NormalMapScale, graphics::UniformId::NormalMapScale },
        { PBRComponent::OcclusionMap, graphics::UniformId::OcclusionMap },
        { PBRComponent::OcclusionMapStrength, graphics::UniformId::OcclusionMapStrength },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? graphics::UniformId::Undefined : it->second;
}

}
}
