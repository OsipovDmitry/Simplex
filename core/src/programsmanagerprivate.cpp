#include <utils/mesh.h>

#include <core/igraphicsrenderer.h>

#include "programsmanagerprivate.h"

namespace simplex
{
namespace core
{

const std::string ProgramsManagerPrivate::s_opaqueGeometryPassRenderProgramName = "OpaqueGeometryPass";
const std::string ProgramsManagerPrivate::s_transparentGeometryPassRenderProgramName = "TransparentGeometryPass";
const std::string ProgramsManagerPrivate::s_OITClearPassComputeProgramName = "OITClearPass";
const std::string ProgramsManagerPrivate::s_OITSortNodesPassComputeProgramName = "OITSortNodesPass";
const std::string ProgramsManagerPrivate::s_lightPassRenderProgramName = "LightPass";
const std::string ProgramsManagerPrivate::s_finalPassRenderProgramName = "FinalPass";

const std::filesystem::path ProgramsManagerPrivate::s_geometryPassVertexShaderPath = "D:/res/shaders/deferred_geometry_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_opaqueGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_opaque_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_transparentGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_transparent_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_OITClearPassComputeShaderPath = "D:/res/shaders/oit_clear.comp";
const std::filesystem::path ProgramsManagerPrivate::s_OITSortNodesPassComputeShaderPath = "D:/res/shaders/oit_sort.comp";
const std::filesystem::path ProgramsManagerPrivate::s_lightPassVertexShaderPath = "D:/res/shaders/deferred_light_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_lightPassFragmnetShaderPath = "D:/res/shaders/deferred_light_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassVertexShaderPath = "D:/res/shaders/final.vert";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassFragmnetShaderPath = "D:/res/shaders/final.frag";

ProgramsManagerPrivate::ProgramsManagerPrivate(std::shared_ptr<graphics::IRenderer> renderer)
    : m_renderer(renderer)
{
}

ProgramsManagerPrivate::~ProgramsManagerPrivate() = default;

std::shared_ptr<graphics::IRenderer> &ProgramsManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &ProgramsManagerPrivate::resources()
{
    return m_resources;
}

void ProgramsManagerPrivate::prepareVertexAttributesDefines(const utils::VertexAttributesSet &attribsSet,
                                                            utils::ShaderDefines &defines)
{
    static const std::array<std::string, utils::numElementsVertexAttribute()> s_table {
        "POSITION_ATTRIBUTE",
        "NORMAL_ATTRIBUTE",
        "TEX_COORDS_ATTRIBUTE",
        "BONES_IDS_ATTRIBUTE",
        "BONES_WEIGHTS_ATTRIBUTE",
        "TANGENT_ATTRIBUTE",
        "COLOR_ATTRIBUTE"
    };

    for (uint16_t i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        if (attribsSet.count(utils::castToVertexAttribute(i)))
            defines.insert({s_table[i], std::to_string(i)});
}

uint16_t ProgramsManagerPrivate::prepareVertexAttributesKey(const utils::VertexAttributesSet &attribsSet,
                                                            NameKey &nameKey,
                                                            uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        nameKey.set(firstKeyIndex + i, attribsSet.count(utils::castToVertexAttribute(i)));
    return firstKeyIndex + utils::numElementsVertexAttribute();
}

void ProgramsManagerPrivate::preparePBRComponentsDefines(const graphics::PBRComponentsSet &PBRComponentsSet,
                                                         utils::ShaderDefines &defines)
{
    static const std::array<std::string, graphics::numElementsPBRComponent()> s_table {
        "HAS_BASE_COLOR",
        "HAS_METALNESS",
        "HAS_ROUGHNESS",
        "HAS_BASE_COLOR_MAP",
        "HAS_METALNESS_MAP",
        "HAS_ROUGHNESS_MAP",
        "HAS_NORMAL_MAP",
    };

    for (uint16_t i = 0u; i < graphics::numElementsPBRComponent(); ++i)
        if (PBRComponentsSet.count(graphics::castToPBRComponent(i)))
            defines.insert({s_table[i], ""});
}

uint16_t ProgramsManagerPrivate::preparePBRComponentsKey(const graphics::PBRComponentsSet &PBRComponentsSet,
                                                         NameKey &nameKey,
                                                         uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < graphics::numElementsPBRComponent(); ++i)
        nameKey.set(firstKeyIndex + i, PBRComponentsSet.count(graphics::castToPBRComponent(i)));
    return firstKeyIndex + graphics::numElementsPBRComponent();
}

void ProgramsManagerPrivate::prepareLightComponentsDefines(const graphics::LightComponentsSet &lightComponentsSet,
                                                           utils::ShaderDefines &defines)
{
    static const std::array<std::string, graphics::numElementsLightComponent()> s_table {
        "HAS_COLOR",
        "HAS_RADIUSES",
        "HAS_HALF_ANGLES",
    };

    for (uint16_t i = 0; i < graphics::numElementsLightComponent(); ++i)
        if (lightComponentsSet.count(graphics::castToLightComponent(i)))
            defines.insert({s_table[i], ""});
}

uint16_t ProgramsManagerPrivate::prepareLightComponentsKey(const graphics::LightComponentsSet &lightComponentsSet,
                                                           NameKey &nameKey,
                                                           uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < graphics::numElementsLightComponent(); ++i)
        nameKey.set(firstKeyIndex + i, lightComponentsSet.count(graphics::castToLightComponent(i)));
    return firstKeyIndex + graphics::numElementsLightComponent();
}

void ProgramsManagerPrivate::prepareDefinesAndKeyForGeometryPassRenderProgram(const utils::VertexAttributesSet& attribsSet,
                                                                              const graphics::PBRComponentsSet &PBRComponentsSet,
                                                                              utils::ShaderDefines &defines,
                                                                              NameKey &key)
{
    prepareVertexAttributesDefines(attribsSet, defines);
    preparePBRComponentsDefines(PBRComponentsSet, defines);

    uint16_t bit = 0u;
    bit = prepareVertexAttributesKey(attribsSet, key, bit);
    bit = preparePBRComponentsKey(PBRComponentsSet, key, bit);
}

void ProgramsManagerPrivate::prepareDefinesAndKeyForLightPassRenderProgram(const utils::VertexAttributesSet &attribsSet,
                                                                           const graphics::LightComponentsSet &lightComponentsSet,
                                                                           utils::ShaderDefines &defines,
                                                                           NameKey &key)
{
    prepareVertexAttributesDefines(attribsSet, defines);
    prepareLightComponentsDefines(lightComponentsSet, defines);

    uint16_t bit = 0u;
    bit = prepareVertexAttributesKey(attribsSet, key, bit);
    bit = prepareLightComponentsKey(lightComponentsSet, key, bit);
}

const std::string &ProgramsManagerPrivate::opaqueGeometryPassRenderProgramName()
{
    return s_opaqueGeometryPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::transparentGeometryPassRenderProgramName()
{
    return s_transparentGeometryPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::OITClearPassComputeProgramName()
{
    return s_OITClearPassComputeProgramName;
}

const std::string &ProgramsManagerPrivate::OITSortNodesPassComputeProgramName()
{
    return s_OITSortNodesPassComputeProgramName;
}

const std::string &ProgramsManagerPrivate::lightPassRenderProgramName()
{
    return s_lightPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::finalPassRenderProgramName()
{
    return s_finalPassRenderProgramName;
}

const std::filesystem::path &ProgramsManagerPrivate::geometryPassVertexShaderPath()
{
    return s_geometryPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::opaqueGeometryPassFragmnetShaderPath()
{
    return s_opaqueGeometryPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::transparentGeometryPassFragmnetShaderPath()
{
    return s_transparentGeometryPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::OITClearPassComputeShaderPath()
{
    return s_OITClearPassComputeShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::OITSortNodesPassComputeShaderPath()
{
    return s_OITSortNodesPassComputeShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::lightPassVertexShaderPath()
{
    return s_lightPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::lightPassFragmnetShaderPath()
{
    return s_lightPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::finalPassVertexShaderPath()
{
    return s_finalPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::finalPassFragmnetShaderPath()
{
    return s_finalPassFragmnetShaderPath;
}

}
}
