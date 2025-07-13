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

    ProgramsManagerPrivate(const std::shared_ptr<graphics::RendererBase>&);
    ~ProgramsManagerPrivate();

    std::shared_ptr<graphics::RendererBase> &renderer();
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> &resources();

    static void prepareTransparentDefines(bool,
                                          utils::ShaderDefines&);
    static uint16_t prepareTransparentKey(bool,
                                          NameKey&,
                                          uint16_t);

    static void prepareVertexAttributesDefines(const utils::VertexAttributeSet&,
                                               utils::ShaderDefines&);
    static uint16_t prepareVertexAttributesKey(const utils::VertexAttributeSet&,
                                               NameKey&,
                                               uint16_t);

    static void prepareImageTypeDefines(graphics::TextureType,
                                        const std::string&,
                                        utils::ShaderDefines&);
    static uint16_t prepareImageTypeKey(graphics::TextureType,
                                        NameKey&,
                                        uint16_t);

    static void prepareImageFormatDefines(const graphics::SupportedImageFormats&,
                                          graphics::PixelInternalFormat,
                                          const std::string&,
                                          utils::ShaderDefines&);
    static uint16_t prepareImageFormatKey(const graphics::SupportedImageFormats&,
                                          graphics::PixelInternalFormat,
                                          NameKey&,
                                          uint16_t);

    static void prepareVisualDrawableComponentsDefines(const UniformCollection&,
                                                       utils::ShaderDefines&);
    static uint16_t prepareVisualDrawableComponentsKey(const UniformCollection&,
                                                       NameKey&,
                                                       uint16_t);

    static void prepareSSAOModeDefines(SSAOMode,
                                       utils::ShaderDefines&);
    static uint16_t prepareSSAOModeKey(SSAOMode,
                                       NameKey&,
                                       uint16_t);

    static void prepareShadingModeDefines(ShadingMode,
                                          utils::ShaderDefines&);
    static uint16_t prepareShadingModeKey(ShadingMode,
                                          NameKey&,
                                          uint16_t);

    static void prepareShadingFilterDefines(ShadingFilter,
                                            utils::ShaderDefines&);
    static uint16_t prepareShadingFilterKey(ShadingFilter,
                                            NameKey&,
                                            uint16_t);

    static void prepareLightTypeDefines(LightType,
                                        utils::ShaderDefines&);
    static uint16_t prepareLightTypeKey(LightType,
                                        NameKey&,
                                        uint16_t);

    static void prepareBackgroundComponentsDefines(const UniformCollection&,
                                                   utils::ShaderDefines&);
    static uint16_t prepareBackgroundComponentsKey(const UniformCollection&,
                                                   NameKey&,
                                                   uint16_t);

    static void prepareLightComponentsDefines(const UniformCollection&,
                                              utils::ShaderDefines&);
    static uint16_t prepareLightComponentsKey(const UniformCollection&,
                                              NameKey&,
                                              uint16_t);

    static const std::string &geometryPassRenderProgramName();
    static const std::string &stencilPassRenderProgramName();
    static const std::string &lightPassRenderProgramName();
    static const std::string &OITClearPassComputeProgramName();
    static const std::string &OITSortNodesPassComputeProgramName();
    static const std::string &SSAORenderProgramName();
    static const std::string &blurRenderProgramName();
    static const std::string &bilaterialBlurRenderProgramName();
    static const std::string &shadowRenderProgramName();
    static const std::string &backgroundPassRenderProgramName();
    static const std::string &finalPassRenderProgramName();
    static const std::string &postprocessPassRenderProgramName();

    static const std::filesystem::path &geometryPassVertexShaderPath();
    static const std::filesystem::path &geometryPassFragmnetShaderPath();
    static const std::filesystem::path &stencilPassVertexShaderPath();
    static const std::filesystem::path &stencilPassFragmnetShaderPath();
    static const std::filesystem::path &lightPassVertexShaderPath();
    static const std::filesystem::path &lightPassFragmnetShaderPath();
    static const std::filesystem::path &OITClearPassComputeShaderPath();
    static const std::filesystem::path &OITSortNodesPassComputeShaderPath();
    static const std::filesystem::path &SSAOVertexShaderPath();
    static const std::filesystem::path &SSAOFragmnetShaderPath();
    static const std::filesystem::path &blurVertexShaderPath();
    static const std::filesystem::path &blurFragmnetShaderPath();
    static const std::filesystem::path &blurComputeShaderPath();
    static const std::filesystem::path &bilaterialBlurVertexShaderPath();
    static const std::filesystem::path &bilaterialBlurFragmnetShaderPath();
    static const std::filesystem::path &shadowVertexShaderPath();
    static const std::filesystem::path &shadowGeometryShaderPath();
    static const std::filesystem::path &shadowFragmnetShaderPath();
    static const std::filesystem::path &backgroundPassVertexShaderPath();
    static const std::filesystem::path &backgroundPassFragmnetShaderPath();
    static const std::filesystem::path &finalPassVertexShaderPath();
    static const std::filesystem::path &finalPassFragmnetShaderPath();
    static const std::filesystem::path &postprocessPassVertexShaderPath();
    static const std::filesystem::path &postprocessPassFragmnetShaderPath();

private:
    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::unordered_map<std::string, std::shared_ptr<graphics::IProgram>> m_resources;

    static const std::string s_geometryPassRenderProgramName;
    static const std::string s_lightPassRenderProgramName;
    static const std::string s_backgroundPassRenderProgramName;
    static const std::string s_OITClearPassComputeProgramName;
    static const std::string s_OITSortNodesPassComputeProgramName;
    static const std::string s_SSAORenderProgramName;
    static const std::string s_blurRenderProgramName;
    static const std::string s_bilaterialBlurRenderProgramName;
    static const std::string s_shadowRenderProgramName;
    static const std::string s_stencilPassRenderProgramName;
    static const std::string s_finalPassRenderProgramName;
    static const std::string s_postprocessPassRenderProgramName;

    static const std::filesystem::path s_geometryPassVertexShaderPath;
    static const std::filesystem::path s_geometryPassFragmnetShaderPath;
    static const std::filesystem::path s_stencilPassVertexShaderPath;
    static const std::filesystem::path s_stencilPassFragmnetShaderPath;
    static const std::filesystem::path s_lightPassVertexShaderPath;
    static const std::filesystem::path s_lightPassFragmnetShaderPath;
    static const std::filesystem::path s_OITClearPassComputeShaderPath;
    static const std::filesystem::path s_OITSortNodesPassComputeShaderPath;
    static const std::filesystem::path s_SSAOVertexShaderPath;
    static const std::filesystem::path s_SSAOFragmnetShaderPath;
    static const std::filesystem::path s_blurVertexShaderPath;
    static const std::filesystem::path s_blurFragmnetShaderPath;
    static const std::filesystem::path s_blurComputeShaderPath;
    static const std::filesystem::path s_bilaterialBlurVertexShaderPath;
    static const std::filesystem::path s_bilaterialBlurFragmnetShaderPath;
    static const std::filesystem::path s_shadowVertexShaderPath;
    static const std::filesystem::path s_shadowGeometryShaderPath;
    static const std::filesystem::path s_shadowFragmnetShaderPath;
    static const std::filesystem::path s_backgroundPassVertexShaderPath;
    static const std::filesystem::path s_backgroundPassFragmnetShaderPath;
    static const std::filesystem::path s_finalPassVertexShaderPath;
    static const std::filesystem::path s_finalPassFragmnetShaderPath;
    static const std::filesystem::path s_postprocessPassVertexShaderPath;
    static const std::filesystem::path s_postprocessPassFragmnetShaderPath;
};

}
}

#endif // CORE_PROGRAMSMANAGERPRIVATE_H
