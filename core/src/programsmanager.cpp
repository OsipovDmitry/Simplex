#include <utils/logger.h>
#include <utils/shader.h>

#include <core/programsmanager.h>
#include <core/igraphicsrenderer.h>

#include "programsmanagerprivate.h"

namespace simplex
{
namespace core
{

ProgramsManager::ProgramsManager(std::shared_ptr<graphics::IRenderer> renderer)
    : m_(std::make_unique<ProgramsManagerPrivate>(renderer))
{
}

ProgramsManager::~ProgramsManager() = default;

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetRenderProgram(const std::filesystem::path &vShaderFileName,
                                                                                  const std::filesystem::path &fShaderFileName,
                                                                                  const utils::ShaderDefines &defines,
                                                                                  const std::string &key)
{
    auto name = key;
    if (name == "autogen")
    {
        name = vShaderFileName.string() + fShaderFileName.string();
        for (const auto &define : defines)
            name += define.first + define.second;
    }

    if (auto it = m_->resources().find(name); it != m_->resources().end())
    {
        auto renderProgram = std::dynamic_pointer_cast<graphics::IRenderProgram>(it->second);
        if (!renderProgram)
            LOG_CRITICAL << "Program with name \"" << name << "\" was created in a different type";

        return renderProgram;
    }

    auto vShader = utils::Shader::loadFromFile(vShaderFileName, defines);
    auto fShader = utils::Shader::loadFromFile(fShaderFileName, defines);

    auto renderProgram = m_->renderer()->createRenderProgram(vShader, fShader);
    if (renderProgram)
        m_->resources()[name] = renderProgram;
    else
        LOG_CRITICAL << "Failed to load render program: " << vShaderFileName << ", " << fShaderFileName;

    return renderProgram;
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetRenderProgram(const std::filesystem::path &vShaderFileName,
                                                                                  const std::filesystem::path &gShaderFileName,
                                                                                  const std::filesystem::path &fShaderFileName,
                                                                                  const utils::ShaderDefines &defines,
                                                                                  const std::string &key)
{
    auto name = key;
    if (name == "autogen")
    {
        name = vShaderFileName.string() + gShaderFileName.string() + fShaderFileName.string();
        for (const auto &define : defines)
            name += define.first + define.second;
    }

    if (auto it = m_->resources().find(name); it != m_->resources().end())
    {
        auto renderProgram = std::dynamic_pointer_cast<graphics::IRenderProgram>(it->second);
        if (!renderProgram)
            LOG_CRITICAL << "Program with name \"" << name << "\" was created in a different type";

        return renderProgram;
    }

    auto vShader = utils::Shader::loadFromFile(vShaderFileName, defines);
    auto gShader = utils::Shader::loadFromFile(gShaderFileName, defines);
    auto fShader = utils::Shader::loadFromFile(fShaderFileName, defines);

    auto renderProgram = m_->renderer()->createRenderProgram(vShader, gShader, fShader);
    if (renderProgram)
        m_->resources()[name] = renderProgram;
    else
        LOG_CRITICAL << "Failed to load render program: " << vShaderFileName << ", " << gShaderFileName << ", " << fShaderFileName;

    return renderProgram;
}

std::shared_ptr<graphics::IComputeProgram> ProgramsManager::loadOrGetComputeProgram(const std::filesystem::path &cShaderFileName,
                                                                                    const utils::ShaderDefines &defines,
                                                                                    const std::string &key)
{
    auto name = key;
    if (name == "autogen")
    {
        name = cShaderFileName.string();
        for (const auto &define : defines)
            name += define.first + define.second;
    }

    if (auto it = m_->resources().find(name); it != m_->resources().end())
    {
        auto computeProgram = std::dynamic_pointer_cast<graphics::IComputeProgram>(it->second);
        if (!computeProgram)
            LOG_CRITICAL << "Program with name \"" << name << "\" was created in a different type";

        return computeProgram;
    }

    auto cShader = utils::Shader::loadFromFile(cShaderFileName, defines);

    auto computeProgram = m_->renderer()->createComputeProgram(cShader);
    if (computeProgram)
        m_->resources()[name] = computeProgram;
    else
        LOG_CRITICAL << "Failed to load compute program: " << cShaderFileName;

    return computeProgram;
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetGeometryPassRenderProgram(const utils::VertexAttributeSet &attribsSet,
                                                                                              const UniformCollection &uniformCollection,
                                                                                              bool isTransparent)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareTransparentDefines(isTransparent, defines);
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);
    ProgramsManagerPrivate::prepareVisualDrawableComponentsDefines(uniformCollection, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareTransparentKey(isTransparent, key, bit);
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);
    bit = ProgramsManagerPrivate::prepareVisualDrawableComponentsKey(uniformCollection, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::geometryPassVertexShaderPath(),
                                  ProgramsManagerPrivate::geometryPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::geometryPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetStencilPassRenderProgram(const utils::VertexAttributeSet &attribsSet,
                                                                                             LightType lightType)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);
    ProgramsManagerPrivate::prepareLightTypeDefines(lightType, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);
    bit = ProgramsManagerPrivate::prepareLightTypeKey(lightType, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::stencilPassVertexShaderPath(),
                                  ProgramsManagerPrivate::stencilPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::stencilPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetLightPassRenderProgram(const utils::VertexAttributeSet &attribsSet,
                                                                                           const UniformCollection &uniformCollection,
                                                                                           LightType lightType,
                                                                                           ShadingMode shadingMode,
                                                                                           ShadingFilter shadingFilter,
                                                                                           SSAOMode ssaoMode)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);
    ProgramsManagerPrivate::prepareLightComponentsDefines(uniformCollection, defines);
    ProgramsManagerPrivate::prepareLightTypeDefines(lightType, defines);
    ProgramsManagerPrivate::prepareShadingModeDefines(shadingMode, defines);
    ProgramsManagerPrivate::prepareShadingFilterDefines(shadingFilter, defines);
    ProgramsManagerPrivate::prepareSSAOModeDefines(ssaoMode, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);
    bit = ProgramsManagerPrivate::prepareLightComponentsKey(uniformCollection, key, bit);
    bit = ProgramsManagerPrivate::prepareLightTypeKey(lightType, key, bit);
    bit = ProgramsManagerPrivate::prepareShadingModeKey(shadingMode, key, bit);
    bit = ProgramsManagerPrivate::prepareShadingFilterKey(shadingFilter, key, bit);
    bit = ProgramsManagerPrivate::prepareSSAOModeKey(ssaoMode, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::lightPassVertexShaderPath(),
                                  ProgramsManagerPrivate::lightPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::lightPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IComputeProgram> ProgramsManager::loadOrGetOITClearPassComputeProgram()
{
    return loadOrGetComputeProgram(ProgramsManagerPrivate::OITClearPassComputeShaderPath(),
                                   {},
                                   ProgramsManagerPrivate::OITClearPassComputeProgramName());
}

std::shared_ptr<graphics::IComputeProgram> ProgramsManager::loadOrGetOITSortNodesPassComputeProgram()
{
    return loadOrGetComputeProgram(ProgramsManagerPrivate::OITSortNodesPassComputeShaderPath(),
                                   {},
                                   ProgramsManagerPrivate::OITSortNodesPassComputeProgramName());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetSSAORenderProgram(const utils::VertexAttributeSet &attribsSet,
                                                                                      SSAOMode mode)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);
    ProgramsManagerPrivate::prepareSSAOModeDefines(mode, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);
    bit = ProgramsManagerPrivate::prepareSSAOModeKey(mode, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::SSAOVertexShaderPath(),
                                  ProgramsManagerPrivate::SSAOFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::SSAORenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetBlurRenderProgram(const utils::VertexAttributeSet &attribsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::blurVertexShaderPath(),
                                  ProgramsManagerPrivate::blurFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::blurRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IComputeProgram> ProgramsManager::loadOrGetBlurComputeProgram(graphics::TextureType textureType,
                                                                                        graphics::PixelInternalFormat imageFormat)
{
    const auto &supportedFormats = m_->renderer()->supportedImageFormats();

    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareImageTypeDefines(textureType, "SOURCE_IMAGE", defines);
    ProgramsManagerPrivate::prepareImageFormatDefines(supportedFormats, imageFormat, "SOURCE_IMAGE", defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareImageTypeKey(textureType, key, bit);
    bit = ProgramsManagerPrivate::prepareImageFormatKey(supportedFormats, imageFormat, key, bit);

    return loadOrGetComputeProgram(ProgramsManagerPrivate::blurComputeShaderPath(),
                                   defines,
                                   ProgramsManagerPrivate::blurRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetBilaterialBlurRenderProgram(const utils::VertexAttributeSet &attribsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::bilaterialBlurVertexShaderPath(),
                                  ProgramsManagerPrivate::bilaterialBlurFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::bilaterialBlurRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetShadowRenderProgram(const utils::VertexAttributeSet &attribsSet,
                                                                                        const UniformCollection &uniformCollection,
                                                                                        ShadingMode shadingMode,
                                                                                        ShadingFilter shadingFilter)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);
    ProgramsManagerPrivate::prepareVisualDrawableComponentsDefines(uniformCollection, defines);
    ProgramsManagerPrivate::prepareShadingModeDefines(shadingMode, defines);
    ProgramsManagerPrivate::prepareShadingFilterDefines(shadingFilter, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);
    bit = ProgramsManagerPrivate::prepareVisualDrawableComponentsKey(uniformCollection, key, bit);
    bit = ProgramsManagerPrivate::prepareShadingModeKey(shadingMode, key, bit);
    bit = ProgramsManagerPrivate::prepareShadingFilterKey(shadingFilter, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::shadowVertexShaderPath(),
                                  ProgramsManagerPrivate::shadowGeometryShaderPath(),
                                  ProgramsManagerPrivate::shadowFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::shadowRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetBackgroundPassRenderProgram(const utils::VertexAttributeSet &attribsSet,
                                                                                                const UniformCollection &uniformCollection)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);
    ProgramsManagerPrivate::prepareBackgroundComponentsDefines(uniformCollection, defines);

    ProgramsManagerPrivate::NameKey key;
    uint16_t bit = 0u;
    bit = ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, bit);
    bit = ProgramsManagerPrivate::prepareBackgroundComponentsKey(uniformCollection, key, bit);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::backgroundPassVertexShaderPath(),
                                  ProgramsManagerPrivate::backgroundPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::backgroundPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetFinalPassRenderProgram(const utils::VertexAttributeSet &attribsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);

    ProgramsManagerPrivate::NameKey key;
    ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, 0u);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::finalPassVertexShaderPath(),
                                  ProgramsManagerPrivate::finalPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::finalPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetPostprocessPassRenderProgram(const utils::VertexAttributeSet &attribsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);

    ProgramsManagerPrivate::NameKey key;
    ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, 0u);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::postprocessPassVertexShaderPath(),
                                  ProgramsManagerPrivate::postprocessPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::postprocessPassRenderProgramName() + key.to_string());
}



}
}
