#ifndef CORE_PROGRAMSMANAGERPRIVATE_H
#define CORE_PROGRAMSMANAGERPRIVATE_H

#include <memory>
#include <unordered_map>
#include <string>
#include <bitset>
#include <filesystem>

#include <utils/forwarddecl.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ProgramsManagerPrivate
{
public:
    using NameKey = std::bitset<32u>;

    ProgramsManagerPrivate(std::shared_ptr<graphics::IRenderer>);
    ~ProgramsManagerPrivate();

    std::shared_ptr<graphics::IRenderer> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &resources();

    static float &dielectricSpecular();

    static void prepareVertexAttributesDefines(const utils::VertexAttributesSet&,
                                               utils::ShaderDefines&);
    static uint16_t prepareVertexAttributesKey(const utils::VertexAttributesSet&,
                                               NameKey&,
                                               uint16_t);

    static void preparePBRComponentsDefines(const std::unordered_set<PBRComponent>&,
                                            utils::ShaderDefines&);
    static uint16_t preparePBRComponentsKey(const std::unordered_set<PBRComponent>&,
                                            NameKey&,
                                            uint16_t);

    static void prepareLightShadingModeDefines(LightShadingMode,
                                               utils::ShaderDefines&);
    static uint16_t prepareLightShadingModeKey(LightShadingMode,
                                               NameKey&,
                                               uint16_t);

    static void prepareLightTypeDefines(LightType,
                                        utils::ShaderDefines&);
    static uint16_t prepareLightTypeKey(LightType,
                                        NameKey&,
                                        uint16_t);

    static void prepareBackgroundComponentsDefines(const std::unordered_set<BackgroundComponent>&,
                                                   utils::ShaderDefines&);
    static uint16_t prepareBackgroundComponentsKey(const std::unordered_set<BackgroundComponent>&,
                                                   NameKey&,
                                                   uint16_t);

    static void prepareLightComponentsDefines(const std::unordered_set<LightComponent>&,
                                              utils::ShaderDefines&);
    static uint16_t prepareLightComponentsKey(const std::unordered_set<LightComponent>&,
                                              NameKey&,
                                              uint16_t);

    static const std::string &opaqueGeometryPassRenderProgramName();
    static const std::string &transparentGeometryPassRenderProgramName();
    static const std::string &OITClearPassComputeProgramName();
    static const std::string &OITSortNodesPassComputeProgramName();
    static const std::string &shadowRenderProgramName();
    static const std::string &stencilPassRenderProgramName();
    static const std::string &lightPassRenderProgramName();
    static const std::string &backgroundPassRenderProgramName();
    static const std::string &foregroundPassRenderProgramName();
    static const std::string &finalPassRenderProgramName();
    static const std::string &postprocessPassRenderProgramName();

    static const std::filesystem::path &geometryPassVertexShaderPath();
    static const std::filesystem::path &opaqueGeometryPassFragmnetShaderPath();
    static const std::filesystem::path &transparentGeometryPassFragmnetShaderPath();
    static const std::filesystem::path &OITClearPassComputeShaderPath();
    static const std::filesystem::path &OITSortNodesPassComputeShaderPath();
    static const std::filesystem::path &shadowVertexShaderPath();
    static const std::filesystem::path &shadowGeometryShaderPath();
    static const std::filesystem::path &shadowFragmnetShaderPath();
    static const std::filesystem::path &stencilPassVertexShaderPath();
    static const std::filesystem::path &stencilPassFragmnetShaderPath();
    static const std::filesystem::path &lightPassVertexShaderPath();
    static const std::filesystem::path &lightPassFragmnetShaderPath();
    static const std::filesystem::path &backgroundPassVertexShaderPath();
    static const std::filesystem::path &backgroundPassFragmnetShaderPath();
    static const std::filesystem::path &foregroundPassVertexShaderPath();
    static const std::filesystem::path &foregroundPassFragmnetShaderPath();
    static const std::filesystem::path &finalPassVertexShaderPath();
    static const std::filesystem::path &finalPassFragmnetShaderPath();
    static const std::filesystem::path &postprocessPassVertexShaderPath();
    static const std::filesystem::path &postprocessPassFragmnetShaderPath();

private:
    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> m_resources;

    static float s_dielectricSpecular;

    static const std::string s_opaqueGeometryPassRenderProgramName;
    static const std::string s_transparentGeometryPassRenderProgramName;
    static const std::string s_OITClearPassComputeProgramName;
    static const std::string s_OITSortNodesPassComputeProgramName;
    static const std::string s_shadowRenderProgramName;
    static const std::string s_stencilPassRenderProgramName;
    static const std::string s_lightPassRenderProgramName;
    static const std::string s_backgroundPassRenderProgramName;
    static const std::string s_foregroundPassRenderProgramName;
    static const std::string s_finalPassRenderProgramName;
    static const std::string s_postprocessPassRenderProgramName;

    static const std::filesystem::path s_geometryPassVertexShaderPath;
    static const std::filesystem::path s_opaqueGeometryPassFragmnetShaderPath;
    static const std::filesystem::path s_transparentGeometryPassFragmnetShaderPath;
    static const std::filesystem::path s_OITClearPassComputeShaderPath;
    static const std::filesystem::path s_OITSortNodesPassComputeShaderPath;
    static const std::filesystem::path s_shadowVertexShaderPath;
    static const std::filesystem::path s_shadowGeometryShaderPath;
    static const std::filesystem::path s_shadowFragmnetShaderPath;
    static const std::filesystem::path s_stencilPassVertexShaderPath;
    static const std::filesystem::path s_stencilPassFragmnetShaderPath;
    static const std::filesystem::path s_lightPassVertexShaderPath;
    static const std::filesystem::path s_lightPassFragmnetShaderPath;
    static const std::filesystem::path s_backgroundPassVertexShaderPath;
    static const std::filesystem::path s_backgroundPassFragmnetShaderPath;
    static const std::filesystem::path s_foregroundPassVertexShaderPath;
    static const std::filesystem::path s_foregroundPassFragmnetShaderPath;
    static const std::filesystem::path s_finalPassVertexShaderPath;
    static const std::filesystem::path s_finalPassFragmnetShaderPath;
    static const std::filesystem::path s_postprocessPassVertexShaderPath;
    static const std::filesystem::path s_postprocessPassFragmnetShaderPath;
};

}
}

#endif // CORE_PROGRAMSMANAGERPRIVATE_H
