#include <core/settings.h>
#include <core/scene.h>

#include "materialprivate.h"
#include "sceneprivate.h"


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

std::shared_ptr<const utils::Image>& MaterialMapPrivate::image()
{
    return m_image;
}

std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<MaterialMapHandler>>& MaterialMapPrivate::handles()
{
    return m_handles;
}

void MaterialMapPrivate::onChanged()
{
    for (auto& handle : m_handles)
        handle.first->m().onMaterialMapChanged(handle.second->materialMap().lock(), handle.second->ID());
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

std::array<std::shared_ptr<const MaterialMap>, numElementsMaterialMapTarget()>& MaterialPrivate::maps()
{
    return m_maps;
}

std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<MaterialHandler>>& MaterialPrivate::handles()
{
    return m_handles;
}

void MaterialPrivate::onChanged()
{
    for (auto& handle : m_handles)
        handle.first->m().onMaterialChanged(handle.second->material().lock(), handle.second->ID());
}

}
}
