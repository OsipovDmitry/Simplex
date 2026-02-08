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

class FrustumCullingPass : public RenderPass
{
public:
    FrustumCullingPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
    ~FrustumCullingPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
};

class UpdateCameraInfoPass : public RenderPass
{
public:
    UpdateCameraInfoPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
    ~UpdateCameraInfoPass() override;

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
    CalculateBonesTransformsDataPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
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

class RenderDrawDataGeometryPass : public RenderPass
{
public:
    RenderDrawDataGeometryPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
    ~RenderDrawDataGeometryPass() override;

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
    BuildClusterPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
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
    ClusterLightPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
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

class RenderBackgroundPass : public RenderPass
{
public:
    RenderBackgroundPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
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
    BlendPass(const std::shared_ptr<ProgramsManager>&, const std::shared_ptr<RenderPipeLine>&);
    ~BlendPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;

    SceneInfoBuffer m_sceneInfoBuffer;
    ClusterNodesBuffer m_clusterNodesBuffer;
    CameraBuffer m_cameraBuffer;
};


}
}

#endif // CORE_RENDERPASSHELPERS_H