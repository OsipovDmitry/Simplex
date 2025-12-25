#ifndef CORE_RENDERPASSHELPERS_H
#define CORE_RENDERPASSHELPERS_H

#include <utils/clipspace.h>
#include <utils/transform.h>

#include "renderpass.h"

namespace simplex
{
namespace core
{

class CameraRenderInfo;

class FrustumCullingPass : public RenderPass
{
public:
    FrustumCullingPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<CameraRenderInfo>&);
    ~FrustumCullingPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

    void setFrustum(const utils::Transform&, const utils::ClipSpace&, const utils::Range&);

    graphics::PDrawArraysIndirectCommandsConstBuffer opaqueCommandsBuffer() const;
    graphics::PConstBufferRange opaqueParameterBuffer() const;

    graphics::PDrawArraysIndirectCommandsConstBuffer transparentCommandsBuffer() const;
    graphics::PConstBufferRange transparentParameterBuffer() const;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentCommandsBuffer;
    graphics::PBufferRange m_opaqueParameterBuffer;
    graphics::PBufferRange m_transparentParameterBuffer;
    std::shared_ptr<CameraRenderInfo> m_cameraRenderInfo;
    utils::Transform m_viewTransform;
    utils::ClipSpace m_clipSpace;
    utils::Range m_cullPlanesLimits;
};

class BuildClusterPass : public RenderPass
{
public:
    BuildClusterPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<CameraRenderInfo>&);
    ~BuildClusterPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

    void setCluster(const glm::uvec3&);

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    std::shared_ptr<CameraRenderInfo> m_cameraRenderInfo;

    glm::uvec3 m_clusterMaxSize;
};

class RenderDrawDataGeometryPass : public RenderPass
{
public:
    RenderDrawDataGeometryPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<const CameraRenderInfo>&,
        const graphics::PDrawArraysIndirectCommandsConstBuffer&,
        const graphics::PConstBufferRange&,
        const graphics::PDrawArraysIndirectCommandsConstBuffer&,
        const graphics::PConstBufferRange&);
    ~RenderDrawDataGeometryPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_opaqueProgram;
    std::shared_ptr<graphics::IRenderProgram> m_transparentProgram;
    std::shared_ptr<graphics::IComputeProgram> m_clearOITIndicesImageProgram;
    std::shared_ptr<graphics::IComputeProgram> m_sortOITNodesProgram;
    std::shared_ptr<const CameraRenderInfo> m_cameraRenderInfo;
    graphics::PDrawArraysIndirectCommandsConstBuffer m_opaqueCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsConstBuffer m_transparentCommandsBuffer;
    graphics::PConstBufferRange m_opaqueParameterBuffer;
    graphics::PConstBufferRange m_transparentParameterBuffer;
};

class ClusterLightPass : public RenderPass
{
public:
    ClusterLightPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<CameraRenderInfo>&);
    ~ClusterLightPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    std::shared_ptr<CameraRenderInfo> m_cameraRenderInfo;
};

class RenderBackgroundPass : public RenderPass
{
public:
    RenderBackgroundPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<const CameraRenderInfo>&);
    ~RenderBackgroundPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
    std::shared_ptr<const CameraRenderInfo> m_cameraRenderInfo;
};

//class BuildLightsCommandsBufferPass : public Pass
//{
//public:
//    BuildLightsCommandsBufferPass(const std::shared_ptr<ProgramsManager>&);
//    ~BuildLightsCommandsBufferPass() override;
//
//    void run(
//        const std::shared_ptr<graphics::RendererBase>&,
//        const std::shared_ptr<graphics::IFrameBuffer>&,
//        const std::shared_ptr<graphics::IVertexArray>&,
//        const std::shared_ptr<CameraRenderInfo>&,
//        const std::shared_ptr<const SceneData>&) override;
//
//    graphics::PDrawArraysIndirectCommandsConstBuffer lightsCommandsBuffer();
//
//private:
//    std::shared_ptr<graphics::IComputeProgram> m_program;
//    graphics::PDrawArraysIndirectCommandsBuffer m_lightsCommandsBuffer;
//};

//class RenderDrawDataStencilPass : public Pass
//{
//public:
//    RenderDrawDataStencilPass(
//        const std::shared_ptr<ProgramsManager>&,
//        const graphics::PDrawArraysIndirectCommandsConstBuffer&);
//    ~RenderDrawDataStencilPass() override;
//
//    void run(
//        const std::shared_ptr<graphics::RendererBase>&,
//        const std::shared_ptr<graphics::IFrameBuffer>&,
//        const std::shared_ptr<graphics::IVertexArray>&,
//        const std::shared_ptr<CameraRenderInfo>&,
//        const std::shared_ptr<const SceneData>&) override;
//
//private:
//    std::shared_ptr<graphics::IRenderProgram> m_program;
//    graphics::PDrawArraysIndirectCommandsConstBuffer m_lightsCommandsBuffer;
//};

class BlendPass : public RenderPass
{
public:
    BlendPass(const std::shared_ptr<ProgramsManager>&);
    ~BlendPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

private:
    std::shared_ptr<graphics::IRenderProgram> m_program;
};


}
}

#endif // CORE_RENDERPASSHELPERS_H