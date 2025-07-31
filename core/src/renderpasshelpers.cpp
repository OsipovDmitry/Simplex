#include <core/programsmanager.h>
#include <core/graphicsrendererbase.h>

#include "resources.h"
#include "renderpasshelpers.h"

namespace simplex
{
namespace core
{

BuildBackgroundsCommandsBufferPass::BuildBackgroundsCommandsBufferPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const MeshesBuffer& meshesBuffer,
    const BackgroundsBuffer& backgroundsBuffer,
    const CommandsBuffer& commandsBuffer)
    : Pass()
    , m_commandsBuffer(commandsBuffer)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildBackgroundsCommandsBufferPassComputeProgramPath, {});

    getOrCreateSSBO(SSBOId::MeshesBuffer) = meshesBuffer->buffer();
    getOrCreateSSBO(SSBOId::BackgroundsBuffer) = backgroundsBuffer->buffer();
    getOrCreateSSBO(SSBOId::CommandsBuffer) = commandsBuffer->buffer();
}

BuildBackgroundsCommandsBufferPass::~BuildBackgroundsCommandsBufferPass() = default;

void BuildBackgroundsCommandsBufferPass::run(const std::shared_ptr<graphics::RendererBase>& renderer)
{
    m_commandsBuffer->setReservedData(0u);
    renderer->compute(m_program, glm::uvec3(m_commandsBuffer->size(), 1u, 1u), shared_from_this());
}

BuildDrawDataCommandsBufferPass::BuildDrawDataCommandsBufferPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const MeshesBuffer& meshesBuffer,
    const DrawablesBuffer& drawablesBuffer,
    const DrawDataBuffer& drawDataBuufer,
    const CommandsBuffer& commandsBuffer)
    : Pass()
    , m_commandsBuffer(commandsBuffer)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildDrawDataCommandsBufferPassComputeProgramPath, {});

    getOrCreateSSBO(SSBOId::MeshesBuffer) = meshesBuffer->buffer();
    getOrCreateSSBO(SSBOId::DrawablesBuffer) = drawablesBuffer->buffer();
    getOrCreateSSBO(SSBOId::DrawDataBuffer) = drawDataBuufer->buffer();
    getOrCreateSSBO(SSBOId::CommandsBuffer) = commandsBuffer->buffer();
}

BuildDrawDataCommandsBufferPass::~BuildDrawDataCommandsBufferPass() = default;

void BuildDrawDataCommandsBufferPass::run(const std::shared_ptr<graphics::RendererBase>& renderer)
{
    m_commandsBuffer->setReservedData(0u);
    renderer->compute(m_program, glm::uvec3(m_commandsBuffer->size(), 1u, 1u), shared_from_this());
}

}
}