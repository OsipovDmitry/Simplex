#ifndef CORE_RENDERPASSHELPERS_H
#define CORE_RENDERPASSHELPERS_H

#include "renderpass.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

class BuildBackgroundsCommandsBufferPass : public Pass
{
public:
    BuildBackgroundsCommandsBufferPass(
        const std::shared_ptr<ProgramsManager>&,
        const MeshesBuffer&,
        const BackgroundsBuffer&,
        const CommandsBuffer&);
    ~BuildBackgroundsCommandsBufferPass() override;

    void run(const std::shared_ptr<graphics::RendererBase>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    CommandsBuffer m_commandsBuffer;
};

class BuildDrawDataCommandsBufferPass : public Pass
{
public:
    BuildDrawDataCommandsBufferPass(
        const std::shared_ptr<ProgramsManager>&,
        const MeshesBuffer&,
        const DrawablesBuffer&,
        const DrawDataBuffer&,
        const CommandsBuffer&);
    ~BuildDrawDataCommandsBufferPass() override;

    void run(const std::shared_ptr<graphics::RendererBase>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    CommandsBuffer m_commandsBuffer;
};

}
}

#endif // CORE_RENDERPASSHELPERS_H