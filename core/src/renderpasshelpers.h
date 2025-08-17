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
        const std::shared_ptr<SceneData>&);
    ~BuildBackgroundsCommandsBufferPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    DrawIndirectElementsCommandsBuffer m_commandsBuffer;
};

class BuildDrawDataCommandsBufferPass : public Pass
{
public:
    BuildDrawDataCommandsBufferPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<SceneData>&);
    ~BuildDrawDataCommandsBufferPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    DrawDataBuffer m_drawDataBuffer;
    DrawIndirectElementsCommandsBuffer m_opaqueCommandsBuffer;
    DrawIndirectElementsCommandsBuffer m_transparentCommandsBuffer;
};

class RenderOpaqueDrawDataGeometryPass : public Pass
{
public:
    RenderOpaqueDrawDataGeometryPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<SceneData>&);
    ~RenderOpaqueDrawDataGeometryPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
    graphics::PBufferRange m_drawIndirectBufferRange;
    graphics::PBufferRange m_parameterBufferRange;
};

}
}

#endif // CORE_RENDERPASSHELPERS_H