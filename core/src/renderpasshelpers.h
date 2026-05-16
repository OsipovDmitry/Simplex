#ifndef CORE_RENDERPASSHELPERS_H
#define CORE_RENDERPASSHELPERS_H

#include <utils/clipspace.h>
#include <utils/transform.h>

#include "renderpass.h"
#include "renderpipeline.h"

namespace simplex
{
namespace core
{

class SimplePass : public RenderPass
{
public:
    using RunMethod = std::function<void(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&)>;

    SimplePass(const std::shared_ptr<RenderPipeLine>&, const RunMethod&);
    ~SimplePass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    RunMethod m_runMethod;
};

class InitializeCameraPass : public RenderPass
{
public:
    InitializeCameraPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~InitializeCameraPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class CullDrawDataPass : public RenderPass
{
public:
    CullDrawDataPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~CullDrawDataPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class CollectSkeletalAnimatedDataToUpdatePass : public RenderPass
{
public:
    CollectSkeletalAnimatedDataToUpdatePass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~CollectSkeletalAnimatedDataToUpdatePass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class UpdateCameraPass : public RenderPass
{
public:
    UpdateCameraPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~UpdateCameraPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class PrepareBonesTransformsDataCalculateCommandPass : public RenderPass
{
public:
    PrepareBonesTransformsDataCalculateCommandPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~PrepareBonesTransformsDataCalculateCommandPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class CalculateBonesTransformsDataPass : public RenderPass
{
public:
    CalculateBonesTransformsDataPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~CalculateBonesTransformsDataPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class RenderDrawDataPass : public RenderPass
{
public:
    RenderDrawDataPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~RenderDrawDataPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_opaqueProgram;
    std::shared_ptr<graphics::IRenderProgram> m_transparentProgram;
};

class BuildClusterPass : public RenderPass
{
public:
    BuildClusterPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~BuildClusterPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class ClusterLightPass : public RenderPass
{
public:
    ClusterLightPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~ClusterLightPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class PrepareShadowDataCullCommnadPass : public RenderPass
{
public:
    PrepareShadowDataCullCommnadPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~PrepareShadowDataCullCommnadPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class PrepareShadowMapBlurCommandsPass : public RenderPass
{
public:
    PrepareShadowMapBlurCommandsPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~PrepareShadowMapBlurCommandsPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class CullShadowDataPass : public RenderPass
{
public:
    CullShadowDataPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~CullShadowDataPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    graphics::PBufferRange m_opaqueBlackShadowDataRenderCommandsBuffer;
    graphics::PBufferRange m_transparentBlackShadowDataRenderCommandsBuffer;
    graphics::PBufferRange m_opaqueColorShadowDataRenderCommandsBuffer;
    graphics::PBufferRange m_transparentColorShadowDataRenderCommandsBuffer;
};

class RenderShadowDataPass : public RenderPass
{
public:
    RenderShadowDataPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~RenderShadowDataPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_opaqueProgram;
    std::shared_ptr<graphics::IRenderProgram> m_transparentProgram;
};

class BlurShadowMapPass : public RenderPass
{
public:
    BlurShadowMapPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~BlurShadowMapPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_horizontalProgram;
    std::shared_ptr<graphics::IRenderProgram> m_verticalProgram;
};

class RenderBackgroundPass : public RenderPass
{
public:
    RenderBackgroundPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~RenderBackgroundPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
};

class BlendPass : public RenderPass
{
public:
    BlendPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~BlendPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
};

class FinalPass : public RenderPass
{
public:
    FinalPass(const std::shared_ptr<ProgramsLoader>&, const std::shared_ptr<RenderPipeLine>&);
    ~FinalPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
};

} // namespace core
} // namespace simplex

#endif // CORE_RENDERPASSHELPERS_H