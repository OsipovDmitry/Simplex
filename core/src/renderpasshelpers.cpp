#include "renderpasshelpers.h"

#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/programsloader.h>
#include <core/settings.h>

#include "geometrybuffer.h"
#include "resources.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

SimplePass::SimplePass(const std::shared_ptr<RenderPipeLine>& renderPipeLine, const RunMethod& runMethod)
    : RenderPass(renderPipeLine)
    , m_runMethod(runMethod)
{
}

SimplePass::~SimplePass() = default;

void SimplePass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    m_runMethod(renderer, frameBuffer, vertexArray, geometryBuffer, sceneData);
}

InitializeCameraPass::InitializeCameraPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::InitializeCameraPassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());
}

InitializeCameraPass::~InitializeCameraPass() = default;

void InitializeCameraPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    renderer->compute(glm::uvec3(1u), m_program, {shared_from_this()});
}

CullDrawDataPass::CullDrawDataPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    const auto drawDataCullingAlgorithm = settings::Settings::instance().graphics().drawDataCullingAlgorithm();
    m_program = programsManager->loadOrGetComputeProgram(
        resources::CullDrawDataPassComputeShaderPath,
        {{"DRAW_DATA_CULLING_ALGORITHM", std::to_string(castFromDrawDataCullingAlgorithm(drawDataCullingAlgorithm))}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OpaqueDrawDataRenderCommandsBuffer) =
        graphics::BufferRange::create(renderPipeLine->opaqueDrawDataRenderCommandsBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::TransparentDrawDataRenderCommandsBuffer) =
        graphics::BufferRange::create(renderPipeLine->transparentDrawDataRenderCommandsBuffer()->buffer());
}

CullDrawDataPass::~CullDrawDataPass() = default;

void CullDrawDataPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(sceneData->drawDataCount()), 1u, 1u), m_program, {sceneData, shared_from_this()});
}

CollectSkeletalAnimatedDataToUpdatePass::CollectSkeletalAnimatedDataToUpdatePass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::CollectSkeletalAnimatedDataToUpdatePassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SkeletalAnimatedDataToUpdateBuffer) =
        graphics::BufferRange::create(renderPipeLine->skeletalAnimatedDataToUpdateBuffer()->buffer());
}

CollectSkeletalAnimatedDataToUpdatePass::~CollectSkeletalAnimatedDataToUpdatePass() = default;

void CollectSkeletalAnimatedDataToUpdatePass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(sceneData->skeletalAnimatedDataCount()), 1u, 1u), m_program,
        {sceneData, shared_from_this()});
}

UpdateCameraPass::UpdateCameraPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::UpdateCameraPassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());
}

UpdateCameraPass::~UpdateCameraPass() = default;

void UpdateCameraPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    renderer->compute(glm::uvec3(1u), m_program, {shared_from_this()});
}

PrepareBonesTransformsDataCalculateCommandPass::PrepareBonesTransformsDataCalculateCommandPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    const auto calculateBonesTransformsDataComputeProgram =
        programsManager->loadOrGetComputeProgram(resources::CalculateBonesTransformsDataPassComputeShaderPath, {});

    const auto calculateBonesTransformsDataComputeProgramWorkGroupSizeX =
        calculateBonesTransformsDataComputeProgram->workGroupSize().x;

    m_program = programsManager->loadOrGetComputeProgram(
        resources::PrepareBonesTransformsDataCalculateCommandPassComputeShaderPath,
        {{"CALCULATE_BONES_TRANSFORMS_DATA_COMPUTE_PROGRAM_WORK_GROUP_SIZE_X",
          std::to_string(calculateBonesTransformsDataComputeProgramWorkGroupSizeX)}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SkeletalAnimatedDataToUpdateCommandBuffer) =
        graphics::BufferRange::create(renderPipeLine->bonesTransformsDataCalculateCommandBuffer()->buffer());
}

PrepareBonesTransformsDataCalculateCommandPass::~PrepareBonesTransformsDataCalculateCommandPass() = default;

void PrepareBonesTransformsDataCalculateCommandPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    renderer->compute(glm::uvec3(1u), m_program, {shared_from_this()});
}

CalculateBonesTransformsDataPass::CalculateBonesTransformsDataPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::CalculateBonesTransformsDataPassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SkeletalAnimatedDataToUpdateBuffer) =
        graphics::BufferRange::create(renderPipeLine->skeletalAnimatedDataToUpdateBuffer()->buffer());
}

CalculateBonesTransformsDataPass::~CalculateBonesTransformsDataPass() = default;

void CalculateBonesTransformsDataPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    renderer->computeIndirect(
        m_program, {sceneData, shared_from_this()}, renderPipeLine->bonesTransformsDataCalculateCommandBuffer());
}

RenderDrawDataPass::RenderDrawDataPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)

{
    m_opaqueProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderDrawDataPassVertexShaderPath, resources::RenderOpaqueDrawDataPassFragmentShaderPath, {});

    m_transparentProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderDrawDataPassVertexShaderPath, resources::RenderTransparentDrawDataPassFragmentShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());
}

RenderDrawDataPass::~RenderDrawDataPass() = default;

void RenderDrawDataPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    framebuffer->reset();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->colorTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());
    framebuffer->setColorMask(0u, true);
    framebuffer->setColorMask(1u, true);
    framebuffer->setDepthTest(true);
    framebuffer->setDepthMask(true);

    renderer->multiDrawArraysIndirectCount(
        glm::uvec4(0u, 0u, geometryBuffer->size()), m_opaqueProgram, framebuffer, vertexArray, {sceneData, shared_from_this()},
        utils::PrimitiveType::Triangles, renderPipeLine->opaqueDrawDataRenderCommandsBuffer(),
        renderPipeLine->opaqueDrawDataRenderParameterBuffer());

    framebuffer->reset();
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());
    framebuffer->setDepthTest(true);

    renderer->multiDrawArraysIndirectCount(
        glm::uvec4(0u, 0u, geometryBuffer->size()), m_transparentProgram, framebuffer, vertexArray,
        {geometryBuffer, sceneData, shared_from_this()}, utils::PrimitiveType::Triangles,
        renderPipeLine->transparentDrawDataRenderCommandsBuffer(), renderPipeLine->transparentDrawDataRenderParameterBuffer());
}

BuildClusterPass::BuildClusterPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildClusterPassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterNodesBuffer()->buffer());
}

BuildClusterPass::~BuildClusterPass() = default;

void BuildClusterPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(renderPipeLine->clusterNodesBuffer()->size()), 1u, 1u), m_program, {shared_from_this()});
}

ClusterGlobalLightPass::ClusterGlobalLightPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    const auto spotLightCullingAlgorithm = settings::Settings::instance().graphics().spotLightCullingAlgorithm();
    m_program = programsManager->loadOrGetComputeProgram(
        resources::ClusterGlobalLightPassComputeShaderPath,
        {{"SPOT_LIGHT_CULLING_ALGORITHM", std::to_string(castFromSpotLightCullingAlgorithm(spotLightCullingAlgorithm))}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterNodesBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterLocalLightsBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterLocalLightsBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::LightNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->lightNodesBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ShadowsToUpdateBuffer) =
        graphics::BufferRange::create(renderPipeLine->shadowsToUpdateBuffer()->buffer());
}

ClusterGlobalLightPass::~ClusterGlobalLightPass() = default;

void ClusterGlobalLightPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(sceneData->lightsCount()), 1u, 1u), m_program, {sceneData, shared_from_this()});
}

PrepareClusterLocalLightsCommandPass::PrepareClusterLocalLightsCommandPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    const auto clusterLocalLightComputeProgram =
        programsManager->loadOrGetComputeProgram(resources::ClusterLocalLightPassComputeShaderPath, {});

    const auto clusterLocalLightComputeProgramWorkGroupSize = clusterLocalLightComputeProgram->workGroupSize();

    m_program = programsManager->loadOrGetComputeProgram(
        resources::PrepareClusterLocalLightCommandPassComputeShaderPath,
        {{"CLUSTER_LOCAL_LIGHT_COMPUTE_PROGRAM_WORK_GROUP_SIZE_X",
          std::to_string(clusterLocalLightComputeProgramWorkGroupSize.x)},
         {"CLUSTER_LOCAL_LIGHT_COMPUTE_PROGRAM_WORK_GROUP_SIZE_Y",
          std::to_string(clusterLocalLightComputeProgramWorkGroupSize.y)}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterLocalLightsCommandBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterLocalLightsCommandBuffer()->buffer());
}

PrepareClusterLocalLightsCommandPass::~PrepareClusterLocalLightsCommandPass() = default;

void PrepareClusterLocalLightsCommandPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    renderer->compute(glm::uvec3(1u), m_program, {shared_from_this()});
}

ClusterLocalLightPass::ClusterLocalLightPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    const auto spotLightCullingAlgorithm = settings::Settings::instance().graphics().spotLightCullingAlgorithm();
    m_program = programsManager->loadOrGetComputeProgram(
        resources::ClusterLocalLightPassComputeShaderPath,
        {{"SPOT_LIGHT_CULLING_ALGORITHM", std::to_string(castFromSpotLightCullingAlgorithm(spotLightCullingAlgorithm))}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterNodesBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterLocalLightsBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterLocalLightsBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::LightNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->lightNodesBuffer()->buffer());
}

ClusterLocalLightPass::~ClusterLocalLightPass() = default;

void ClusterLocalLightPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    renderer->computeIndirect(m_program, {sceneData, shared_from_this()}, renderPipeLine->clusterLocalLightsCommandBuffer());
}

PrepareShadowDataCullCommnadPass::PrepareShadowDataCullCommnadPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    const auto cullShadowDataComputeProgram =
        programsManager->loadOrGetComputeProgram(resources::CullShadowDataPassComputeShaderPath, {});

    const auto cullShadowDataComputeProgramWorkGroupSize = cullShadowDataComputeProgram->workGroupSize();

    m_program = programsManager->loadOrGetComputeProgram(
        resources::PrepareShadowDataCullCommandPassComputeShaderPath,
        {{"CULL_SHADOW_DATA_COMPUTE_PROGRAM_WORK_GROUP_SIZE_X", std::to_string(cullShadowDataComputeProgramWorkGroupSize.x)},
         {"CULL_SHADOW_DATA_COMPUTE_PROGRAM_WORK_GROUP_SIZE_Y", std::to_string(cullShadowDataComputeProgramWorkGroupSize.y)}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ShadowDataCullCommandBuffer) =
        graphics::BufferRange::create(renderPipeLine->shadowDataCullCommandBuffer()->buffer());
}

PrepareShadowDataCullCommnadPass::~PrepareShadowDataCullCommnadPass() = default;

void PrepareShadowDataCullCommnadPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    renderer->compute(glm::uvec3(1u), m_program, {shared_from_this()});
}

PrepareShadowMapBlurCommandsPass::PrepareShadowMapBlurCommandsPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::PrepareShadowMapBlurCommandsComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ShadowMapBlurCommandsBuffer) =
        graphics::BufferRange::create(renderPipeLine->shadowMapBlurCommandsBuffer()->buffer());
}

PrepareShadowMapBlurCommandsPass::~PrepareShadowMapBlurCommandsPass() = default;

void PrepareShadowMapBlurCommandsPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    renderer->compute(glm::uvec3(1u), m_program, {shared_from_this()});
}

CullShadowDataPass::CullShadowDataPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    const auto shadowDataCullingAlgorithm = settings::Settings::instance().graphics().shadowDataCullingAlgorithm();
    m_program = programsManager->loadOrGetComputeProgram(
        resources::CullShadowDataPassComputeShaderPath,
        {{"SHADOW_DATA_CULLING_ALGORITHM", std::to_string(castFromShadowDataCullingAlgorithm(shadowDataCullingAlgorithm))}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ShadowsToUpdateBuffer) =
        graphics::BufferRange::create(renderPipeLine->shadowsToUpdateBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ShadowDataBuffer) =
        graphics::BufferRange::create(renderPipeLine->shadowDataBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OpaqueShadowDataRenderCommandsBuffer) =
        graphics::BufferRange::create(renderPipeLine->opaqueShadowDataRenderCommandsBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::TransparentShadowDataRenderCommandsBuffer) =
        graphics::BufferRange::create(renderPipeLine->transparentShadowDataRenderCommandsBuffer()->buffer());
}

CullShadowDataPass::~CullShadowDataPass() = default;

void CullShadowDataPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    renderer->computeIndirect(m_program, {sceneData, shared_from_this()}, renderPipeLine->shadowDataCullCommandBuffer());
}

RenderShadowDataPass::RenderShadowDataPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_opaqueProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderShadowDataPassVertexShaderPath, resources::RenderShadowDataPassGeometryShaderPath,
        resources::RenderOpaqueShadowDataPassFragmentShaderPath, {});

    m_transparentProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderShadowDataPassVertexShaderPath, resources::RenderShadowDataPassGeometryShaderPath,
        resources::RenderTransparentShadowDataPassFragmentShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ShadowDataBuffer) =
        graphics::BufferRange::create(renderPipeLine->shadowDataBuffer()->buffer());
}

RenderShadowDataPass::~RenderShadowDataPass() = default;

void RenderShadowDataPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    const auto shadowDepthTexture = sceneData->shadowDepthTexture();
    const auto shadowVarianceTexture = sceneData->shadowVarianceTexture();
    const auto shadowColorTexture = sceneData->shadowColorTexture();

    if (shadowDepthTexture && shadowVarianceTexture && shadowColorTexture)
    {
        const auto viewport = glm::uvec4(0u, 0u, glm::uvec2(sceneData->shadowAtlasSize()));

        framebuffer->reset();
        framebuffer->attach(graphics::FrameBufferAttachment::Color0, shadowVarianceTexture);
        framebuffer->attach(graphics::FrameBufferAttachment::Color1, shadowColorTexture);
        framebuffer->attach(graphics::FrameBufferAttachment::Depth, shadowDepthTexture);
        framebuffer->setClearColor(0u, glm::vec4(1.0f));
        framebuffer->setClearColor(1u, glm::vec4(1.0f));
        framebuffer->setColorMask(0u, true);
        framebuffer->setColorMask(1u, true);
        framebuffer->setDepthMask(true);

        framebuffer->clear(
            {graphics::FrameBufferAttachment::Color0, graphics::FrameBufferAttachment::Color1,
             core::graphics::FrameBufferAttachment::Depth});

        framebuffer->reset();
        framebuffer->attach(graphics::FrameBufferAttachment::Color0, shadowVarianceTexture);
        framebuffer->attach(graphics::FrameBufferAttachment::Depth, shadowDepthTexture);
        framebuffer->setColorMask(0u, true);
        framebuffer->setDepthTest(true);
        framebuffer->setDepthMask(true);

        for (uint32_t i = 0; i < 6u; ++i)
            framebuffer->setClipDistance(i, true);

        renderer->multiDrawArraysIndirectCount(
            viewport, m_opaqueProgram, framebuffer, vertexArray, {sceneData, shared_from_this()}, utils::PrimitiveType::Triangles,
            renderPipeLine->opaqueShadowDataRenderCommandsBuffer(), renderPipeLine->opaqueShadowDataRenderParameterBuffer());

        framebuffer->reset();
        framebuffer->attach(graphics::FrameBufferAttachment::Color0, shadowColorTexture);
        framebuffer->attach(graphics::FrameBufferAttachment::Depth, shadowDepthTexture);
        framebuffer->setColorMask(0u, true);
        framebuffer->setDepthTest(true);
        framebuffer->setBlending(true);
        framebuffer->setBlendEquation(0u, graphics::BlendEquation::Add, graphics::BlendEquation::Add);
        framebuffer->setBlendFactor(
            0u, graphics::BlendFactor::Zero, graphics::BlendFactor::SrcColor, graphics::BlendFactor::Zero,
            graphics::BlendFactor::One);

        for (uint32_t i = 0; i < 6u; ++i)
            framebuffer->setClipDistance(i, true);

        renderer->multiDrawArraysIndirectCount(
            viewport, m_transparentProgram, framebuffer, vertexArray, {sceneData, shared_from_this()},
            utils::PrimitiveType::Triangles, renderPipeLine->transparentShadowDataRenderCommandsBuffer(),
            renderPipeLine->transparentShadowDataRenderParameterBuffer());
    }
}

BlurShadowMapPass::BlurShadowMapPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_horizontalProgram = programsManager->loadOrGetRenderProgram(
        resources::BlurShadowMapPassVertexShaderPath, resources::BlurShadowMapPassGeometryShaderPath,
        resources::BlurShadowMapPassFragmentShaderPath, {{"HORIZONTAL_PASS", ""}});

    m_verticalProgram = programsManager->loadOrGetRenderProgram(
        resources::BlurShadowMapPassVertexShaderPath, resources::BlurShadowMapPassGeometryShaderPath,
        resources::BlurShadowMapPassFragmentShaderPath, {{"VERTICAL_PASS", ""}});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());
}

BlurShadowMapPass::~BlurShadowMapPass() = default;

void BlurShadowMapPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    const auto shadowVarianceTexture = sceneData->shadowVarianceTexture();
    const auto shadowColorTexture = sceneData->shadowColorTexture();

    if (shadowVarianceTexture && shadowColorTexture)
    {
        const auto& shadowVarianceBluredTextureHandle = renderPipeLine->shadowVarianceBluredTextureHandle();
        const auto& shadowColorBluredTextureHandle = renderPipeLine->shadowColorBluredTextureHandle();

        const auto viewport = glm::uvec4(0u, 0u, glm::uvec2(sceneData->shadowAtlasSize()));

        framebuffer->reset();
        framebuffer->attach(graphics::FrameBufferAttachment::Color0, shadowVarianceBluredTextureHandle->texture());
        framebuffer->attach(graphics::FrameBufferAttachment::Color1, shadowColorBluredTextureHandle->texture());
        framebuffer->setColorMask(0u, true);
        framebuffer->setColorMask(1u, true);
        renderer->drawArraysIndirect(
            viewport, m_horizontalProgram, framebuffer, vertexArray, {sceneData, shared_from_this()},
            utils::PrimitiveType::TriangleStrip, renderPipeLine->shadowMapBlurCommandsBuffer());

        framebuffer->reset();
        framebuffer->attach(graphics::FrameBufferAttachment::Color0, shadowVarianceTexture);
        framebuffer->attach(graphics::FrameBufferAttachment::Color1, shadowColorTexture);
        framebuffer->setColorMask(0u, true);
        framebuffer->setColorMask(1u, true);
        renderer->drawArraysIndirect(
            viewport, m_verticalProgram, framebuffer, vertexArray, {sceneData, shared_from_this()},
            utils::PrimitiveType::TriangleStrip, renderPipeLine->shadowMapBlurCommandsBuffer());
    }
}

RenderBackgroundPass::RenderBackgroundPass(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetRenderProgram(
        resources::RenderBackgroundPassVertexShaderPath, resources::RenderBackgroundPassFragmentShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());
}

RenderBackgroundPass::~RenderBackgroundPass() = default;

void RenderBackgroundPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    framebuffer->reset();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->finalTexture());
    framebuffer->setColorMask(0u, true);

    renderer->drawArrays(
        glm::uvec4(0u, 0u, geometryBuffer->size()), m_program, framebuffer, vertexArray, {sceneData, shared_from_this()},
        utils::PrimitiveType::TriangleStrip, 0u, 4u);
}

BlendPass::BlendPass(const std::shared_ptr<ProgramsLoader>& programsManager, const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program =
        programsManager->loadOrGetRenderProgram(resources::BlendPassVertexShaderPath, resources::BlendPassFragmentShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::RenderInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->renderInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CountersBuffer) =
        graphics::BufferRange::create(renderPipeLine->countersBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterNodesBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::LightNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->lightNodesBuffer()->buffer());
}

BlendPass::~BlendPass() = default;

void BlendPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    framebuffer->reset();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->finalTexture());
    framebuffer->setColorMask(0u, true);
    framebuffer->setBlending(true);
    framebuffer->setBlendEquation(0u, graphics::BlendEquation::Add, graphics::BlendEquation::Add);
    framebuffer->setBlendFactor(
        0u, graphics::BlendFactor::SrcAlpha, graphics::BlendFactor::OneMinusSrcAlpha, graphics::BlendFactor::One,
        graphics::BlendFactor::SrcAlpha);

    renderer->drawArrays(
        glm::uvec4(0u, 0u, geometryBuffer->size()), m_program, framebuffer, vertexArray,
        {geometryBuffer, sceneData, shared_from_this()}, utils::PrimitiveType::TriangleStrip, 0u, 4u);
}

FinalPass::FinalPass(const std::shared_ptr<ProgramsLoader>& programsManager, const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program =
        programsManager->loadOrGetRenderProgram(resources::FinalPassVertexShaderPath, resources::FinalPassFragmentShaderPath, {});
}

FinalPass::~FinalPass() = default;

void FinalPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{

    auto renderPipeLine = m_renderPipeLine.lock();
    if (!renderPipeLine)
    {
        LOG_CRITICAL << "RenderPipeLine can't be nullptr";
        return;
    }

    framebuffer->reset();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, renderPipeLine->finalTexture());
    framebuffer->setColorMask(0u, true);

    renderer->drawArrays(
        glm::uvec4(0u, 0u, geometryBuffer->size()), m_program, framebuffer, vertexArray, {geometryBuffer, shared_from_this()},
        utils::PrimitiveType::TriangleStrip, 0u, 4u);
}

} // namespace core
} // namespace simplex