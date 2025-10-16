#include <utils/glm/integer.hpp>
#include <utils/logger.h>
#include <utils/mesh.h>

#include <core/graphicsrendererbase.h>
#include <core/lightnode.h>
#include <core/ssao.h>
#include <core/shadow.h>
#include <core/visualdrawable.h>

#include "programsmanagerprivate.h"
#include "backgrounddrawable.h"
#include "lightdrawable.h"

namespace simplex
{
namespace core
{

const std::string ProgramsManagerPrivate::s_geometryPassRenderProgramName = "GeometryPass";
const std::string ProgramsManagerPrivate::s_stencilPassRenderProgramName = "StencilPass";
const std::string ProgramsManagerPrivate::s_lightPassRenderProgramName = "LightPass";
const std::string ProgramsManagerPrivate::s_OITClearPassComputeProgramName = "OITClearPass";
const std::string ProgramsManagerPrivate::s_OITSortNodesPassComputeProgramName = "OITSortNodesPass";
const std::string ProgramsManagerPrivate::s_SSAORenderProgramName = "SSAOPass";
const std::string ProgramsManagerPrivate::s_blurRenderProgramName = "BlurPass";
const std::string ProgramsManagerPrivate::s_bilaterialBlurRenderProgramName = "BilaterialBlurPass";
const std::string ProgramsManagerPrivate::s_shadowRenderProgramName = "ShadowPass";
const std::string ProgramsManagerPrivate::s_backgroundPassRenderProgramName = "BackgroundPass";
const std::string ProgramsManagerPrivate::s_finalPassRenderProgramName = "FinalPass";
const std::string ProgramsManagerPrivate::s_postprocessPassRenderProgramName = "PostprocessPass";

const std::filesystem::path ProgramsManagerPrivate::s_geometryPassVertexShaderPath = "./resources/shaders/deferred_geometry_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_geometryPassFragmnetShaderPath = "./resources/shaders/deferred_geometry_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_stencilPassVertexShaderPath = "./resources/shaders/deferred_stencil_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_stencilPassFragmnetShaderPath = "./resources/shaders/deferred_stencil_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_lightPassVertexShaderPath = "./resources/shaders/deferred_light_pass.vert";
const std::filesystem::path ProgramsManagerPrivate::s_lightPassFragmnetShaderPath = "./resources/shaders/deferred_light_pass.frag";
const std::filesystem::path ProgramsManagerPrivate::s_OITClearPassComputeShaderPath = "./resources/shaders/oit_clear.comp";
const std::filesystem::path ProgramsManagerPrivate::s_OITSortNodesPassComputeShaderPath = "./resources/shaders/oit_sort.comp";
const std::filesystem::path ProgramsManagerPrivate::s_SSAOVertexShaderPath = "./resources/shaders/ssao.vert";
const std::filesystem::path ProgramsManagerPrivate::s_SSAOFragmnetShaderPath = "./resources/shaders/ssao.frag";
const std::filesystem::path ProgramsManagerPrivate::s_blurVertexShaderPath = "./resources/shaders/blur.vert";
const std::filesystem::path ProgramsManagerPrivate::s_blurFragmnetShaderPath = "./resources/shaders/blur.frag";
const std::filesystem::path ProgramsManagerPrivate::s_blurComputeShaderPath = "./resources/shaders/blur.comp";;
const std::filesystem::path ProgramsManagerPrivate::s_bilaterialBlurVertexShaderPath = "./resources/shaders/bilaterial_blur.vert";
const std::filesystem::path ProgramsManagerPrivate::s_bilaterialBlurFragmnetShaderPath = "./resources/shaders/bilaterial_blur.frag";
const std::filesystem::path ProgramsManagerPrivate::s_shadowVertexShaderPath = "./resources/shaders/shadow.vert";
const std::filesystem::path ProgramsManagerPrivate::s_shadowGeometryShaderPath = "./resources/shaders/shadow.geom";
const std::filesystem::path ProgramsManagerPrivate::s_shadowFragmnetShaderPath = "./resources/shaders/shadow.frag";
const std::filesystem::path ProgramsManagerPrivate::s_backgroundPassVertexShaderPath = "./resources/shaders/background.vert";
const std::filesystem::path ProgramsManagerPrivate::s_backgroundPassFragmnetShaderPath = "./resources/shaders/background.frag";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassVertexShaderPath = "./resources/shaders/final.vert";
const std::filesystem::path ProgramsManagerPrivate::s_finalPassFragmnetShaderPath = "./resources/shaders/final.frag";
const std::filesystem::path ProgramsManagerPrivate::s_postprocessPassVertexShaderPath = "./resources/shaders/postprocess.vert";
const std::filesystem::path ProgramsManagerPrivate::s_postprocessPassFragmnetShaderPath = "./resources/shaders/postprocess.frag";

ProgramsManagerPrivate::ProgramsManagerPrivate(const std::shared_ptr<graphics::RendererBase> &renderer)
    : m_renderer(renderer)
{
}

ProgramsManagerPrivate::~ProgramsManagerPrivate() = default;

std::shared_ptr<graphics::RendererBase> &ProgramsManagerPrivate::renderer()
{
    return m_renderer;
}

std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &ProgramsManagerPrivate::resources()
{
    return m_resources;
}

void ProgramsManagerPrivate::prepareTransparentDefines(bool value, utils::ShaderDefines &defines)
{
    if (value)
        defines.insert({"IS_TRANSPARENT", ""});
}

uint16_t ProgramsManagerPrivate::prepareTransparentKey(bool value, NameKey &nameKey, uint16_t firstKeyIndex)
{
    nameKey.set(firstKeyIndex, value);
    return firstKeyIndex + 1u;
}

void ProgramsManagerPrivate::prepareVertexAttributesDefines(const utils::VertexAttributeSet &attribsSet,
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

uint16_t ProgramsManagerPrivate::prepareVertexAttributesKey(const utils::VertexAttributeSet &attribsSet,
                                                            NameKey &nameKey,
                                                            uint16_t firstKeyIndex)
{
    for (uint16_t i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        nameKey.set(firstKeyIndex + i, attribsSet.count(utils::castToVertexAttribute(i)));
    return firstKeyIndex + utils::numElementsVertexAttribute();
}

void ProgramsManagerPrivate::prepareImageTypeDefines(graphics::TextureType textureType,
                                                     const std::string &definePrefix,
                                                     utils::ShaderDefines &defines)
{
    static const std::unordered_map<graphics::TextureType, std::string> s_table {
        { graphics::TextureType::Type1D, "1D" },
        { graphics::TextureType::Type2D, "2D" },
        { graphics::TextureType::Type3D, "3D" },
        { graphics::TextureType::TypeCube, "CUBE" },
        { graphics::TextureType::Type1DArray, "1D_ARRAY" },
        { graphics::TextureType::Type2DArray, "2D_ARRAY" },
        { graphics::TextureType::TypeCubeArray, "CUBE_ARRAY" },
        { graphics::TextureType::TypeRect, "RECT" },
    };

    if (auto it = s_table.find(textureType); it != s_table.end())
        defines.insert({definePrefix + "_TYPE_" + it->second, ""});
    else
        LOG_CRITICAL << "Unset texture type define";
}

uint16_t ProgramsManagerPrivate::prepareImageTypeKey(graphics::TextureType textureType,
                                                     NameKey &nameKey,
                                                     uint16_t firstKeyIndex)
{
    static_assert(graphics::numElementsTextureType() > 0u);
    static const auto typeBitsCount = glm::findMSB(graphics::numElementsTextureType()) + 1;
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(graphics::castFromTextureType(textureType), i, 1u));

    firstKeyIndex += typeBitsCount;

    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareImageFormatDefines(const graphics::SupportedImageFormats &supportedFormats,
                                                       graphics::PixelInternalFormat format,
                                                       const std::string &definePrefix,
                                                       utils::ShaderDefines &defines)
{
    if (auto it = supportedFormats.find(format); it != supportedFormats.end())
        defines.insert({definePrefix + "_FORMAT", it->second});
    else
        LOG_CRITICAL << "Unsupported image format";
}

uint16_t ProgramsManagerPrivate::prepareImageFormatKey(const graphics::SupportedImageFormats &supportedFormats,
                                                       graphics::PixelInternalFormat format,
                                                       NameKey &nameKey,
                                                       uint16_t firstKeyIndex)
{
    auto it = supportedFormats.find(format);
    if (it == supportedFormats.end())
        LOG_CRITICAL << "Unsupported image format";

    assert(supportedFormats.size() > 0u);
    const auto typeBitsCount = glm::findMSB(supportedFormats.size()) + 1;

    const auto d = static_cast<typename std::underlying_type<graphics::PixelInternalFormat>::type>(std::distance(supportedFormats.begin(), it));
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(d, i, 1u));

    firstKeyIndex += typeBitsCount;

    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareVisualDrawableComponentsDefines(const UniformCollection &uniformCollection,
                                                                    utils::ShaderDefines &defines)
{
    static const std::unordered_map<UniformID, std::string> s_table {
        { UniformID::AlphaCutoff, "HAS_ALPHA_CUTOFF" },
        { UniformID::BaseColor, "HAS_BASE_COLOR" },
        { UniformID::BaseColorMap, "HAS_BASE_COLOR_MAP" },
        { UniformID::Emission, "HAS_EMISSION" },
        { UniformID::EmissionMap, "HAS_EMISSION_MAP" },
        { UniformID::OcclusionMap, "HAS_OCCLUSION_MAP" },
        { UniformID::OcclusionMapStrength, "HAS_OCCLUSION_MAP_STRENGTH" },
        { UniformID::Roughness, "HAS_ROUGHNESS" },
        { UniformID::RoughnessMap, "HAS_ROUGHNESS_MAP" },
        { UniformID::Metalness, "HAS_METALNESS" },
        { UniformID::MetalnessMap, "HAS_METALNESS_MAP" },
        { UniformID::NormalMap, "HAS_NORMAL_MAP" },
        { UniformID::NormalMapScale, "HAS_NORMAL_MAP_SCALE" },
    };

    for (auto component : VisualDrawable::componentSet())
        if (uniformCollection.count(component))
        {
            if (auto it = s_table.find(component); it != s_table.end())
                defines.insert({it->second, ""});
            else
                LOG_CRITICAL << "Unset uniform id define";
        }
}

uint16_t ProgramsManagerPrivate::prepareVisualDrawableComponentsKey(const UniformCollection &uniformCollection,
                                                                    NameKey &nameKey,
                                                                    uint16_t firstKeyIndex)
{
    for (auto component : VisualDrawable::componentSet())
        nameKey.set(firstKeyIndex++, uniformCollection.count(component));
    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareSSAOModeDefines(SSAOMode mode,
                                                    utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsSSAOMode()> s_typesTable {
        "SSAO_MODE_DISABLED",
        "SSAO_MODE_OPAQUE",
        "SSAO_MODE_OPAQUE_AND_TRANSPARENT",
        "SSAO_MODE_COLOR"
    };

    defines.insert({ s_typesTable[castFromSSAOMode(mode)], "" });
}

uint16_t ProgramsManagerPrivate::prepareSSAOModeKey(SSAOMode mode,
                                                    NameKey &nameKey,
                                                    uint16_t firstKeyIndex)
{
    static_assert(numElementsSSAOMode() > 0u);
    static const auto typeBitsCount = glm::findMSB(numElementsSSAOMode()) + 1;
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(castFromSSAOMode(mode), i, 1u));

    firstKeyIndex += typeBitsCount;

    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareShadingModeDefines(ShadingMode shadingMode,
                                                       utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsShadingMode()> s_typesTable {
        "SHADING_MODE_DISABLED",
        "SHADING_MODE_OPAQUE",
        "SHADING_MODE_OPAQUE_AND_TRANSPARENT",
        "SHADING_MODE_COLOR"
    };

    defines.insert({ s_typesTable[castFromShadingMode(shadingMode)], "" });
}

uint16_t ProgramsManagerPrivate::prepareShadingModeKey(ShadingMode shadingMode,
                                                       NameKey &nameKey,
                                                       uint16_t firstKeyIndex)
{
    static_assert(numElementsShadingMode() > 0u);
    static const auto typeBitsCount = glm::findMSB(numElementsShadingMode()) + 1;
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(castFromShadingMode(shadingMode), i, 1u));

    firstKeyIndex += typeBitsCount;

    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareShadingFilterDefines(ShadingFilter shadingFilter,
                                                         utils::ShaderDefines &defines)
{
    static const std::array<std::string, numElementsShadingFilter()> s_typesTable {
        "SHADING_FILTER_POINT",
        "SHADING_FILTER_PCF",
        "SHADING_FILTER_VSM"
    };

    defines.insert({ s_typesTable[castFromShadingFilter(shadingFilter)], "" });
}

uint16_t ProgramsManagerPrivate::prepareShadingFilterKey(ShadingFilter shadingFilter,
                                                         NameKey &nameKey,
                                                         uint16_t firstKeyIndex)
{
    static_assert(numElementsShadingFilter() > 0u);
    static const auto typeBitsCount = glm::findMSB(numElementsShadingFilter()) + 1;
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(castFromShadingFilter(shadingFilter), i, 1u));

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
    static const auto typeBitsCount = glm::findMSB(numElementsLightType()) + 1;
    for (int i = 0; i < typeBitsCount; ++i)
        nameKey.set(firstKeyIndex + static_cast<size_t>(i), glm::bitfieldExtract(castFromLightType(lightType), i, 1u));
    firstKeyIndex += typeBitsCount;

    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareBackgroundComponentsDefines(const UniformCollection &uniformCollection,
                                                                utils::ShaderDefines &defines)
{
    static const std::unordered_map<UniformID, std::string> s_table {
        { UniformID::BackgroundColor, "HAS_BACKGROUND_COLOR" },
        { UniformID::BackgroundColorMap, "HAS_BACKGROUND_COLOR_MAP" },
        { UniformID::BackgroundRoughness, "HAS_BACKGROUND_ROUGHNESS" }
    };

    for (auto component : BackgroundDrawable::componentSet())
        if (uniformCollection.count(component))
        {
            if (auto it = s_table.find(component); it != s_table.end())
                defines.insert({it->second, ""});
            else
                LOG_CRITICAL << "Unset unifrom id define";
        }
}

uint16_t ProgramsManagerPrivate::prepareBackgroundComponentsKey(const UniformCollection &uniformCollection,
                                                                NameKey &nameKey,
                                                                uint16_t firstKeyIndex)
{
    for (auto component : BackgroundDrawable::componentSet())
        nameKey.set(firstKeyIndex++, uniformCollection.count(component));
    return firstKeyIndex;
}

void ProgramsManagerPrivate::prepareLightComponentsDefines(const UniformCollection &uniformCollection,
                                                           utils::ShaderDefines &defines)
{
    static const std::unordered_map<UniformID, std::string> s_table {
    };

    for (auto component : LightDrawable::componentSet())
        if (uniformCollection.count(component))
        {
            if (auto it = s_table.find(component); it != s_table.end())
                defines.insert({it->second, ""});
            else
                LOG_CRITICAL << "Unset unifrom id define";
        }
}

uint16_t ProgramsManagerPrivate::prepareLightComponentsKey(const UniformCollection &uniformCollection,
                                                           NameKey &nameKey,
                                                           uint16_t firstKeyIndex)
{
    for (auto component : LightDrawable::componentSet())
        nameKey.set(firstKeyIndex++, uniformCollection.count(component));
    return firstKeyIndex;
}

const std::string &ProgramsManagerPrivate::geometryPassRenderProgramName()
{
    return s_geometryPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::shadowRenderProgramName()
{
    return s_shadowRenderProgramName;
}

const std::string &ProgramsManagerPrivate::stencilPassRenderProgramName()
{
    return s_stencilPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::OITClearPassComputeProgramName()
{
    return s_OITClearPassComputeProgramName;
}

const std::string &ProgramsManagerPrivate::OITSortNodesPassComputeProgramName()
{
    return s_OITSortNodesPassComputeProgramName;
}

const std::string &ProgramsManagerPrivate::SSAORenderProgramName()
{
    return s_SSAORenderProgramName;
}

const std::string &ProgramsManagerPrivate::blurRenderProgramName()
{
    return s_blurRenderProgramName;
}

const std::string &ProgramsManagerPrivate::bilaterialBlurRenderProgramName()
{
    return s_bilaterialBlurRenderProgramName;
}

const std::string &ProgramsManagerPrivate::lightPassRenderProgramName()
{
    return s_lightPassRenderProgramName;
}

const std::string &ProgramsManagerPrivate::backgroundPassRenderProgramName()
{
    return s_backgroundPassRenderProgramName;
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

const std::filesystem::path &ProgramsManagerPrivate::geometryPassFragmnetShaderPath()
{
    return s_geometryPassFragmnetShaderPath;
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

const std::filesystem::path &ProgramsManagerPrivate::OITClearPassComputeShaderPath()
{
    return s_OITClearPassComputeShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::OITSortNodesPassComputeShaderPath()
{
    return s_OITSortNodesPassComputeShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::SSAOVertexShaderPath()
{
    return s_SSAOVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::SSAOFragmnetShaderPath()
{
    return s_SSAOFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::blurVertexShaderPath()
{
    return s_blurVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::blurFragmnetShaderPath()
{
    return s_blurFragmnetShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::blurComputeShaderPath()
{
    return s_blurComputeShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::bilaterialBlurVertexShaderPath()
{
    return s_bilaterialBlurVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::bilaterialBlurFragmnetShaderPath()
{
    return s_bilaterialBlurFragmnetShaderPath;
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

const std::filesystem::path &ProgramsManagerPrivate::backgroundPassVertexShaderPath()
{
    return s_backgroundPassVertexShaderPath;
}

const std::filesystem::path &ProgramsManagerPrivate::backgroundPassFragmnetShaderPath()
{
    return s_backgroundPassFragmnetShaderPath;
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
