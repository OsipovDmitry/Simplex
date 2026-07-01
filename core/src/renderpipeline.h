#ifndef CORE_RENDERPIPELINE_H
#define CORE_RENDERPIPELINE_H

#include <memory>
#include <vector>

#include <core/forwarddecl.h>

#include "descriptions.h"

namespace simplex
{
namespace core
{

class GeometryBuffer;
class ProgramsLoader;
class RenderPass;
class SceneData;

using RenderInfoBuffer = std::shared_ptr<graphics::StructBuffer<RenderInfoDescription>>;
using CountersBuffer = std::shared_ptr<graphics::StructBuffer<CountersDescription>>;
using CameraBuffer = std::shared_ptr<graphics::StructBuffer<CameraDescription>>;
using ClusterNodesBuffer = std::shared_ptr<graphics::VectorBuffer<ClusterNodeDescription>>;
using ClusterLocalLightsBuffer = std::shared_ptr<graphics::VectorBuffer<ClusterLocalLightDescription>>;
using LightNodesBuffer = std::shared_ptr<graphics::VectorBuffer<LightNodeDescription>>;
using SkeletalAnimatedDataToUpdateBuffer = std::shared_ptr<graphics::VectorBuffer<SkeletalAnimatedDataToUpdateDescription>>;
using ShadowsToUpdateBuffer = std::shared_ptr<graphics::VectorBuffer<ShadowToUpdateDescription>>;
using ShadowDataBuffer = std::shared_ptr<graphics::VectorBuffer<ShadowDataDescription>>;

class RenderPipeLine
{
public:
    ~RenderPipeLine();

    void run(
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&,
        uint64_t time,
        float dielectricSpecular,
        const utils::OrientedBoundingBox&,
        const utils::Transform&,
        const utils::ClipSpace&,
        const utils::Range&,
        const glm::uvec3&);

    RenderInfoBuffer& renderInfoBuffer();
    CountersBuffer& countersBuffer();
    CameraBuffer& cameraBuffer();
    ClusterNodesBuffer& clusterNodesBuffer();
    ClusterLocalLightsBuffer& clusterLocalLightsBuffer();
    LightNodesBuffer& lightNodesBuffer();
    SkeletalAnimatedDataToUpdateBuffer& skeletalAnimatedDataToUpdateBuffer();
    ShadowsToUpdateBuffer& shadowsToUpdateBuffer();
    ShadowDataBuffer& shadowDataBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& bonesTransformsDataCalculateCommandBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& opaqueDrawDataRenderCommandsBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& transparentDrawDataRenderCommandsBuffer();
    graphics::PBufferRange& opaqueDrawDataRenderParameterBuffer();
    graphics::PBufferRange& transparentDrawDataRenderParameterBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& clusterLocalLightsCommandBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& shadowDataCullCommandBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& shadowMapBlurCommandsBuffer();
    graphics::PTextureHandle& shadowMomentsBluredTextureHandle();
    graphics::PTextureHandle& shadowColorBluredTextureHandle();
    graphics::PDrawArraysIndirectCommandsBuffer& opaqueShadowDataRenderCommandsBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& transparentShadowDataRenderCommandsBuffer();
    graphics::PBufferRange& opaqueShadowDataRenderParameterBuffer();
    graphics::PBufferRange& transparentShadowDataRenderParameterBuffer();

    graphics::PConstTexture finalTexture() const;

    static std::shared_ptr<RenderPipeLine> create(
        const std::shared_ptr<ProgramsLoader>&,
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&);

private:
    RenderPipeLine(
        const std::shared_ptr<ProgramsLoader>&,
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&);

    void resizeShadowMomentsBluredTexture(const graphics::PConstTexture&);
    void resizeShadowColorBluredTexture(const graphics::PConstTexture&);
    void resizeFinalTexture(const glm::uvec2&);
    std::vector<float> calculateShadowBlurKernel(float);

    RenderInfoBuffer m_renderInfoBuffer;
    CountersBuffer m_countersBuffer;
    CameraBuffer m_cameraBuffer;
    ClusterNodesBuffer m_clusterNodesBuffer;
    ClusterLocalLightsBuffer m_clusterLocalLightsBuffer;
    LightNodesBuffer m_lightNodesBuffer;
    SkeletalAnimatedDataToUpdateBuffer m_skeletalAnimatedDataToUpdateBuffer;
    ShadowsToUpdateBuffer m_shadowsToUpdateBuffer;
    ShadowDataBuffer m_shadowDataBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_bonesTransformsDataCalculateCommandBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueDrawDataRenderCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentDrawDataRenderCommandsBuffer;
    graphics::PBufferRange m_opaqueDrawDataRenderParameterBuffer;
    graphics::PBufferRange m_transparentDrawDataRenderParameterBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_clusterLocalLightsCommandBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_shadowDataCullCommandBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_shadowMapBlurCommandsBuffer;
    graphics::PTextureHandle m_shadowMomentsBluredTextureHandle;
    graphics::PTextureHandle m_shadowColorBluredTextureHandle;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueShadowDataRenderCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentShadowDataRenderCommandsBuffer;
    graphics::PBufferRange m_opaqueShadowDataRenderParameterBuffer;
    graphics::PBufferRange m_transparentShadowDataRenderParameterBuffer;
    graphics::PTexture m_finalTexture;

    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;

    std::vector<std::shared_ptr<RenderPass>> m_passes;
};

} // namespace core
} // namespace simplex

#endif // CORE_RENDERPIPELINE_H