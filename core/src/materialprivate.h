#ifndef CORE_MATERIALPRIVATE_H
#define CORE_MATERIALPRIVATE_H

#include <filesystem>

#include <utils/forwarddecl.h>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>

#include <core/forwarddecl.h>
#include <core/material.h>

namespace simplex
{
namespace core
{

class MaterialMapHandler;

class MaterialMapPrivate
{
public:
    MaterialMapPrivate();
    ~MaterialMapPrivate();

    std::filesystem::path& path();
    std::shared_ptr<const utils::Image>& image();

    std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<MaterialMapHandler>> &handles();
    void onChanged();

private:
    std::filesystem::path m_path;
    std::shared_ptr<const utils::Image> m_image;

    std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<MaterialMapHandler>> m_handles;
};

class MaterialHandler;

class MaterialPrivate
{
public:
    MaterialPrivate();
    ~MaterialPrivate();

    bool &isLighted();
    bool &isShadowed();
    bool &isShadowCasted();
    bool &isDoubleSided();

    glm::u32vec3& ORMSwizzleMask();
    glm::vec4 &baseColor();
    glm::vec3& emission();
    float &occlusionMapStrength();
    float &roughness();
    float &metalness();
    float &normalMapScale();

    std::array<std::shared_ptr<const MaterialMap>, numElementsMaterialMapTarget()>& maps();

    std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<MaterialHandler>> &handles();
    void onChanged();

private:
    bool m_isLighted;
    bool m_isShadowed;
    bool m_isShadowCasted;
    bool m_isDoubleSided;

    glm::u32vec3 m_ORMSwizzleMask;
    glm::vec4 m_baseColor;
    glm::vec3 m_emission;
    float m_occlusionMapStrength;
    float m_roughness;
    float m_metalness;
    float m_normalMapScale;

    std::array<std::shared_ptr<const MaterialMap>, numElementsMaterialMapTarget()> m_maps;

    std::unordered_map<std::shared_ptr<Scene>, std::shared_ptr<MaterialHandler>> m_handles;
};

}
}

#endif // CORE_MATERIALPRIVATE_H
