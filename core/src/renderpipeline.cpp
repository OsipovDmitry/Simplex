#include "renderpipeline.h"

#include <utils/glm/gtx/component_wise.hpp>
#include <utils/glm/gtx/functions.hpp>
#include <utils/logger.h>

#include <core/graphicsrendererbase.h>
#include <core/programsloader.h>

#include "geometrybuffer.h"
#include "renderpasshelpers.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

RenderPipeLine::RenderPipeLine(uint32_t shadowAtlasSize)
    : m_shadowAtlasSize(shadowAtlasSize)
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
    m_shadowMapsBuffer = ShadowMapsBuffer::element_type::create(ShadowMapsDescription::makeEmpty());
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

RenderPipeLine::~RenderPipeLine() = default;

void RenderPipeLine::initialize(const std::shared_ptr<ProgramsLoader>& programsLoader)
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

    if (m_isInitialized) return;

    auto sharedThis = shared_from_this();

    m_passes.clear();
    m_passes.push_back(std::make_shared<InitializeCameraPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<CullDrawDataPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<CollectSkeletalAnimatedDataToUpdatePass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<UpdateCameraPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<PrepareBonesTransformsDataCalculateCommandPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<CalculateBonesTransformsDataPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<SimplePass>(sharedThis, clear));
    m_passes.push_back(std::make_shared<RenderDrawDataPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<SimplePass>(sharedThis, sort));
    m_passes.push_back(std::make_shared<BuildClusterPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<ClusterGlobalLightPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<PrepareClusterLocalLightsCommandPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<ClusterLocalLightPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<PrepareShadowDataCullCommnadPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<PrepareShadowMapBlurCommandsPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<CullShadowDataPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<RenderShadowDataPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<BlurShadowMapPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<RenderBackgroundPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<BlendPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<FinalPass>(programsLoader, sharedThis));

    m_isInitialized = true;
}

void RenderPipeLine::deinitialize()
{
    m_isInitialized = false;
}

void RenderPipeLine::run(
    const std::shared_ptr<graphics::RendererBase>& graphicsRenderer,
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
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

    m_renderInfoBuffer->set(RenderInfoDescription::make(
        static_cast<uint32_t>(time), dielectricSpecular, globalBoundingBox, static_cast<uint32_t>(drawDataCount),
        static_cast<uint32_t>(skeletalAnimatedDataCount), static_cast<uint32_t>(shadowsCount), static_cast<uint32_t>(lightsCount),
        clusterSize, viewTransform, clipSpace, cullPlaneLimits));

    resizeShadowTextures(graphicsRenderer, sceneData->shadowMapsLayersCount());
    resizeFinalTexture(graphicsRenderer, geometryBuffer->size());

    for (auto& pass : m_passes)
        pass->run(graphicsRenderer, frameBuffer, vertexArray, geometryBuffer, sceneData);
}

uint32_t RenderPipeLine::shadowAtlasSize() const
{
    return m_shadowAtlasSize;
}

ShadowFilter& RenderPipeLine::shadowFilter()
{
    return m_shadowFilter;
}

float& RenderPipeLine::shadowBlurSigma()
{
    return m_shadowBlurSigma;
}

float& RenderPipeLine::shadowLightBleedingAmount()
{
    return m_shadowLightBleedingAmount;
}

float& RenderPipeLine::shadowPositiveExponent()
{
    return m_shadowPositiveExponent;
}

float& RenderPipeLine::shadowNegativeExponent()
{
    return m_shadowNegativeExponent;
}

float& RenderPipeLine::shadowMomentsBias()
{
    return m_shadowMomentsBias;
}

float& RenderPipeLine::shadowDepthBiasFactor()
{
    return m_shadowDepthBiasFactor;
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

ShadowMapsBuffer& RenderPipeLine::shadowMapsBuffer()
{
    return m_shadowMapsBuffer;
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

graphics::PConstTexture RenderPipeLine::shadowDepthTexture() const
{
    return m_shadowDepthTextureHandle ? m_shadowDepthTextureHandle->texture() : nullptr;
}

graphics::PConstTexture RenderPipeLine::shadowMomentsTexture() const
{
    return m_shadowMomentsTextureHandle ? m_shadowMomentsTextureHandle->texture() : nullptr;
}

graphics::PConstTexture RenderPipeLine::shadowColorTexture() const
{
    return m_shadowColorTextureHandle ? m_shadowColorTextureHandle->texture() : nullptr;
}

graphics::PConstTexture RenderPipeLine::shadowMomentsBluredTexture() const
{
    return m_shadowMomentsBluredTextureHandle ? m_shadowMomentsBluredTextureHandle->texture() : nullptr;
}

graphics::PConstTexture RenderPipeLine::shadowColorBluredTexture() const
{
    return m_shadowColorBluredTextureHandle ? m_shadowColorBluredTextureHandle->texture() : nullptr;
}

graphics::PConstTexture RenderPipeLine::finalTexture() const
{
    return m_finalTexture;
}

glm::vec4 RenderPipeLine::shadowMomentsTextureClearColor() const
{
    glm::vec4 result(0.f);

    switch (m_shadowFilter)
    {
        case ShadowFilter::Discrete:
        {
            result = glm::vec4(1.f, 0.f, 0.f, 0.f);
            break;
        }
        case ShadowFilter::VSM:
        {
            result = glm::vec4(1.f, 1.f, 0.f, 0.f);
            break;
        }
        case ShadowFilter::EVSM:
        {
            const auto expPos = glm::exp(m_shadowPositiveExponent);
            const auto expNeg = -glm::exp(m_shadowNegativeExponent);
            result = glm::vec4(expPos, expPos * expPos, expNeg, expNeg * expNeg);
            break;
        }
        case ShadowFilter::HamburgerMSM:
        case ShadowFilter::HausdorffMSM:
        {
            result = glm::vec4(1.f, 0.f, 0.f, 0.f);
            break;
        }
        default:
            break;
    }

    return result;
}

bool RenderPipeLine::shadowIsBlurPassNeeded() const
{
    static std::unordered_map<ShadowFilter, bool> s_table{
        {ShadowFilter::Discrete, false},
        {ShadowFilter::VSM, true},
        {ShadowFilter::EVSM, true},
        {ShadowFilter::HamburgerMSM, true},
        {ShadowFilter::HausdorffMSM, true}};

    auto it = s_table.find(m_shadowFilter);
    return (it != s_table.end()) ? it->second : false;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::shadowMapBlurCommandsBuffer()
{
    return m_shadowMapBlurCommandsBuffer;
}

graphics::PixelInternalFormat RenderPipeLine::shadowMomentsTextureInternalFormat() const
{
    static std::unordered_map<ShadowFilter, graphics::PixelInternalFormat> s_table{
        {ShadowFilter::Discrete, graphics::PixelInternalFormat::R32F},
        {ShadowFilter::VSM, graphics::PixelInternalFormat::RG32F},
        {ShadowFilter::EVSM, graphics::PixelInternalFormat::RGBA32F},
        {ShadowFilter::HamburgerMSM, graphics::PixelInternalFormat::RGBA32F},
        {ShadowFilter::HausdorffMSM, graphics::PixelInternalFormat::RGBA32F}};

    auto it = s_table.find(m_shadowFilter);
    return (it != s_table.end()) ? it->second : graphics::PixelInternalFormat::Count;
}

std::vector<float> RenderPipeLine::calculateShadowBlurKernel() const
{
    static constexpr auto SampleEPS = 1e-2f;

    std::vector<float> result;
    for (size_t i = 0u; i < ShadowMapsDescription::BlurKernelSize; ++i)
    {
        const float sample = glm::gauss(static_cast<float>(i), 0.0f, m_shadowBlurSigma);
        if (sample < SampleEPS) break;
        result.push_back(sample);
    }

    auto samplesSum = 0.0f;
    if (!result.empty()) samplesSum += result[0u];
    for (size_t i = 1u; i < result.size(); ++i)
        samplesSum += 2.0f * result[i];

    for (auto& sample : result)
        sample /= samplesSum;

    return result;
}

void RenderPipeLine::resizeShadowTextures(const std::shared_ptr<graphics::RendererBase>& renderer, uint32_t shadowMapsLayersCount)
{
    const graphics::PConstTexture shadowMomentsTexture =
        m_shadowMomentsTextureHandle ? m_shadowMomentsTextureHandle->texture() : nullptr;

    const auto oldLayersCount = shadowMomentsTexture ? shadowMomentsTexture->mipmapSize()[2u] : 0u;
    const auto oldMomentsTextureInternalFormat =
        shadowMomentsTexture ? shadowMomentsTexture->internalFormat() : graphics::PixelInternalFormat::Count;

    auto momentsTextureInternalFormat = shadowMomentsTextureInternalFormat();
    if (momentsTextureInternalFormat == graphics::PixelInternalFormat::Count)
    {
        LOG_CRITICAL << "Undefined shadow moments texture internal format";
        return;
    }

    if ((oldLayersCount == shadowMapsLayersCount) && (oldMomentsTextureInternalFormat == momentsTextureInternalFormat)) return;

    if (!shadowMapsLayersCount)
    {
        m_shadowDepthTextureHandle.reset();
        m_shadowMomentsTextureHandle.reset();
        m_shadowColorTextureHandle.reset();
        m_shadowMomentsBluredTextureHandle.reset();
        m_shadowColorBluredTextureHandle.reset();
    }
    else
    {
        auto depthTexture = renderer->createTexture2DArrayEmpty(
            m_shadowAtlasSize, m_shadowAtlasSize, shadowMapsLayersCount, graphics::PixelInternalFormat::Depth32F);
        m_shadowDepthTextureHandle = renderer->createTextureHandle(depthTexture);
        m_shadowDepthTextureHandle->makeResident();

        auto momentsTexture = renderer->createTexture2DArrayEmpty(
            m_shadowAtlasSize, m_shadowAtlasSize, shadowMapsLayersCount, momentsTextureInternalFormat);
        m_shadowMomentsTextureHandle = renderer->createTextureHandle(momentsTexture);
        m_shadowMomentsTextureHandle->makeResident();

        auto momentsBluredTexture = renderer->createTexture2DArrayEmpty(
            m_shadowAtlasSize, m_shadowAtlasSize, shadowMapsLayersCount, momentsTextureInternalFormat);
        m_shadowMomentsBluredTextureHandle = renderer->createTextureHandle(momentsBluredTexture);
        m_shadowMomentsBluredTextureHandle->makeResident();

        auto colorTexture = renderer->createTexture2DArrayEmpty(
            m_shadowAtlasSize, m_shadowAtlasSize, shadowMapsLayersCount, graphics::PixelInternalFormat::R11F_G11F_B10F);
        m_shadowColorTextureHandle = renderer->createTextureHandle(colorTexture);
        m_shadowColorTextureHandle->makeResident();

        auto colorBluredTexture = renderer->createTexture2DArrayEmpty(
            m_shadowAtlasSize, m_shadowAtlasSize, shadowMapsLayersCount, graphics::PixelInternalFormat::R11F_G11F_B10F);
        m_shadowColorBluredTextureHandle = renderer->createTextureHandle(colorBluredTexture);
        m_shadowColorBluredTextureHandle->makeResident();
    }

    updateShadowMapsBuffer();
}

void RenderPipeLine::updateShadowMapsBuffer()
{
    m_shadowMapsBuffer->set(ShadowMapsDescription::make(
        m_shadowDepthTextureHandle ? m_shadowDepthTextureHandle->handle() : utils::IDsGeneratorT<graphics::TextureHandle>::last(),
        m_shadowMomentsTextureHandle ? m_shadowMomentsTextureHandle->handle()
                                     : utils::IDsGeneratorT<graphics::TextureHandle>::last(),
        m_shadowColorTextureHandle ? m_shadowColorTextureHandle->handle() : utils::IDsGeneratorT<graphics::TextureHandle>::last(),
        m_shadowMomentsBluredTextureHandle ? m_shadowMomentsBluredTextureHandle->handle()
                                           : utils::IDsGeneratorT<graphics::TextureHandle>::last(),
        m_shadowColorBluredTextureHandle ? m_shadowColorBluredTextureHandle->handle()
                                         : utils::IDsGeneratorT<graphics::TextureHandle>::last(),
        m_shadowAtlasSize, m_shadowLightBleedingAmount, m_shadowPositiveExponent, m_shadowNegativeExponent, m_shadowMomentsBias,
        m_shadowDepthBiasFactor, calculateShadowBlurKernel()));
}

void RenderPipeLine::resizeFinalTexture(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const glm::uvec2& geometryBufferSize)
{
    const auto newSize = glm::max(geometryBufferSize, glm::uvec2(1u));

    if (const auto size = m_finalTexture ? m_finalTexture->size() : glm::uvec2(0u); !m_finalTexture || (size != newSize))
    {
        m_finalTexture = renderer->createTextureRectEmpty(newSize.x, newSize.y, graphics::PixelInternalFormat::RGBA8);
    }
}

} // namespace core
} // namespace simplex
