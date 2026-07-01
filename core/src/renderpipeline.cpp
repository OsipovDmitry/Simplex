#include "renderpipeline.h"

#include <utils/glm/gtx/functions.hpp>
#include <utils/logger.h>

#include <core/graphicsrendererbase.h>

#include "geometrybuffer.h"
#include "renderpasshelpers.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

RenderPipeLine::~RenderPipeLine() = default;

void RenderPipeLine::run(
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData,
    uint64_t time,
    float dielectricSpecular,
    const utils::OrientedBoundingBox& globalBoundingBox,
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& cullPlaneLimits,
    const glm::uvec3& clusterSize)
{
    const auto drawDataCount = sceneData->drawDataCount();
    m_opaqueDrawDataRenderCommandsBuffer->resize(drawDataCount);
    m_transparentDrawDataRenderCommandsBuffer->resize(drawDataCount);

    const auto skeletalAnimatedDataCount = sceneData->skeletalAnimatedDataCount();
    m_skeletalAnimatedDataToUpdateBuffer->resize(skeletalAnimatedDataCount);

    const auto shadowsCount = sceneData->shadowsCount();
    m_shadowsToUpdateBuffer->resize(shadowsCount);

    const auto clusterNodesCount = glm::compMul(clusterSize);
    m_clusterNodesBuffer->resize(clusterNodesCount);

    const auto lightsCount = sceneData->lightsCount();
    m_clusterLocalLightsBuffer->resize(lightsCount);

    const auto lightNodesCount = clusterNodesCount * lightsCount;
    m_lightNodesBuffer->resize(lightNodesCount);

    const auto shadowDataCount = drawDataCount * shadowsCount;
    m_shadowDataBuffer->resize(shadowDataCount);
    m_opaqueShadowDataRenderCommandsBuffer->resize(shadowDataCount);
    m_transparentShadowDataRenderCommandsBuffer->resize(shadowDataCount);

    resizeShadowMomentsBluredTexture(sceneData->shadowMomentsTexture());
    resizeShadowColorBluredTexture(sceneData->shadowColorTexture());
    resizeFinalTexture(geometryBuffer->size());

    const auto shadowBlurKernel = calculateShadowBlurKernel(sceneData->shadowBlurSigma());
    const auto shadowAtlasSize = sceneData->shadowAtlasSize();
    const auto shadowLightBleedingAmount = sceneData->shadowLightBleedingAmount();

    const graphics::TextureHandle shadowMomentsBluredTextureHandle = m_shadowMomentsBluredTextureHandle
                                                                         ? m_shadowMomentsBluredTextureHandle->handle()
                                                                         : utils::IDsGeneratorT<graphics::TextureHandle>::last();

    const graphics::TextureHandle shadowColorBluredTextureHandle = m_shadowColorBluredTextureHandle
                                                                       ? m_shadowColorBluredTextureHandle->handle()
                                                                       : utils::IDsGeneratorT<graphics::TextureHandle>::last();

    m_renderInfoBuffer->set(RenderInfoDescription::make(
        static_cast<uint32_t>(time), dielectricSpecular, globalBoundingBox, static_cast<uint32_t>(drawDataCount),
        static_cast<uint32_t>(skeletalAnimatedDataCount), static_cast<uint32_t>(shadowsCount), static_cast<uint32_t>(lightsCount),
        shadowMomentsBluredTextureHandle, shadowColorBluredTextureHandle, shadowBlurKernel, shadowLightBleedingAmount,
        shadowAtlasSize, clusterSize, viewTransform, clipSpace, cullPlaneLimits));

    for (auto& pass : m_passes)
        pass->run(m_renderer, m_frameBuffer, m_vertexArray, geometryBuffer, sceneData);
}

RenderInfoBuffer& RenderPipeLine::renderInfoBuffer()
{
    return m_renderInfoBuffer;
}

CountersBuffer& RenderPipeLine::countersBuffer()
{
    return m_countersBuffer;
}

CameraBuffer& RenderPipeLine::cameraBuffer()
{
    return m_cameraBuffer;
}

ClusterNodesBuffer& RenderPipeLine::clusterNodesBuffer()
{
    return m_clusterNodesBuffer;
}

ClusterLocalLightsBuffer& RenderPipeLine::clusterLocalLightsBuffer()
{
    return m_clusterLocalLightsBuffer;
}

LightNodesBuffer& RenderPipeLine::lightNodesBuffer()
{
    return m_lightNodesBuffer;
}

SkeletalAnimatedDataToUpdateBuffer& RenderPipeLine::skeletalAnimatedDataToUpdateBuffer()
{
    return m_skeletalAnimatedDataToUpdateBuffer;
}

ShadowsToUpdateBuffer& RenderPipeLine::shadowsToUpdateBuffer()
{
    return m_shadowsToUpdateBuffer;
}

ShadowDataBuffer& RenderPipeLine::shadowDataBuffer()
{
    return m_shadowDataBuffer;
}

graphics::PDispatchComputeIndirectCommandBuffer& RenderPipeLine::bonesTransformsDataCalculateCommandBuffer()
{
    return m_bonesTransformsDataCalculateCommandBuffer;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::opaqueDrawDataRenderCommandsBuffer()
{
    return m_opaqueDrawDataRenderCommandsBuffer;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::transparentDrawDataRenderCommandsBuffer()
{
    return m_transparentDrawDataRenderCommandsBuffer;
}

graphics::PBufferRange& RenderPipeLine::opaqueDrawDataRenderParameterBuffer()
{
    return m_opaqueDrawDataRenderParameterBuffer;
}

graphics::PBufferRange& RenderPipeLine::transparentDrawDataRenderParameterBuffer()
{
    return m_transparentDrawDataRenderParameterBuffer;
}

graphics::PDispatchComputeIndirectCommandBuffer& RenderPipeLine::clusterLocalLightsCommandBuffer()
{
    return m_clusterLocalLightsCommandBuffer;
}

graphics::PDispatchComputeIndirectCommandBuffer& RenderPipeLine::shadowDataCullCommandBuffer()
{
    return m_shadowDataCullCommandBuffer;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::opaqueShadowDataRenderCommandsBuffer()
{
    return m_opaqueShadowDataRenderCommandsBuffer;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::transparentShadowDataRenderCommandsBuffer()
{
    return m_transparentShadowDataRenderCommandsBuffer;
}

graphics::PBufferRange& RenderPipeLine::opaqueShadowDataRenderParameterBuffer()
{
    return m_opaqueShadowDataRenderParameterBuffer;
}

graphics::PBufferRange& RenderPipeLine::transparentShadowDataRenderParameterBuffer()
{
    return m_transparentShadowDataRenderParameterBuffer;
}

graphics::PConstTexture RenderPipeLine::finalTexture() const
{
    return m_finalTexture;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::shadowMapBlurCommandsBuffer()
{
    return m_shadowMapBlurCommandsBuffer;
}

graphics::PTextureHandle& RenderPipeLine::shadowMomentsBluredTextureHandle()
{
    return m_shadowMomentsBluredTextureHandle;
}

graphics::PTextureHandle& RenderPipeLine::shadowColorBluredTextureHandle()
{
    return m_shadowColorBluredTextureHandle;
}

std::shared_ptr<RenderPipeLine> RenderPipeLine::create(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray)
{
    static const auto clear =
        [](const std::shared_ptr<graphics::RendererBase>& renderer, const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
           const std::shared_ptr<graphics::IVertexArray>&, const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
           const std::shared_ptr<const SceneData>&)
    {
        geometryBuffer->clear(renderer, frameBuffer);
    };

    static const auto sort = [](const std::shared_ptr<graphics::RendererBase>& renderer,
                                const std::shared_ptr<graphics::IFrameBuffer>&, const std::shared_ptr<graphics::IVertexArray>&,
                                const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
                                const std::shared_ptr<const SceneData>&)
    {
        geometryBuffer->sortOITNodes(renderer);
    };

    auto result = std::shared_ptr<RenderPipeLine>(new RenderPipeLine(programsManager, renderer, frameBuffer, vertexArray));

    auto& passes = result->m_passes;
    passes.push_back(std::make_shared<InitializeCameraPass>(programsManager, result));
    passes.push_back(std::make_shared<CullDrawDataPass>(programsManager, result));
    passes.push_back(std::make_shared<CollectSkeletalAnimatedDataToUpdatePass>(programsManager, result));
    passes.push_back(std::make_shared<UpdateCameraPass>(programsManager, result));
    passes.push_back(std::make_shared<PrepareBonesTransformsDataCalculateCommandPass>(programsManager, result));
    passes.push_back(std::make_shared<CalculateBonesTransformsDataPass>(programsManager, result));
    passes.push_back(std::make_shared<SimplePass>(result, clear));
    passes.push_back(std::make_shared<RenderDrawDataPass>(programsManager, result));
    passes.push_back(std::make_shared<SimplePass>(result, sort));
    passes.push_back(std::make_shared<BuildClusterPass>(programsManager, result));
    passes.push_back(std::make_shared<ClusterGlobalLightPass>(programsManager, result));
    passes.push_back(std::make_shared<PrepareClusterLocalLightsCommandPass>(programsManager, result));
    passes.push_back(std::make_shared<ClusterLocalLightPass>(programsManager, result));
    passes.push_back(std::make_shared<PrepareShadowDataCullCommnadPass>(programsManager, result));
    passes.push_back(std::make_shared<PrepareShadowMapBlurCommandsPass>(programsManager, result));
    passes.push_back(std::make_shared<CullShadowDataPass>(programsManager, result));
    passes.push_back(std::make_shared<RenderShadowDataPass>(programsManager, result));
    passes.push_back(std::make_shared<BlurShadowMapPass>(programsManager, result));
    passes.push_back(std::make_shared<RenderBackgroundPass>(programsManager, result));
    passes.push_back(std::make_shared<BlendPass>(programsManager, result));
    passes.push_back(std::make_shared<FinalPass>(programsManager, result));

    return result;
}

RenderPipeLine::RenderPipeLine(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray)
    : m_renderer(renderer)
    , m_frameBuffer(frameBuffer)
    , m_vertexArray(vertexArray)
{
    m_renderInfoBuffer = RenderInfoBuffer::element_type::create();
    m_countersBuffer = CountersBuffer::element_type::create();
    m_cameraBuffer = CameraBuffer::element_type::create();
    m_clusterNodesBuffer = ClusterNodesBuffer::element_type::create();
    m_clusterLocalLightsBuffer = ClusterLocalLightsBuffer::element_type::create();
    m_lightNodesBuffer = LightNodesBuffer::element_type::create();
    m_skeletalAnimatedDataToUpdateBuffer = SkeletalAnimatedDataToUpdateBuffer::element_type::create();
    m_shadowsToUpdateBuffer = ShadowsToUpdateBuffer::element_type::create();
    m_shadowDataBuffer = ShadowDataBuffer::element_type::create();
    m_bonesTransformsDataCalculateCommandBuffer = graphics::DispatchComputeIndirectCommandBuffer::create();
    m_opaqueDrawDataRenderCommandsBuffer = graphics::DrawArraysIndirectCommandsBuffer::create();
    m_transparentDrawDataRenderCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create();
    m_opaqueDrawDataRenderParameterBuffer = graphics::PBufferRange::element_type::create(
        m_countersBuffer->buffer(), offsetof(CountersDescription, opaqueDrawDataRenderCommandsCount),
        sizeof(CountersDescription::opaqueDrawDataRenderCommandsCount));
    m_transparentDrawDataRenderParameterBuffer = graphics::PBufferRange::element_type::create(
        m_countersBuffer->buffer(), offsetof(CountersDescription, transparentDrawDataRenderCommandsCount),
        sizeof(CountersDescription::transparentDrawDataRenderCommandsCount));
    m_clusterLocalLightsCommandBuffer = graphics::DispatchComputeIndirectCommandBuffer::create();
    m_shadowDataCullCommandBuffer = graphics::DispatchComputeIndirectCommandBuffer::create();
    m_shadowMapBlurCommandsBuffer =
        graphics::PDrawArraysIndirectCommandsBuffer::element_type::create({graphics::DrawArraysIndirectCommand()});
    m_opaqueShadowDataRenderCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create();
    m_transparentShadowDataRenderCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create();
    m_opaqueShadowDataRenderParameterBuffer = graphics::PBufferRange::element_type::create(
        m_countersBuffer->buffer(), offsetof(CountersDescription, opaqueShadowDataRenderCommandsCount),
        sizeof(CountersDescription::opaqueShadowDataRenderCommandsCount));
    m_transparentShadowDataRenderParameterBuffer = graphics::PBufferRange::element_type::create(
        m_countersBuffer->buffer(), offsetof(CountersDescription, transparentShadowDataRenderCommandsCount),
        sizeof(CountersDescription::transparentShadowDataRenderCommandsCount));
}

void RenderPipeLine::resizeShadowMomentsBluredTexture(const graphics::PConstTexture& shadowMomentsTexture)
{
    if (!shadowMomentsTexture) return;

    graphics::PConstTexture shadowMomentsBluredTexture;
    if (m_shadowMomentsBluredTextureHandle) shadowMomentsBluredTexture = m_shadowMomentsBluredTextureHandle->texture();

    const auto shadowMomentsBluredTextureMaxSize = glm::max(
        shadowMomentsBluredTexture ? shadowMomentsBluredTexture->mipmapSize(0u) : glm::uvec3(0u),
        shadowMomentsTexture->mipmapSize(0u));

    if ((!shadowMomentsBluredTexture) || (shadowMomentsBluredTexture->mipmapSize(0u) != shadowMomentsBluredTextureMaxSize))
    {
        shadowMomentsBluredTexture = m_renderer->createTexture2DArrayEmpty(
            shadowMomentsBluredTextureMaxSize.x, shadowMomentsBluredTextureMaxSize.y, shadowMomentsBluredTextureMaxSize.z,
            shadowMomentsTexture->internalFormat());
        m_shadowMomentsBluredTextureHandle = m_renderer->createTextureHandle(shadowMomentsBluredTexture);
        m_shadowMomentsBluredTextureHandle->makeResident();
    }
}

void RenderPipeLine::resizeShadowColorBluredTexture(const graphics::PConstTexture& shadowColorTexture)
{
    if (!shadowColorTexture) return;

    graphics::PConstTexture shadowColorBluredTexture;
    if (m_shadowColorBluredTextureHandle) shadowColorBluredTexture = m_shadowColorBluredTextureHandle->texture();

    const auto shadowColorBluredTextureMaxSize = glm::max(
        shadowColorBluredTexture ? shadowColorBluredTexture->mipmapSize(0u) : glm::uvec3(0u), shadowColorTexture->mipmapSize(0u));

    if ((!shadowColorBluredTexture) || (shadowColorBluredTexture->mipmapSize(0u) != shadowColorBluredTextureMaxSize))
    {
        shadowColorBluredTexture = m_renderer->createTexture2DArrayEmpty(
            shadowColorBluredTextureMaxSize.x, shadowColorBluredTextureMaxSize.y, shadowColorBluredTextureMaxSize.z,
            shadowColorTexture->internalFormat());
        m_shadowColorBluredTextureHandle = m_renderer->createTextureHandle(shadowColorBluredTexture);
        m_shadowColorBluredTextureHandle->makeResident();
    }
}

void RenderPipeLine::resizeFinalTexture(const glm::uvec2& geometryBufferSize)
{
    if (const auto finalTextureMaxSize = glm::max(m_finalTexture ? m_finalTexture->size() : glm::uvec2(0u), geometryBufferSize);
        !m_finalTexture || (m_finalTexture->size() != finalTextureMaxSize))
    {
        m_finalTexture = m_renderer->createTextureRectEmpty(
            finalTextureMaxSize.x, finalTextureMaxSize.y, graphics::PixelInternalFormat::RGBA8);
    }
}

std::vector<float> RenderPipeLine::calculateShadowBlurKernel(float sigma)
{
    static constexpr auto shadowBlurSampleEPS = 1e-2f;

    std::vector<float> shadowBlurKernel;
    for (size_t i = 0u; i < RenderInfoDescription::ShadowBlurKernelSize; ++i)
    {
        const float sample = glm::gauss(static_cast<float>(i), 0.0f, sigma);
        if (sample < shadowBlurSampleEPS) break;
        shadowBlurKernel.push_back(sample);
    }

    float samplesSum = 0.0f;
    if (!shadowBlurKernel.empty()) samplesSum += shadowBlurKernel[0u];
    for (size_t i = 1u; i < shadowBlurKernel.size(); ++i)
        samplesSum += 2.0f * shadowBlurKernel[i];

    for (auto& sample : shadowBlurKernel)
        sample /= samplesSum;

    return shadowBlurKernel;
}

} // namespace core
} // namespace simplex
