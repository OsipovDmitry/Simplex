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
    m_->resources()[name] = renderProgram;

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
    m_->resources()[name] = computeProgram;

    return computeProgram;
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetOpaqueGeometryPassRenderProgram(
        const utils::VertexAttributesSet &attribsSet,
        const graphics::PBRComponentsSet &PBRComponentsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::VertexAttributesKey key;
    ProgramsManagerPrivate::prepareDefinesAndKeyForGeometryPassRenderProgram(attribsSet,
                                                                             PBRComponentsSet,
                                                                             defines,
                                                                             key);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::geometryPassVertexShaderPath(),
                                  ProgramsManagerPrivate::opaqueGeometryPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::opaqueGeometryPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetTransparentGeometryPassRenderProgram(
        const utils::VertexAttributesSet &attribsSet,
        const graphics::PBRComponentsSet &PBRComponentsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::VertexAttributesKey key;
    ProgramsManagerPrivate::prepareDefinesAndKeyForGeometryPassRenderProgram(attribsSet,
                                                                             PBRComponentsSet,
                                                                             defines,
                                                                             key);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::geometryPassVertexShaderPath(),
                                  ProgramsManagerPrivate::transparentGeometryPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::transparentGeometryPassRenderProgramName() + key.to_string());
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

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetLightPassRenderProgram(const utils::VertexAttributesSet &attribsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::VertexAttributesKey key;
    ProgramsManagerPrivate::prepareDefinesAndKeyForLightPassRenderProgram(attribsSet,
                                                                          defines,
                                                                          key);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::lightPassVertexShaderPath(),
                                  ProgramsManagerPrivate::lightPassFragmnetShaderPath(),
                                  defines,
                                  ProgramsManagerPrivate::lightPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> ProgramsManager::loadOrGetFinalPassRenderProgram(const utils::VertexAttributesSet &attribsSet)
{
    utils::ShaderDefines defines;
    ProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);

    ProgramsManagerPrivate::VertexAttributesKey key;
    ProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, 0u);

    return loadOrGetRenderProgram(ProgramsManagerPrivate::finalPassVertexShaderPath(),
                     ProgramsManagerPrivate::finalPassFragmnetShaderPath(),
                     defines,
                     ProgramsManagerPrivate::finalPassRenderProgramName() + key.to_string());
}



}
}
