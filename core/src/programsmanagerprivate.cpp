#include <utils/glm/integer.hpp>
#include <utils/mesh.h>

#include <core/lightnode.h>
#include <core/pbrdrawable.h>
#include <core/lightdrawable.h>
#include <core/backgrounddrawable.h>

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
const std::string ProgramsManagerPrivate::s_shadowRenderProgramName = "ShadowPass";
const std::string ProgramsManagerPrivate::s_stencilPassRenderProgramName = "StencilPass";
const std::string ProgramsManagerPrivate::s_lightPassRenderProgramName = "LightPass";
const std::string ProgramsManagerPrivate::s_backgroundPassRenderProgramName = "BackgroundPass";
const std::string ProgramsManagerPrivate::s_foregroundPassRenderProgramName = "ForegroundPass";
const std::string ProgramsManagerPrivate::s_finalPassRenderProgramName = "FinalPass";
const std::string ProgramsManagerPrivate::s_postprocessPassRenderProgramName = "PostprocessPass";

const std::filesystem::path ProgramsManagerPrivate::s_geometryPassVertexShaderPath = "D:/res/shaders/deferred_geometry_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_opaqueGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_opaque_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_transparentGeometryPassFragmnetShaderPath = "D:/res/shaders/deferred_transparent_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_OITClearPassComputeShaderPath = "D:/res/shaders/oit_clear.comp";
const std::filesystem::path ProgramsManagerPrivate::s_OITSortNodesPassComputeShaderPath = "D:/res/shaders/oit_sort.comp";
const std::filesystem::path ProgramsManagerPrivate::s_shadowVertexShaderPath = "D:/res/shaders/shadow.vert";
const std::filesystem::path ProgramsManagerPrivate::s_shadowGeometryShaderPath = "D:/res/shaders/shadow.geom";
const std::filesystem::path ProgramsManagerPrivate::s_shadowFragmnetShaderPath = "D:/res/shaders/shadow.frag";
const std::filesystem::path ProgramsManagerPrivate::s_stencilPassVertexShaderPath = "D:/res/shaders/deferred_stencil_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_stencilPassFragmnetShaderPath = "D:/res/shaders/deferred_stencil_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_lightPassVertexShaderPath = "D:/res/shaders/deferred_light_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_lightPassFragmnetShaderPath = "D:/res/shaders/deferred_light_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_backgroundPassVertexShaderPath = "D:/res/shaders/background.vert";
const std::filesystem::path ProgramsManagerPrivate::s_backgroundPassFragmnetShaderPath = "D:/res/shaders/background.frag";
const std::filesystem::path ProgramsManagerPrivate::s_foregroundPassVertexShaderPath = "D:/res/shaders/foreground.vert";
const std::filesystem::path ProgramsManagerPrivate::s_foregroundPassFragmnetShaderPath = "D:/res/shaders/foreground.frag";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassVertexShaderPath = "D:/res/shaders/final.vert";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassFragmnetShaderPath = "D:/res/shaders/final.frag";
const std::filesystem::path ProgramsManagerPrivate::s_postprocessPassVertexShaderPath = "D:/res/shaders/postprocess.vert";
const std::filesystem::path ProgramsManagerPrivate::s_postprocessPassFragmnetShaderPath = "D:/res/shaders/postprocess.frag";

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

void ProgramsManagerPrivate::preparePBRComponentsDefines(const std::unordered_set<PBRComponent> &PBRComponentsSet,
                                                         utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsPBRComponent()> s_table {
        "HAS_BASE_COLOR",
        "HAS_BASE_COLOR_MAP",
        "HAS_METALNESS",
        "HAS_METALNESS_MAP",
        "HAS_ROUGHNESS",
        "HAS_ROUGHNESS_MAP",
        "HAS_NORMAL_MAP",
        "HAS_NORMAL_MAP_SCALE",
        "HAS_OCCLUSION_MAP",
        "HAS_OCCLUSION_MAP_STRENGTH",
    };

    for (uint16_t i = 0u; i < numElementsPBRComponent(); ++i)
        if (PBRComponentsSet.count(castToPBRComponent(i)))
            defines.insert({s_table[i], ""});
}

uint16_t ProgramsManagerPrivate::preparePBRComponentsKey(const std::unordered_set<PBRComponent> &PBRComponentsSet,
                                                         NameKey &nameKey,
                                                         uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < numElementsPBRComponent(); ++i)
        nameKey.set(firstKeyIndex + i, PBRComponentsSet.count(castToPBRComponent(i)));
    return firstKeyIndex + numElementsPBRComponent();
}

void ProgramsManagerPrivate::prepareLightShadingModeDefines(LightShadingMode shadingMode,
                                                            utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsLightShadingMode()> s_typesTable {
        "SHADING_MODE_DISABLED",
        "SHADING_MODE_OPAQUE",
        "SHADING_MODE_OPAQUE_AND_TRANSPARENT",
        "SHADING_MODE_COLOR"
    };

    defines.insert({s_typesTable[castFromLightShadingMode(shadingMode)], ""});
}

uint16_t ProgramsManagerPrivate::prepareLightShadingModeKey(LightShadingMode shadingMode,
                                                            NameKey &nameKey,
                                                            uint16_t firstKeyIndex)
{
    static_assert(numElementsLightShadingMode() > 0u);
    static const auto typeBitsCount = glm::findMSB(numElementsLightShadingMode());
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(castFromLightShadingMode(shadingMode), i, 1u));

    firstKeyIndex += typeBitsCount;

    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareLightTypeDefines(LightType lightType,
                                                     utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsLightType()> s_typesTable {
        "POINT_LIGHT_TYPE",
        "SPOT_LIGHT_TYPE",
        "DIRECTIONAL_LIGHT_TYPE",
        "IBL_TYPE"
    };

    defines.insert({s_typesTable[castFromLightType(lightType)], ""});
}

uint16_t ProgramsManagerPrivate::prepareLightTypeKey(LightType lightType,
                                                     NameKey &nameKey,
                                                     uint16_t firstKeyIndex)
{
    static_assert(numElementsLightType() > 0u);
    static const auto typeBitsCount = glm::findMSB(numElementsLightType());
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(castFromLightType(lightType), i, 1u));
    firstKeyIndex += typeBitsCount;

    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareBackgroundComponentsDefines(const std::unordered_set<BackgroundComponent> &backgroundComponentsSet,
                                                                utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsBackgroundComponent()> s_table {
        "HAS_BACKGROUND_COLOR_MAP",
        "HAS_BACKGROUND_BASE_COLOR",
        "HAS_BACKGROUND_ROUGHNESS",
    };

    for (uint16_t i = 0u; i < numElementsBackgroundComponent(); ++i)
        if (backgroundComponentsSet.count(castToBackgroundComponent(i)))
            defines.insert({s_table[i], ""});
}

uint16_t ProgramsManagerPrivate::prepareBackgroundComponentsKey(const std::unordered_set<BackgroundComponent> &backgroundComponentsSet,
                                                                NameKey &nameKey,
                                                                uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < numElementsBackgroundComponent(); ++i)
        nameKey.set(firstKeyIndex + i, backgroundComponentsSet.count(castToBackgroundComponent(i)));
    return firstKeyIndex + numElementsBackgroundComponent();
}

void ProgramsManagerPrivate::prepareLightComponentsDefines(const std::unordered_set<LightComponent> &lightComponentSet,
                                                           utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsLightComponent()> s_table {
        "NONE",
    };

    for (uint16_t i = 0u; i < numElementsLightComponent(); ++i)
        if (lightComponentSet.count(castToLightComponent(i)))
            defines.insert({s_table[i], ""});
}

uint16_t ProgramsManagerPrivate::prepareLightComponentsKey(const std::unordered_set<LightComponent> &lightComponentSet,
                                                           NameKey &nameKey,
                                                           uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < numElementsLightComponent(); ++i)
        nameKey.set(firstKeyIndex + i, lightComponentSet.count(castToLightComponent(i)));
    return firstKeyIndex + numElementsLightComponent();
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

const std::string &ProgramsManagerPrivate::shadowRenderProgramName()
{
    return s_shadowRenderProgramName;
}

const std::string &ProgramsManagerPrivate::stencilPassRenderProgramName()
{
    return s_stencilPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::lightPassRenderProgramName()
{
    return s_lightPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::backgroundPassRenderProgramName()
{
    return s_backgroundPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::foregroundPassRenderProgramName()
{
    return s_foregroundPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::finalPassRenderProgramName()
{
    return s_finalPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::postprocessPassRenderProgramName()
{
    return s_postprocessPassRenderProgramName;
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

const std::filesystem::path &ProgramsManagerPrivate::shadowVertexShaderPath()
{
    return s_shadowVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::shadowGeometryShaderPath()
{
    return s_shadowGeometryShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::shadowFragmnetShaderPath()
{
    return s_shadowFragmnetShaderPath;
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

const std::filesystem::path &ProgramsManagerPrivate::backgroundPassVertexShaderPath()
{
    return s_backgroundPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::backgroundPassFragmnetShaderPath()
{
    return s_backgroundPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::foregroundPassVertexShaderPath()
{
    return s_foregroundPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::foregroundPassFragmnetShaderPath()
{
    return s_foregroundPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::finalPassVertexShaderPath()
{
    return s_finalPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::finalPassFragmnetShaderPath()
{
    return s_finalPassFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::postprocessPassVertexShaderPath()
{
    return s_postprocessPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::postprocessPassFragmnetShaderPath()
{
    return s_postprocessPassFragmnetShaderPath;
}

}
}
