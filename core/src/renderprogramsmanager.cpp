#include <utils/logger.h>
#include <utils/shader.h>

#include <core/renderprogramsmanager.h>
#include <core/igraphicsrenderer.h>

#include "renderprogramsmanagerprivate.h"

namespace simplex
{
namespace core
{

RenderProgramsManager::RenderProgramsManager(std::shared_ptr<graphics::IRenderer> renderer)
    : m_(std::make_unique<RenderProgramsManagerPrivate>(renderer))
{
}

simplex::core::RenderProgramsManager::~RenderProgramsManager() = default;

std::shared_ptr<graphics::IRenderProgram> RenderProgramsManager::loadOrGetRenderProgram(const std::filesystem::path &vShaderFileName,
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
        {
            LOG_ERROR << "Program with name \"" << name << "\" was created in a different type";
            assert(false);
        }

        return renderProgram;
    }

    auto vShader = utils::Shader::loadShader(vShaderFileName, defines);
    auto fShader = utils::Shader::loadShader(fShaderFileName, defines);

    auto renderProgram = m_->renderer()->createRenderProgram(*vShader, *fShader);
    m_->resources()[name] = renderProgram;

    return renderProgram;
}

std::shared_ptr<graphics::IComputeProgram> RenderProgramsManager::loadOrGetComputeProgram(const std::filesystem::path &cShaderFileName,
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
        {
            LOG_ERROR << "Program with name \"" << name << "\" was created in a different type";
            assert(false);
        }

        return computeProgram;
    }

    auto cShader = utils::Shader::loadShader(cShaderFileName, defines);

    auto computeProgram = m_->renderer()->createComputeProgram(*cShader);
    m_->resources()[name] = computeProgram;

    return computeProgram;
}

std::shared_ptr<graphics::IRenderProgram> RenderProgramsManager::loadOrGetOpaqueGeometryPassRenderProgram(
        const utils::VertexAttributesSet &attribsSet,
        const graphics::PBRComponentsSet &PBRComponentsSet)
{
    utils::ShaderDefines defines;
    RenderProgramsManagerPrivate::VertexAttributesKey key;
    RenderProgramsManagerPrivate::prepareDefinesAndKeyForGeometryPassRenderProgram(attribsSet,
                                                                                   PBRComponentsSet,
                                                                                   defines,
                                                                                   key);

    return loadOrGetRenderProgram(RenderProgramsManagerPrivate::geometryPassVertexShaderPath(),
                     RenderProgramsManagerPrivate::opaqueGeometryPassFragmnetShaderPath(),
                     defines,
                     RenderProgramsManagerPrivate::opaqueGeometryPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IRenderProgram> RenderProgramsManager::loadOrGetTransparentGeometryPassRenderProgram(
        const utils::VertexAttributesSet &attribsSet,
        const graphics::PBRComponentsSet &PBRComponentsSet)
{
    utils::ShaderDefines defines;
    RenderProgramsManagerPrivate::VertexAttributesKey key;
    RenderProgramsManagerPrivate::prepareDefinesAndKeyForGeometryPassRenderProgram(attribsSet,
                                                                                   PBRComponentsSet,
                                                                                   defines,
                                                                                   key);

    return loadOrGetRenderProgram(RenderProgramsManagerPrivate::geometryPassVertexShaderPath(),
                     RenderProgramsManagerPrivate::transparentGeometryPassFragmnetShaderPath(),
                     defines,
                                  RenderProgramsManagerPrivate::transparentGeometryPassRenderProgramName() + key.to_string());
}

std::shared_ptr<graphics::IComputeProgram> RenderProgramsManager::loadOrGetOITClearPassComputeProgram()
{
    return loadOrGetComputeProgram(RenderProgramsManagerPrivate::OITClearPassComputeShaderPath(),
                                   {},
                                   RenderProgramsManagerPrivate::OITClearPassComputeProgramName());
}

std::shared_ptr<graphics::IComputeProgram> RenderProgramsManager::loadOrGetOITSortNodesPassComputeProgram()
{
    return loadOrGetComputeProgram(RenderProgramsManagerPrivate::OITSortNodesPassComputeShaderPath(),
                                   {},
                                   RenderProgramsManagerPrivate::OITSortNodesPassComputeProgramName());
}

std::shared_ptr<graphics::IRenderProgram> RenderProgramsManager::loadOrGetFinalPassRenderProgram(const utils::VertexAttributesSet &attribsSet)
{
    utils::ShaderDefines defines;
    RenderProgramsManagerPrivate::prepareVertexAttributesDefines(attribsSet, defines);

    RenderProgramsManagerPrivate::VertexAttributesKey key;
    RenderProgramsManagerPrivate::prepareVertexAttributesKey(attribsSet, key, 0u);

    return loadOrGetRenderProgram(RenderProgramsManagerPrivate::finalPassVertexShaderPath(),
                     RenderProgramsManagerPrivate::finalPassFragmnetShaderPath(),
                     defines,
                     RenderProgramsManagerPrivate::finalPassRenderProgramName() + key.to_string());
}



}
}
