#ifndef CORE_RENDERPASSHELPERS_H
#define CORE_RENDERPASSHELPERS_H

#include "renderpass.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

class BuildDrawDataCommandsBufferPass : public Pass
{
public:
    BuildDrawDataCommandsBufferPass(const std::shared_ptr<ProgramsManager>&);
    ~BuildDrawDataCommandsBufferPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&,
        const std::shared_ptr<const SceneData>&) override;

    graphics::PDrawArraysIndirectCommandsConstBuffer opaqueCommandsBuffer() const;
    graphics::PDrawArraysIndirectCommandsConstBuffer transparentCommandsBuffer() const;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentCommandsBuffer;
};

class RenderDrawDataGeometryPass : public Pass
{
public:
    RenderDrawDataGeometryPass(
        const std::shared_ptr<ProgramsManager>&,
        const graphics::PDrawArraysIndirectCommandsConstBuffer&,
        const graphics::PDrawArraysIndirectCommandsConstBuffer&);
    ~RenderDrawDataGeometryPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_opaqueProgram;
    std::shared_ptr<graphics::IRenderProgram> m_transparentProgram;
    std::shared_ptr<graphics::IComputeProgram> m_clearOITIndicesImageProgram;
    std::shared_ptr<graphics::IComputeProgram> m_sortOITNodesProgram;
    graphics::PDrawArraysIndirectCommandsConstBuffer m_opaqueDrawDataCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsConstBuffer m_transparentDrawDataCommandsBuffer;
};

class RenderBackgroundPass : public Pass
{
public:
    RenderBackgroundPass(const std::shared_ptr<ProgramsManager>&);
    ~RenderBackgroundPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
};

class BuildLightsCommandsBufferPass : public Pass
{
public:
    BuildLightsCommandsBufferPass(const std::shared_ptr<ProgramsManager>&);
    ~BuildLightsCommandsBufferPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&,
        const std::shared_ptr<const SceneData>&) override;

    graphics::PDrawArraysIndirectCommandsConstBuffer lightsCommandsBuffer();

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    graphics::PDrawArraysIndirectCommandsBuffer m_lightsCommandsBuffer;
};

class RenderDrawDataStencilPass : public Pass
{
public:
    RenderDrawDataStencilPass(
        const std::shared_ptr<ProgramsManager>&,
        const graphics::PDrawArraysIndirectCommandsConstBuffer&);
    ~RenderDrawDataStencilPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
    graphics::PDrawArraysIndirectCommandsConstBuffer m_lightsCommandsBuffer;
};

class BlendPass : public Pass
{
public:
    BlendPass(const std::shared_ptr<ProgramsManager>&);
    ~BlendPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryRenderInfo>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
};


}
}

#endif // CORE_RENDERPASSHELPERS_H