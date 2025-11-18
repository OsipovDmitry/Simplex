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
    FrustumCullingPass(const std::shared_ptr<ProgramsManager>&);
    ~FrustumCullingPass() override;

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&) override;

    void setFrustum(const utils::Transform&, const utils::ClipSpace&, const utils::Range&);

    std::shared_ptr<const CameraRenderInfo> cameraRenderInfo() const;

    graphics::PDrawArraysIndirectCommandsConstBuffer opaqueCommandsBuffer() const;
    graphics::PDrawArraysIndirectCommandsConstBuffer transparentCommandsBuffer() const;

private:
    std::shared_ptr<graphics::IComputeProgram> m_program;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentCommandsBuffer;
    std::shared_ptr<graphics::StructBuffer<glm::uvec2>> m_ZRangeBuffer;
    std::shared_ptr<CameraRenderInfo> m_cameraRenderInfo;
    utils::Transform m_viewTransform;
    utils::ClipSpace m_clipSpace;
    utils::Range m_cullPlanesLimits;
};

class RenderDrawDataGeometryPass : public RenderPass
{
public:
    RenderDrawDataGeometryPass(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<const CameraRenderInfo>&,
        const graphics::PDrawArraysIndirectCommandsConstBuffer&,
        const graphics::PDrawArraysIndirectCommandsConstBuffer&);
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
    graphics::PDrawArraysIndirectCommandsConstBuffer m_opaqueDrawDataCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsConstBuffer m_transparentDrawDataCommandsBuffer;
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