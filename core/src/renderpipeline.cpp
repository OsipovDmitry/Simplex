#include "renderpipeline.h"

#include <utils/glm/gtx/component_wise.hpp>
#include <utils/glm/gtx/functions.hpp>
#include <utils/glm/gtx/texture.hpp>
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
    m_opaqueDrawDataRenderCommandsBuffer = graphics::PDrawElementsIndirectCommandBuffer::element_type::create();
    m_transparentDrawDataRenderCommandsBuffer = graphics::PDrawElementsIndirectCommandBuffer::element_type::create();
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
    m_HDRBuffer = HDRBuffer::element_type::create();
    m_bloomBuffer = BloomBuffer::element_type::create();
    m_toneMappingBuffer = ToneMappingBuffer::element_type::create();
    m_opaqueShadowDataRenderCommandsBuffer = graphics::PDrawElementsIndirectCommandBuffer::element_type::create();
    m_transparentShadowDataRenderCommandsBuffer = graphics::PDrawElementsIndirectCommandBuffer::element_type::create();
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
    if (isShadowBlurPassNeeded()) m_passes.push_back(std::make_shared<BlurShadowMapPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<RenderBackgroundPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<BlendPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<ToneMappingPass>(programsLoader, sharedThis));
    if (m_isBloomEnabled) m_passes.push_back(std::make_shared<BloomPass>(programsLoader, sharedThis));
    m_passes.push_back(std::make_shared<FinalPass>(programsLoader, sharedThis));

    m_isInitialized = true;
}

void RenderPipeLine::run(
    const std::shared_ptr<graphics::RendererBase>& graphicsRenderer,
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData,
    const glm::uvec2& viewportSize,
    uint64_t time,
    uint32_t dt,
    float dielectricSpecular,
    const utils::OrientedBoundingBox& globalBoundingBox,
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& cullPlaneLimits,
    const glm::uvec3& clusterSize)
{
    m_viewportSize = viewportSize;

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
        m_viewportSize, static_cast<uint32_t>(time), dt, dielectricSpecular, globalBoundingBox,
        static_cast<uint32_t>(drawDataCount), static_cast<uint32_t>(skeletalAnimatedDataCount),
        static_cast<uint32_t>(shadowsCount), static_cast<uint32_t>(lightsCount), clusterSize, viewTransform, clipSpace,
        cullPlaneLimits));

    resizeShadowTextures(graphicsRenderer, sceneData->shadowMapsLayersCount());
    updateShadowMapsBuffer();

    resizeHDRTexture(graphicsRenderer);
    updateHDRBuffer();

    resizeBloomTexture(graphicsRenderer);
    updateBloomBuffer();

    updateToneMappingBuffer();

    resizeFinalTexture(graphicsRenderer);

    vertexArray->attachIndexBuffer(sceneData->elementDataBuffer()->buffer());

    for (auto& pass : m_passes)
        pass->run(graphicsRenderer, frameBuffer, vertexArray, geometryBuffer, sceneData);
}

const glm::uvec2& RenderPipeLine::viewportSize() const
{
    return m_viewportSize;
}

uint32_t RenderPipeLine::shadowAtlasSize() const
{
    return m_shadowAtlasSize;
}

ShadowFilter RenderPipeLine::shadowFilter() const
{
    return m_shadowFilter;
}

void RenderPipeLine::setShadowFilter(ShadowFilter value)
{
    if (m_shadowFilter != value)
    {
        m_shadowFilter = value;
        deinitialize(); // need to recreate shaders 'cause "shadow filter" is a define parameter (not a shadow maps buffer member)
    }
}

void RenderPipeLine::setShadowBlurSigma(float value)
{
    if (m_shadowBlurSigma != value)
    {
        m_shadowBlurSigma = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setShadowLightBleedingAmount(float value)
{
    if (m_shadowLightBleedingAmount != value)
    {
        m_shadowLightBleedingAmount = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setShadowPositiveExponent(float value)
{
    if (m_shadowPositiveExponent != value)
    {
        m_shadowPositiveExponent = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setShadowNegativeExponent(float value)
{
    if (m_shadowNegativeExponent != value)
    {
        m_shadowNegativeExponent = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setShadowMomentsBias(float value)
{
    if (m_shadowMomentsBias != value)
    {
        m_shadowMomentsBias = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setShadowDepthBiasFactor(float value)
{
    if (m_shadowDepthBiasFactor != value)
    {
        m_shadowDepthBiasFactor = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setShadowCascadesBlendDistanceFactor(float value)
{
    if (m_shadowCascadesBlendDistanceFactor != value)
    {
        m_shadowCascadesBlendDistanceFactor = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setShadowCascadesDistancePower(float value)
{
    if (m_shadowCascadesDistancePower != value)
    {
        m_shadowCascadesDistancePower = value;
        dirtyShadowMapsBuffer();
    }
}

void RenderPipeLine::setBloomEnabled(bool value)
{
    if (m_isBloomEnabled != value)
    {
        m_isBloomEnabled = value;
        deinitialize(); // need to recreate passes 'cause bloom pass will be created if only m_isBloomEnabled flag is set
    }
}

void RenderPipeLine::setBloomContribution(float value)
{
    if (m_bloomContribution != value)
    {
        m_bloomContribution = value;
        dirtyBloomBuffer();
    }
}

void RenderPipeLine::setBloomPassesCount(uint32_t value)
{
    if (m_bloomPassesCount != value)
    {
        m_bloomPassesCount = value;
        // no need any additional actions 'cause bloom texture will be recreated next frame
    }
}

void RenderPipeLine::setBloomUpSamplePassBlurRadius(float value)
{
    if (m_bloomUpSamplePassBlurRadius != value)
    {
        m_bloomUpSamplePassBlurRadius = value;
        dirtyBloomBuffer();
    }
}

void RenderPipeLine::setToneMappingLuminanceRange(const utils::Range& value)
{
    if (m_toneMappingLuminanceRange != value)
    {
        m_toneMappingLuminanceRange = value;
        dirtyToneMappingBuffer();
    }
}

void RenderPipeLine::setToneMappingLuminanceClampRange(const utils::Range& value)
{
    if (m_toneMappingLuminanceClampRange != value)
    {
        m_toneMappingLuminanceClampRange = value;
        dirtyToneMappingBuffer();
    }
}

void RenderPipeLine::setToneMappingPixelsFractionToTrim(const std::pair<float, float>& value)
{
    if (m_toneMappingPixelsFractionToTrim != value)
    {
        m_toneMappingPixelsFractionToTrim = value;
        dirtyToneMappingBuffer();
    }
}

void RenderPipeLine::setToneMappingTauLight(float value)
{
    if (m_toneMappingTauLight != value)
    {
        m_toneMappingTauLight = value;
        dirtyToneMappingBuffer();
    }
}

void RenderPipeLine::setToneMappingTauDark(float value)
{
    if (m_toneMappingTauDark != value)
    {
        m_toneMappingTauDark = value;
        dirtyToneMappingBuffer();
    }
}

void RenderPipeLine::setToneMappingBaseLumiance(float value)
{
    if (m_toneMappingBaseLuminance != value)
    {
        m_toneMappingBaseLuminance = value;
        dirtyToneMappingBuffer();
    }
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

HDRBuffer& RenderPipeLine::hdrBuffer()
{
    return m_HDRBuffer;
}

BloomBuffer& RenderPipeLine::bloomBuffer()
{
    return m_bloomBuffer;
}

ToneMappingBuffer& RenderPipeLine::toneMappingBuffer()
{
    return m_toneMappingBuffer;
}

graphics::PDispatchComputeIndirectCommandBuffer& RenderPipeLine::bonesTransformsDataCalculateCommandBuffer()
{
    return m_bonesTransformsDataCalculateCommandBuffer;
}

graphics::PDrawElementsIndirectCommandBuffer& RenderPipeLine::opaqueDrawDataRenderCommandsBuffer()
{
    return m_opaqueDrawDataRenderCommandsBuffer;
}

graphics::PDrawElementsIndirectCommandBuffer& RenderPipeLine::transparentDrawDataRenderCommandsBuffer()
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

graphics::PDrawElementsIndirectCommandBuffer& RenderPipeLine::opaqueShadowDataRenderCommandsBuffer()
{
    return m_opaqueShadowDataRenderCommandsBuffer;
}

graphics::PDrawElementsIndirectCommandBuffer& RenderPipeLine::transparentShadowDataRenderCommandsBuffer()
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

graphics::PConstTexture RenderPipeLine::HDRTexture() const
{
    return m_HDRTextureHandle ? m_HDRTextureHandle->texture() : nullptr;
}

graphics::PConstTexture RenderPipeLine::bloomTexture() const
{
    return m_bloomTextureHandle ? m_bloomTextureHandle->texture() : nullptr;
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

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::shadowMapBlurCommandsBuffer()
{
    return m_shadowMapBlurCommandsBuffer;
}

void RenderPipeLine::deinitialize()
{
    m_isInitialized = false;
    dirtyShadowMapsBuffer();
    dirtyBloomBuffer();
    dirtyToneMappingBuffer();
}

void RenderPipeLine::dirtyShadowMapsBuffer()
{
    m_isShadowMapsBufferDirty = true;
}

void RenderPipeLine::dirtyHDRBuffer()
{
    m_isHDRBufferDirty = true;
}

void RenderPipeLine::dirtyBloomBuffer()
{
    m_isBloomBufferDirty = true;
}

void RenderPipeLine::dirtyToneMappingBuffer()
{
    m_isToneMappingBufferDirty = true;
}

bool RenderPipeLine::isShadowBlurPassNeeded() const
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

    dirtyShadowMapsBuffer();
}

void RenderPipeLine::updateShadowMapsBuffer()
{
    if (!m_isShadowMapsBufferDirty) return;

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
        m_shadowDepthBiasFactor, m_shadowCascadesBlendDistanceFactor, m_shadowCascadesDistancePower,
        calculateShadowBlurKernel()));

    m_isShadowMapsBufferDirty = false;
}

void RenderPipeLine::resizeHDRTexture(const std::shared_ptr<graphics::RendererBase>& renderer)
{
    const auto newSize = glm::max(m_viewportSize, glm::uvec2(1u));

    const graphics::PConstTexture HDRTexture = m_HDRTextureHandle ? m_HDRTextureHandle->texture() : nullptr;
    const auto oldSize = HDRTexture ? HDRTexture->size() : glm::uvec2(0u);

    if (newSize == oldSize) return;

    auto texture = renderer->createTextureRectEmpty(newSize.x, newSize.y, graphics::PixelInternalFormat::RGBA16F);
    texture->setFilterMode(graphics::TextureFilterMode::Linear);
    texture->setWrapMode(graphics::TextureWrapMode::ClampToEdge);
    m_HDRTextureHandle = renderer->createTextureHandle(texture);
    m_HDRTextureHandle->makeResident();

    dirtyHDRBuffer();
}

void RenderPipeLine::updateHDRBuffer()
{
    if (!m_isHDRBufferDirty) return;

    m_HDRBuffer->set(HDRDescription::make(
        m_HDRTextureHandle ? m_HDRTextureHandle->handle() : utils::IDsGeneratorT<graphics::TextureHandle>::last()));

    m_isHDRBufferDirty = false;
}

void RenderPipeLine::resizeBloomTexture(const std::shared_ptr<graphics::RendererBase>& renderer)
{
    const auto newSize = glm::max(m_viewportSize / 2u, glm::uvec2(1u));
    const auto newLevelsCount = glm::min(glm::levels(newSize), m_bloomPassesCount);

    const graphics::PConstTexture bloomTexture = m_bloomTextureHandle ? m_bloomTextureHandle->texture() : nullptr;
    const auto oldSize = bloomTexture ? bloomTexture->size() : glm::uvec2(0u);
    const auto oldLevelCount = bloomTexture ? bloomTexture->numMipmapLevels() : 0u;

    if ((newSize == oldSize) && (newLevelsCount == oldLevelCount)) return;

    auto texture =
        renderer->createTexture2DEmpty(newSize.x, newSize.y, graphics::PixelInternalFormat::R11F_G11F_B10F, newLevelsCount);
    texture->setFilterMode(graphics::TextureFilterMode::Bilinear);
    texture->setWrapMode(graphics::TextureWrapMode::ClampToEdge);
    m_bloomTextureHandle = renderer->createTextureHandle(texture);
    m_bloomTextureHandle->makeResident();

    dirtyBloomBuffer();
}

void RenderPipeLine::updateBloomBuffer()
{
    if (!m_isBloomBufferDirty) return;

    m_bloomBuffer->set(BloomDescription::make(
        m_bloomTextureHandle ? m_bloomTextureHandle->handle() : utils::IDsGeneratorT<graphics::TextureHandle>::last(),
        m_bloomContribution, m_bloomUpSamplePassBlurRadius));

    m_isBloomBufferDirty = false;
}

void RenderPipeLine::updateToneMappingBuffer()
{
    if (!m_isToneMappingBufferDirty) return;

    m_toneMappingBuffer->set(ToneMappingDescription::make(
        m_toneMappingLuminanceRange, m_toneMappingLuminanceClampRange, m_toneMappingPixelsFractionToTrim,
        m_toneMappingBaseLuminance, m_toneMappingTauLight, m_toneMappingTauDark));

    m_isToneMappingBufferDirty = false;
}

void RenderPipeLine::resizeFinalTexture(const std::shared_ptr<graphics::RendererBase>& renderer)
{
    const auto newSize = glm::max(m_viewportSize, glm::uvec2(1u));

    const graphics::PConstTexture HDRTexture = m_HDRTextureHandle ? m_HDRTextureHandle->texture() : nullptr;
    const auto oldSize = m_finalTexture ? m_finalTexture->size() : glm::uvec2(0u);

    if (newSize == oldSize) return;

    m_finalTexture = renderer->createTextureRectEmpty(newSize.x, newSize.y, graphics::PixelInternalFormat::RGBA8);
}

} // namespace core
} // namespace simplex
