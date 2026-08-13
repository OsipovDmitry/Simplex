#ifndef CORE_RENDERPIPELINE_H
#define CORE_RENDERPIPELINE_H

#include <memory>
#include <vector>

#include <utils/range.h>

#include <core/forwarddecl.h>
#include <core/shadowssettings.h>

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
using HighDynamicRangeBuffer = std::shared_ptr<graphics::StructBuffer<HDRDescription>>;
using ToneMappingBuffer = std::shared_ptr<graphics::StructBuffer<ToneMappingDescription>>;

class RenderPipeLine : public std::enable_shared_from_this<RenderPipeLine>
{
public:
    RenderPipeLine(uint32_t shadowAtlasSize);
    ~RenderPipeLine();

    void initialize(const std::shared_ptr<ProgramsLoader>&);

    void run(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&,
        const glm::uvec2& viewportSize,
        uint64_t time,
        uint32_t dt,
        float dielectricSpecular,
        const utils::OrientedBoundingBox&,
        const utils::Transform&,
        const utils::ClipSpace&,
        const utils::Range&,
        const glm::uvec3&);

    const glm::uvec2& viewportSize() const;

    uint32_t shadowAtlasSize() const;
    ShadowFilter shadowFilter() const;

    void setShadowFilter(ShadowFilter);
    void setShadowBlurSigma(float);
    void setShadowLightBleedingAmount(float);
    void setShadowPositiveExponent(float);
    void setShadowNegativeExponent(float);
    void setShadowMomentsBias(float);
    void setShadowDepthBiasFactor(float);
    void setShadowCascadesBlendDistanceFactor(float);
    void setShadowCascadesDistancePower(float);

    void setBloomEnabled(bool);
    void setBloomContribution(float);
    void setBloomPassesCount(uint32_t);

    void setToneMappingLuminanceRange(const utils::Range&);
    void setToneMappingLuminanceClampRange(const utils::Range&);
    void setToneMappingPixelsFractionToTrim(const std::pair<float, float>&);
    void setToneMappingTauLight(float);
    void setToneMappingTauDark(float);
    void setToneMappingBaseLumiance(float);

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
    HighDynamicRangeBuffer& highDynamicRangeBuffer();
    ToneMappingBuffer& toneMappingBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& bonesTransformsDataCalculateCommandBuffer();
    graphics::PDrawElementsIndirectCommandBuffer& opaqueDrawDataRenderCommandsBuffer();
    graphics::PDrawElementsIndirectCommandBuffer& transparentDrawDataRenderCommandsBuffer();
    graphics::PBufferRange& opaqueDrawDataRenderParameterBuffer();
    graphics::PBufferRange& transparentDrawDataRenderParameterBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& clusterLocalLightsCommandBuffer();
    graphics::PDispatchComputeIndirectCommandBuffer& shadowDataCullCommandBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& shadowMapBlurCommandsBuffer();
    graphics::PDrawElementsIndirectCommandBuffer& opaqueShadowDataRenderCommandsBuffer();
    graphics::PDrawElementsIndirectCommandBuffer& transparentShadowDataRenderCommandsBuffer();
    graphics::PBufferRange& opaqueShadowDataRenderParameterBuffer();
    graphics::PBufferRange& transparentShadowDataRenderParameterBuffer();

    graphics::PConstTexture shadowDepthTexture() const;
    graphics::PConstTexture shadowMomentsTexture() const;
    graphics::PConstTexture shadowColorTexture() const;
    graphics::PConstTexture shadowMomentsBluredTexture() const;
    graphics::PConstTexture shadowColorBluredTexture() const;
    graphics::PConstTexture highDynamicRangeTexture() const;
    graphics::PConstTexture finalTexture() const;

    glm::vec4 shadowMomentsTextureClearColor() const;

private:
    void deinitialize();
    void dirtyShadowMapsBuffer();
    void dirtyHDRBuffer();
    void dirtyToneMappingBuffer();

    bool isShadowBlurPassNeeded() const;
    graphics::PixelInternalFormat shadowMomentsTextureInternalFormat() const;
    std::vector<float> calculateShadowBlurKernel() const;

    void resizeShadowTextures(const std::shared_ptr<graphics::RendererBase>&, uint32_t);
    void updateShadowMapsBuffer();

    void resizeHDRTexture(const std::shared_ptr<graphics::RendererBase>&);
    void updateHDRBuffer();

    void updateToneMappingBuffer();

    void resizeFinalTexture(const std::shared_ptr<graphics::RendererBase>&);

    bool m_isInitialized = false;
    bool m_isShadowMapsBufferDirty = true;
    bool m_isHDRBufferDirty = true;
    bool m_isBloomBufferDirty = true;
    bool m_isToneMappingBufferDirty = true;

    glm::uvec2 m_viewportSize = glm::uvec2(0u, 0u);

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

    bool m_isBloomEnabled = false;
    float m_bloomContribution = .02f;
    uint32_t m_bloomPassesCount = 4u;

    utils::Range m_toneMappingLuminanceRange = utils::Range(glm::exp2(glm::vec2(-5.f, 10.0f)));
    utils::Range m_toneMappingLuminanceClampRange = utils::Range(glm::vec2(0.02f, 12.0f));
    std::pair<float, float> m_toneMappingPixelsFractionToTrim{0.05f, 0.05f};
    float m_toneMappingTauLight = 2.f;
    float m_toneMappingTauDark = 1.f;
    float m_toneMappingBaseLuminance = .18f;

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
    HighDynamicRangeBuffer m_HDRBuffer;
    ToneMappingBuffer m_toneMappingBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_bonesTransformsDataCalculateCommandBuffer;
    graphics::PDrawElementsIndirectCommandBuffer m_opaqueDrawDataRenderCommandsBuffer;
    graphics::PDrawElementsIndirectCommandBuffer m_transparentDrawDataRenderCommandsBuffer;
    graphics::PBufferRange m_opaqueDrawDataRenderParameterBuffer;
    graphics::PBufferRange m_transparentDrawDataRenderParameterBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_clusterLocalLightsCommandBuffer;
    graphics::PDispatchComputeIndirectCommandBuffer m_shadowDataCullCommandBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_shadowMapBlurCommandsBuffer;
    graphics::PDrawElementsIndirectCommandBuffer m_opaqueShadowDataRenderCommandsBuffer;
    graphics::PDrawElementsIndirectCommandBuffer m_transparentShadowDataRenderCommandsBuffer;
    graphics::PBufferRange m_opaqueShadowDataRenderParameterBuffer;
    graphics::PBufferRange m_transparentShadowDataRenderParameterBuffer;

    graphics::PTextureHandle m_shadowDepthTextureHandle;
    graphics::PTextureHandle m_shadowMomentsTextureHandle;
    graphics::PTextureHandle m_shadowColorTextureHandle;
    graphics::PTextureHandle m_shadowMomentsBluredTextureHandle;
    graphics::PTextureHandle m_shadowColorBluredTextureHandle;
    graphics::PTextureHandle m_HDRTextureHandle;
    graphics::PTexture m_finalTexture;

    std::vector<std::shared_ptr<RenderPass>> m_passes;
};

} // namespace core
} // namespace simplex

#endif // CORE_RENDERPIPELINE_H