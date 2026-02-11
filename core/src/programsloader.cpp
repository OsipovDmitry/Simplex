#include <utils/shader.h>

#include <core/graphicsrendererbase.h>
#include <core/programsloader.h>

#include "programsloaderprivate.h"

namespace simplex
{
namespace core
{

ProgramsLoader::ProgramsLoader(const std::string& name, const std::shared_ptr<graphics::RendererBase> &renderer)
    : ResourceLoaderBase<graphics::IProgram>(name)
    , m_(std::make_unique<ProgramsLoaderPrivate>(renderer))
{
}

ProgramsLoader::~ProgramsLoader() = default;

std::shared_ptr<graphics::IRenderProgram> ProgramsLoader::loadOrGetRenderProgram(
    const std::filesystem::path &vShaderFileName,
    const std::filesystem::path &fShaderFileName,
    const utils::ShaderDefines &defines)
{
    auto name = vShaderFileName.string() + fShaderFileName.string();
    for (const auto &define : defines)
        name += define.first + define.second;

    if (auto program = findResource(name))
    {
        auto renderProgram = std::dynamic_pointer_cast<graphics::IRenderProgram>(program);
        if (!renderProgram)
        {
            LOG_CRITICAL << "Program with name \"" << name << "\" was created in a different type";
            return nullptr;
        }

        return renderProgram;
    }

    auto vShader = utils::Shader::loadFromFile(vShaderFileName, defines);
    auto fShader = utils::Shader::loadFromFile(fShaderFileName, defines);

    auto renderProgram = m_->renderer()->createRenderProgram(vShader, fShader);
    if (!renderProgram)
    {
        LOG_CRITICAL << "Failed to create render program: " << vShaderFileName << ", " << fShaderFileName;
        return nullptr;
    }

    addResource(name, renderProgram);
    return renderProgram;
}

std::shared_ptr<graphics::IRenderProgram> ProgramsLoader::loadOrGetRenderProgram(
    const std::filesystem::path &vShaderFileName,
    const std::filesystem::path &gShaderFileName,
    const std::filesystem::path &fShaderFileName,
    const utils::ShaderDefines &defines)
{
    auto name = vShaderFileName.string() + gShaderFileName.string() + fShaderFileName.string();
    for (const auto& define : defines)
        name += define.first + define.second;

    if (auto program = findResource(name))
    {
        auto renderProgram = std::dynamic_pointer_cast<graphics::IRenderProgram>(program);
        if (!renderProgram)
        {
            LOG_CRITICAL << "Program with name \"" << name << "\" was created in a different type";
            return nullptr;
        }

        return renderProgram;
    }

    auto vShader = utils::Shader::loadFromFile(vShaderFileName, defines);
    auto gShader = utils::Shader::loadFromFile(gShaderFileName, defines);
    auto fShader = utils::Shader::loadFromFile(fShaderFileName, defines);

    auto renderProgram = m_->renderer()->createRenderProgram(vShader, gShader, fShader);
    if (!renderProgram)
    {
        LOG_CRITICAL << "Failed to create render program: " << vShaderFileName << ", " << gShaderFileName << ", " << fShaderFileName;
        return nullptr;
    }

    addResource(name, renderProgram);
    return renderProgram;
}

std::shared_ptr<graphics::IComputeProgram> ProgramsLoader::loadOrGetComputeProgram(
    const std::filesystem::path &cShaderFileName,
    const utils::ShaderDefines &defines)
{
    auto name = cShaderFileName.string();
    for (const auto& define : defines)
        name += define.first + define.second;

    if (auto program = findResource(name))
    {
        auto computeProgram = std::dynamic_pointer_cast<graphics::IComputeProgram>(program);
        if (!computeProgram)
        {
            LOG_CRITICAL << "Program with name \"" << name << "\" was created in a different type";
            return nullptr;
        }

        return computeProgram;
    }

    auto cShader = utils::Shader::loadFromFile(cShaderFileName, defines);

    auto computeProgram = m_->renderer()->createComputeProgram(cShader);
    if (!computeProgram)
    {
        LOG_CRITICAL << "Failed to create render program: " << cShaderFileName;
        return nullptr;
    }

    addResource(name, computeProgram);
    return computeProgram;
}

}
}
