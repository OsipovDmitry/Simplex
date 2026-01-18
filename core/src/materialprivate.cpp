#include <core/settings.h>
#include <core/scene.h>

#include "materialprivate.h"
#include "scenedata.h"


namespace simplex
{
namespace core
{

MaterialMapPrivate::MaterialMapPrivate()
{
}

MaterialMapPrivate::~MaterialMapPrivate() = default;

std::filesystem::path& MaterialMapPrivate::path()
{
    return m_path;
}

std::shared_ptr<utils::Image>& MaterialMapPrivate::image()
{
    return m_image;
}

std::set<std::shared_ptr<MaterialMapHandler>>& MaterialMapPrivate::handlers()
{
    return m_handlers;
}

void MaterialMapPrivate::onChanged()
{
    for (auto& handler : m_handlers)
        if (auto sceneData = handler->sceneData().lock())
            sceneData->onMaterialMapChanged(handler->ID(), m_path, m_image);
}

MaterialPrivate::MaterialPrivate()
    : m_isLighted(true)
    , m_isShadowed(true)
    , m_isShadowCasted(true)
    , m_isDoubleSided(true)
    , m_ORMSwizzleMask(settings::Settings::instance().graphics().pbr().ORMSwizzleMask())
    , m_baseColor(settings::Settings::instance().graphics().pbr().baseColor())
    , m_emission(settings::Settings::instance().graphics().pbr().emission())
    , m_occlusionMapStrength(settings::Settings::instance().graphics().pbr().occlusionMapStrength())
    , m_roughness(settings::Settings::instance().graphics().pbr().roughness())
    , m_metalness(settings::Settings::instance().graphics().pbr().metalness())
    , m_normalMapScale(settings::Settings::instance().graphics().pbr().normalMapScale())
    , m_alphaCutoff(0.f)
{
}

MaterialPrivate::~MaterialPrivate() = default;

bool& MaterialPrivate::isLighted()
{
    return m_isLighted;
}

bool& MaterialPrivate::isShadowed()
{
    return m_isShadowed;
}

bool& MaterialPrivate::isShadowCasted()
{
    return m_isShadowCasted;
}

bool& MaterialPrivate::isDoubleSided()
{
    return m_isDoubleSided;
}

glm::u32vec3& MaterialPrivate::ORMSwizzleMask()
{
    return m_ORMSwizzleMask;
}

glm::vec4& MaterialPrivate::baseColor()
{
    return m_baseColor;
}

glm::vec3& MaterialPrivate::emission()
{
    return m_emission;
}

float& MaterialPrivate::occlusionMapStrength()
{
    return m_occlusionMapStrength;
}

float& MaterialPrivate::roughness()
{
    return m_roughness;
}

float& MaterialPrivate::metalness()
{
    return m_metalness;
}

float& MaterialPrivate::normalMapScale()
{
    return m_normalMapScale;
}

float& MaterialPrivate::alphaCutoff()
{
    return m_alphaCutoff;
}

std::array<std::shared_ptr<MaterialMap>, numElementsMaterialMapTarget()>& MaterialPrivate::maps()
{
    return m_maps;
}

std::set<std::shared_ptr<MaterialHandler>>& MaterialPrivate::handlers()
{
    return m_handlers;
}

void MaterialPrivate::onChanged()
{
    for (auto& handler : m_handlers)
        if (auto sceneData = handler->sceneData().lock())
            sceneData->onMaterialChanged(
                handler->ID(),
                m_baseColor,
                m_emission,
                m_roughness,
                m_metalness,
                m_alphaCutoff,
                m_maps[castFromMaterialMapTarget(MaterialMapTarget::BaseColor)],
                m_maps[castFromMaterialMapTarget(MaterialMapTarget::Emission)],
                m_maps[castFromMaterialMapTarget(MaterialMapTarget::Occlusion)],
                m_maps[castFromMaterialMapTarget(MaterialMapTarget::Roughness)],
                m_maps[castFromMaterialMapTarget(MaterialMapTarget::Metalness)],
                m_maps[castFromMaterialMapTarget(MaterialMapTarget::Normal)],
                m_occlusionMapStrength,
                m_normalMapScale,
                m_ORMSwizzleMask,
                m_isLighted,
                m_isShadowed,
                m_isShadowCasted,
                m_isDoubleSided);
}

}
}
