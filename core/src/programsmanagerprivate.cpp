#include <utils/glm/integer.hpp>
#include <utils/mesh.h>

#include <core/igraphicsrenderer.h>
#include <core/lightdrawable.h>

#include "programsmanagerprivate.h"

namespace simplex
{
namespace core
{

float ProgramsManagerPrivate::s_dielectricSpecular = 0.04f;

const std::string ProgramsManagerPrivate::s_opaqueGeometryPassRenderProgramName = "OpaqueGeometryPass";
const std::string ProgramsManagerPrivate::s_transparentGeometryPassRenderProgramName = "TransparentGeometryPass";
const std::string ProgramsManagerPrivate::s_OITClearPassComputeProgramName = "OITClearPass";
const std::string ProgramsManagerPrivate::s_OITSortNodesPassComputeProgramName = "OITSortNodesPass";
const std::string ProgramsManagerPrivate::s_backgroundPassRenderProgramName = "BackgroundPass";
const std::string ProgramsManagerPrivate::s_stencilPassRenderProgramName = "StencilPass";
const std::string ProgramsManagerPrivate::s_lightPassRenderProgramName = "LightPass";
const std::string ProgramsManagerPrivate::s_finalPassRenderProgramName = "FinalPass";

const std::filesystem::path ProgramsManagerPrivate::s_geometryPassVertexShaderPath = "D:/res/shaders/deferred_geometry_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_opaqueGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_opaque_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_transparentGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_transparent_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_OITClearPassComputeShaderPath = "D:/res/shaders/oit_clear.comp";
const std::filesystem::path ProgramsManagerPrivate::s_OITSortNodesPassComputeShaderPath = "D:/res/shaders/oit_sort.comp";
const std::filesystem::path ProgramsManagerPrivate::s_backgroundPassVertexShaderPath = "D:/res/shaders/background.vert";
const std::filesystem::path ProgramsManagerPrivate::s_backgroundPassFragmnetShaderPath = "D:/res/shaders/background.frag";
const std::filesystem::path ProgramsManagerPrivate::s_stencilPassVertexShaderPath = "D:/res/shaders/deferred_stencil_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_stencilPassFragmnetShaderPath = "D:/res/shaders/deferred_stencil_pass.frag";
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

float &ProgramsManagerPrivate::dielectricSpecular()
{
    return s_dielectricSpecular;
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

void ProgramsManagerPrivate::prepareBackgroundComponentsDefines(const graphics::BackgroundComponentsSet &backgroundComponentsSet,
                                                                utils::ShaderDefines &defines)
{
    static const std::array<std::string, graphics::numElementsBackgroundComponent()> s_table {
        "HAS_BACKGROUND_COLOR_MAP",
    };

    for (uint16_t i = 0; i < graphics::numElementsBackgroundComponent(); ++i)
        if (backgroundComponentsSet.count(graphics::castToBackgroundComponent(i)))
            defines.insert({s_table[i], ""});
}

uint16_t ProgramsManagerPrivate::prepareBackgrounfComponentsKey(const graphics::BackgroundComponentsSet &backgroundComponentsSet,
                                                                NameKey &nameKey,
                                                                uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < graphics::numElementsBackgroundComponent(); ++i)
        nameKey.set(firstKeyIndex + i, backgroundComponentsSet.count(graphics::castToBackgroundComponent(i)));
    return firstKeyIndex + graphics::numElementsBackgroundComponent();
}

void ProgramsManagerPrivate::prepareLightComponentsDefines(const graphics::LightComponentsSet &lightComponentsSet,
                                                           LightDrawableType lightDrawableType,
                                                           utils::ShaderDefines &defines)
{
    static const std::array<std::string, graphics::numElementsLightComponent()> s_componentsTable {
    };

    for (uint16_t i = 0; i < graphics::numElementsLightComponent(); ++i)
        if (lightComponentsSet.count(graphics::castToLightComponent(i)))
            defines.insert({s_componentsTable[i], ""});

    static const std::array<std::string, numElementsLightDrawableType()> s_typesTable {
        "POINT_LIGHT_TYPE",
        "SPOT_LIGHT_TYPE",
        "DIRECTIONAL_LIGHT_TYPE",
        "IBL_TYPE"
    };

    defines.insert({s_typesTable[castFromLightDrawableType(lightDrawableType)], ""});
}

uint16_t ProgramsManagerPrivate::prepareLightComponentsKey(const graphics::LightComponentsSet &lightComponentsSet,
                                                           LightDrawableType lightDrawableType,
                                                           NameKey &nameKey,
                                                           uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < graphics::numElementsLightComponent(); ++i)
        nameKey.set(firstKeyIndex + i, lightComponentsSet.count(graphics::castToLightComponent(i)));

    firstKeyIndex += graphics::numElementsLightComponent();

    static_assert(numElementsLightDrawableType() > 0u);
    static const auto typeBitsCount = glm::findMSB(numElementsLightDrawableType());
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(castFromLightDrawableType(lightDrawableType), i, 1u));

    firstKeyIndex += numElementsLightDrawableType();

    return firstKeyIndex;
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

void ProgramsManagerPrivate::prepareDefinesAndKeyForBackgroundPassRenderProgram(const utils::VertexAttributesSet &attribsSet,
                                                                                const graphics::BackgroundComponentsSet &backgroundComponentsSet,
                                                                                utils::ShaderDefines &defines,
                                                                                NameKey &key)
{
    prepareVertexAttributesDefines(attribsSet, defines);
    prepareBackgroundComponentsDefines(backgroundComponentsSet, defines);

    uint16_t bit = 0u;
    bit = prepareVertexAttributesKey(attribsSet, key, bit);
    bit = prepareBackgrounfComponentsKey(backgroundComponentsSet, key, bit);
}

void ProgramsManagerPrivate::prepareDefinesAndKeyForLightPassRenderProgram(const utils::VertexAttributesSet &attribsSet,
                                                                           const graphics::LightComponentsSet &lightComponentsSet,
                                                                           LightDrawableType lightDrawableType,
                                                                           utils::ShaderDefines &defines,
                                                                           NameKey &key)
{
    prepareVertexAttributesDefines(attribsSet, defines);
    prepareLightComponentsDefines(lightComponentsSet, lightDrawableType, defines);
    defines.insert({"DIELECTRIC_SPECULAR", std::to_string(s_dielectricSpecular)});

    uint16_t bit = 0u;
    bit = prepareVertexAttributesKey(attribsSet, key, bit);
    bit = prepareLightComponentsKey(lightComponentsSet, lightDrawableType, key, bit);
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

const std::string &ProgramsManagerPrivate::backgroundPassRenderProgramName()
{
    return s_backgroundPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::stencilPassRenderProgramName()
{
    return s_stencilPassRenderProgramName;
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

const std::filesystem::path &ProgramsManagerPrivate::backgroundPassVertexShaderPath()
{
    return s_backgroundPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::backgroundPassFragmnetShaderPath()
{
    return s_backgroundPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::stencilPassVertexShaderPath()
{
    return s_stencilPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::stencilPassFragmnetShaderPath()
{
    return s_stencilPassFragmnetShaderPath;
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
