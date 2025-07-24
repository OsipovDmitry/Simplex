#include <utils/glm/ext/scalar_constants.hpp>

#include <core/settings.h>
#include <core/pbrdrawable.h>
#include <core/uniform.h>

#include "visualdrawableprivate.h"

//
#include <core/mesh.h>
#include <core/material.h>
#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/texturesmanager.h>

namespace simplex
{
namespace core
{

PBRDrawable::PBRDrawable(const std::shared_ptr<graphics::IVertexArray> &vao, const utils::BoundingBox &bb)
    : VisualDrawable(std::make_unique<VisualDrawablePrivate>(vao, bb))
{
    setLighted(true);
    setShadowed(true);
    setShadowCasted(true);
    setORMSwizzleMask(settings::Settings::instance().graphics().pbr().ORMSwizzleMask());
}

PBRDrawable::PBRDrawable(
    const std::shared_ptr<const Mesh>& mesh,
    const std::shared_ptr<const Material>& material,
    const utils::BoundingBox& bb)
    : VisualDrawable(std::make_unique<VisualDrawablePrivate>(mesh, material, bb))
{

    //tmp
    auto renderer = graphics::RendererBase::current();
    auto widget = renderer->widget();
    auto engine = widget->graphicsEngine();
    auto manager = engine->texturesManager();
    auto loadTexture = [&manager](const std::shared_ptr<const MaterialMap>& map) -> graphics::PTexture {
        if (!map)
            return nullptr;
        else if (auto path = map->asFilesystemPath(); !path.empty())
            return manager->loadOrGetTexture(path);
        else if (auto image = map->asImage(); image)
            return manager->loadOrGetTexture(image);
        else return nullptr;
    };
    m().vertexArray() = renderer->createVertexArray(mesh->mesh());
    setLighted(material->isLighted());
    setShadowed(material->isShadowed());
    setShadowCasted(material->isShadowCasted());
    setORMSwizzleMask(material->ORMSwizzleMask());
    setBaseColor(material->baseColor());
    setBaseColorMap(loadTexture(material->materialMap(MaterialMapTarget::BaseColor)));
    setEmission(material->emission());
    setEmissionMap(loadTexture(material->materialMap(MaterialMapTarget::Emission)));
    setOcclusionMap(loadTexture(material->materialMap(MaterialMapTarget::Occlusion)));
    setOcclusionMapStrength(material->occlusionMapStrength());
    setRoughness(material->roughness());
    setRoughnessMap(loadTexture(material->materialMap(MaterialMapTarget::Roughness)));
    setMetalness(material->metalness());
    setMetalnessMap(loadTexture(material->materialMap(MaterialMapTarget::Metalness)));
    setNormalMap(loadTexture(material->materialMap(MaterialMapTarget::Normal)));
    setNormalMapScale(material->normalMapScale());
    //

    setLighted(true);
    setShadowed(true);
    setShadowCasted(true);
    setORMSwizzleMask(settings::Settings::instance().graphics().pbr().ORMSwizzleMask());

}

PBRDrawable::~PBRDrawable() = default;

void PBRDrawable::setLighted(bool value)
{
    setLightedFlag(value);
}

void PBRDrawable::setShadowed(bool value)
{
    setShadowedFlag(value);
}

void PBRDrawable::setShadowCasted(bool value)
{
    setShadowCastedFlag(value);
}

const glm::u32vec3 &PBRDrawable::ORMSwizzleMask() const
{
    auto uni = uniform_cast<glm::u32vec3>(uniform(UniformId::ORMSwizzleMask));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().ORMSwizzleMask();
}

void PBRDrawable::setORMSwizzleMask(const glm::u32vec3 &value)
{
    getOrCreateUniform(UniformId::ORMSwizzleMask) = makeUniform(value);
}

const glm::vec4 &PBRDrawable::baseColor() const
{
    auto uni = uniform_cast<glm::vec4>(uniform(UniformId::BaseColor));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().baseColor();
}

void PBRDrawable::setBaseColor(const glm::vec4 &value)
{
    getOrCreateUniform(UniformId::BaseColor) = makeUniform(value);
}

graphics::PConstTexture PBRDrawable::baseColorMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::BaseColorMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setBaseColorMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::BaseColorMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

const glm::vec3 &PBRDrawable::emission() const
{
    auto uni = uniform_cast<glm::vec3>(uniform(UniformId::Emission));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().emission();
}

void PBRDrawable::setEmission(const glm::vec3 &value)
{
    getOrCreateUniform(UniformId::Emission) = makeUniform(value);
}

graphics::PConstTexture PBRDrawable::emissionMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::EmissionMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setEmissionMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::EmissionMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

graphics::PConstTexture PBRDrawable::occlusionMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::OcclusionMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setOcclusionMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::OcclusionMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

float PBRDrawable::occlusionMapStrength() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::OcclusionMapStrength));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().occlusionMapStrength();
}

void PBRDrawable::setOcclusionMapStrength(float value)
{
    getOrCreateUniform(UniformId::OcclusionMapStrength) = makeUniform(value);
}

float PBRDrawable::roughness() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::Roughness));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().roughness();
}

void PBRDrawable::setRoughness(float value)
{
    getOrCreateUniform(UniformId::Roughness) = makeUniform(value);
}

graphics::PConstTexture PBRDrawable::roughnessMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::RoughnessMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setRoughnessMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::RoughnessMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

float PBRDrawable::metalness() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::Metalness));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().metalness();
}

void PBRDrawable::setMetalness(float value)
{
    getOrCreateUniform(UniformId::Metalness) = makeUniform(value);
}

graphics::PConstTexture PBRDrawable::metalnessMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::MetalnessMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setMetalnessMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::MetalnessMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

graphics::PConstTexture PBRDrawable::normalMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::NormalMap));
    return uni ? uni->data() : nullptr;
}

void PBRDrawable::setNormalMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::NormalMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

float PBRDrawable::normalMapScale() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::NormalMapScale));
    return uni ? uni->data() : settings::Settings::instance().graphics().pbr().normalMapScale();
}

void PBRDrawable::setNormalMapScale(float value)
{
    getOrCreateUniform(UniformId::NormalMapScale) = makeUniform(value);
}

}
}
