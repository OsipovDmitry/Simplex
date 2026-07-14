#ifndef CORE_RENDERPIPELINE_H
#define CORE_RENDERPIPELINE_H

#include <memory>
#include <vector>

#include <core/cameranode.h>
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
using ShadowMapsBuffer = std::shared_ptr<graphics::StructBuffer<ShadowMapsDescription>>;

class RenderPipeLine : public std::enable_shared_from_this<RenderPipeLine>
{
public:
    RenderPipeLine(uint32_t shadowAtlasSize);
    ~RenderPipeLine();

    void initialize(const std::shared_ptr<ProgramsLoader>&);
    void deinitialize();

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&,
        uint64_t time,
        float dielectricSpecular,
        const utils::OrientedBoundingBox&,
        const utils::Transform&,
        const utils::ClipSpace&,
        const utils::Range&,
        const glm::uvec3&);

    uint32_t shadowAtlasSize() const;
    ShadowFilter& shadowFilter();
    float& shadowBlurSigma();
    float& shadowLightBleedingAmount();
    float& shadowPositiveExponent();
    float& shadowNegativeExponent();
    float& shadowMomentsBias();
    float& shadowDepthBiasFactor();
    float& shadowCascadesBlendDistanceFactor();
    float& shadowCascadesDistancePower();

    RenderInfoBuffer& renderInfoBuffer();
    CountersBuffer& countersBuffer();
    CameraBuffer& cameraBuffer();
    ClusterNodesBuffer& clusterNodesBuffer();
    ClusterLocalLightsBuffer& clusterLocalLightsBuffer();
    LightNodesBuffer& lightNodesBuffer();
    SkeletalAnimatedDataToUpdateBuffer& skeletalAnimatedDataToUpdateBuffer();
    ShadowsToUpdateBuffer& shadowsToUpdateBuffer();
    ShadowDataBuffer& shadowDataBuffer();
    ShadowMapsBuffer& shadowMapsBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& bonesTransformsDataCalculateCommandBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& opaqueDrawDataRenderCommandsBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& transparentDrawDataRenderCommandsBuffer();
    graphics::PBufferRange& opaqueDrawDataRenderParameterBuffer();
    graphics::PBufferRange& transparentDrawDataRenderParameterBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& clusterLocalLightsCommandBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& shadowDataCullCommandBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& shadowMapBlurCommandsBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& opaqueShadowDataRenderCommandsBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& transparentShadowDataRenderCommandsBuffer();
    graphics::PBufferRange& opaqueShadowDataRenderParameterBuffer();
    graphics::PBufferRange& transparentShadowDataRenderParameterBuffer();

    graphics::PConstTexture shadowDepthTexture() const;
    graphics::PConstTexture shadowMomentsTexture() const;
    graphics::PConstTexture shadowColorTexture() const;
    graphics::PConstTexture shadowMomentsBluredTexture() const;
    graphics::PConstTexture shadowColorBluredTexture() const;
    graphics::PConstTexture finalTexture() const;

    glm::vec4 shadowMomentsTextureClearColor() const;
    bool shadowIsBlurPassNeeded() const;

private:
    graphics::PixelInternalFormat shadowMomentsTextureInternalFormat() const;
    std::vector<float> calculateShadowBlurKernel() const;

    void resizeShadowTextures(const std::shared_ptr<graphics::RendererBase>&, uint32_t);
    void updateShadowMapsBuffer();

    void resizeFinalTexture(const std::shared_ptr<graphics::RendererBase>&, const glm::uvec2&);

    bool m_isInitialized = false;
    uint32_t m_shadowAtlasSize = 0u;
    ShadowFilter m_shadowFilter = ShadowFilter::Discrete;
    float m_shadowBlurSigma = 1.f;
    float m_shadowLightBleedingAmount = .2f;
    float m_shadowPositiveExponent = 1.f;
    float m_shadowNegativeExponent = 1.f;
    float m_shadowMomentsBias = 0.f;
    float m_shadowDepthBiasFactor = 0.f;
    float m_shadowCascadesBlendDistanceFactor = .15f;
    float m_shadowCascadesDistancePower = 1.5f;

    RenderInfoBuffer m_renderInfoBuffer;
    CountersBuffer m_countersBuffer;
    CameraBuffer m_cameraBuffer;
    ClusterNodesBuffer m_clusterNodesBuffer;
    ClusterLocalLightsBuffer m_clusterLocalLightsBuffer;
    LightNodesBuffer m_lightNodesBuffer;
    SkeletalAnimatedDataToUpdateBuffer m_skeletalAnimatedDataToUpdateBuffer;
    ShadowsToUpdateBuffer m_shadowsToUpdateBuffer;
    ShadowDataBuffer m_shadowDataBuffer;
    ShadowMapsBuffer m_shadowMapsBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_bonesTransformsDataCalculateCommandBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueDrawDataRenderCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentDrawDataRenderCommandsBuffer;
    graphics::PBufferRange m_opaqueDrawDataRenderParameterBuffer;
    graphics::PBufferRange m_transparentDrawDataRenderParameterBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_clusterLocalLightsCommandBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_shadowDataCullCommandBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_shadowMapBlurCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueShadowDataRenderCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentShadowDataRenderCommandsBuffer;
    graphics::PBufferRange m_opaqueShadowDataRenderParameterBuffer;
    graphics::PBufferRange m_transparentShadowDataRenderParameterBuffer;

    graphics::PTextureHandle m_shadowDepthTextureHandle;
    graphics::PTextureHandle m_shadowMomentsTextureHandle;
    graphics::PTextureHandle m_shadowColorTextureHandle;
    graphics::PTextureHandle m_shadowMomentsBluredTextureHandle;
    graphics::PTextureHandle m_shadowColorBluredTextureHandle;
    graphics::PTexture m_finalTexture;

    std::vector<std::shared_ptr<RenderPass>> m_passes;
};

} // namespace core
} // namespace simplex

#endif // CORE_RENDERPIPELINE_H