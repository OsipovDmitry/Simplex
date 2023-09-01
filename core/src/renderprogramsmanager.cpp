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

simplex::core::RenderProgramsManager::~RenderProgramsManager()
{
}

std::shared_ptr<graphics::IRenderProgram> RenderProgramsManager::loadOrGet(const std::filesystem::path &vShaderFileName,
                                                                           const std::filesystem::path &fShaderFileName,
                                                                           const utils::ShaderDefines &defines)
{
    auto name = vShaderFileName.string() + fShaderFileName.string();
    for (const auto &define : defines)
        name += define.first + define.second;

    if (auto it = m_->resources().find(name); it != m_->resources().end())
        return it->second;

    auto vShader = utils::Shader::loadShader(vShaderFileName, defines);
    auto fShader = utils::Shader::loadShader(fShaderFileName, defines);

    auto renderProgram = m_->renderer()->createRenderProgram(*vShader, *fShader);
    m_->resources()[name] = renderProgram;

    return renderProgram;
}



}
}
