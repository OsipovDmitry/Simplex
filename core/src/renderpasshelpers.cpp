#include <core/programsmanager.h>
#include <core/graphicsrendererbase.h>
#include <core/uniform.h>

#include "geometrybuffer.h"
#include "camerarenderinfo.h"
#include "resources.h"
#include "renderpasshelpers.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

FrustumCullingPass::FrustumCullingPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<CameraRenderInfo>& cameraRenderInfo)
    : RenderPass()
    , m_cameraRenderInfo(cameraRenderInfo)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::FrustumCullingPassComputeShaderPath, {});

    m_opaqueCommandsBuffer = graphics::DrawArraysIndirectCommandsBuffer::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OpaqueCommandsBuffer) =
        graphics::BufferRange::create(m_opaqueCommandsBuffer->buffer());

    m_transparentCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::TransparentCommandsBuffer) =
        graphics::BufferRange::create(m_transparentCommandsBuffer->buffer());

    m_opaqueParameterBuffer = graphics::PBufferRange::element_type::create(
        m_cameraRenderInfo->sceneInfoBuffer()->buffer(),
        offsetof(SceneInfoDescription, opaqueCommandsCount),
        sizeof(SceneInfoDescription::opaqueCommandsCount));

    m_transparentParameterBuffer = graphics::PBufferRange::element_type::create(
        m_cameraRenderInfo->sceneInfoBuffer()->buffer(),
        offsetof(SceneInfoDescription, transparentCommandsCount),
        sizeof(SceneInfoDescription::transparentCommandsCount));

}

FrustumCullingPass::~FrustumCullingPass() = default;

void FrustumCullingPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    const auto drawDataCount = sceneData->drawDataBuffer()->size();
    const auto lightsCount = sceneData->lightsBuffer()->size();

    m_opaqueCommandsBuffer->resize(drawDataCount);
    m_transparentCommandsBuffer->resize(drawDataCount);

    m_cameraRenderInfo->setFrustum(utils::Frustum(m_viewTransform, m_clipSpace, m_cullPlanesLimits));
    m_cameraRenderInfo->resetSceneInfo(glm::uvec3(), drawDataCount, lightsCount, geometryBuffer->OITBuffer()->size());

    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(drawDataCount), 1u, 1u),
        m_program,
        { m_cameraRenderInfo, sceneData, shared_from_this()});

    utils::Range ZRange(m_cullPlanesLimits.nearValue(), m_cullPlanesLimits.nearValue() + 1.f);
    if (auto calculatedZRange = m_cameraRenderInfo->sceneInfoZRange(); !calculatedZRange.isEmpty())
    {
        static const auto s_nearCoef = 0.99f;
        static const auto s_farCoef = 1.01f;
        ZRange = calculatedZRange.translated(m_cullPlanesLimits.nearValue());
        ZRange = m_cullPlanesLimits * utils::Range(ZRange.nearValue() * s_nearCoef, ZRange.farValue() * s_farCoef);
    }

    m_cameraRenderInfo->setFrustum(utils::Frustum(m_viewTransform, m_clipSpace, ZRange));
}

void FrustumCullingPass::setFrustum(
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& cullPlanesLimits)
{
    m_viewTransform = viewTransform;
    m_clipSpace = clipSpace;
    m_cullPlanesLimits = cullPlanesLimits;
}

graphics::PDrawArraysIndirectCommandsConstBuffer FrustumCullingPass::opaqueCommandsBuffer() const
{
    return m_opaqueCommandsBuffer;
}

graphics::PConstBufferRange FrustumCullingPass::opaqueParameterBuffer() const
{
    return m_opaqueParameterBuffer;
}

graphics::PDrawArraysIndirectCommandsConstBuffer FrustumCullingPass::transparentCommandsBuffer() const
{
    return m_transparentCommandsBuffer;
}

graphics::PConstBufferRange FrustumCullingPass::transparentParameterBuffer() const
{
    return m_transparentParameterBuffer;
}

BuildClusterPass::BuildClusterPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<CameraRenderInfo>& cameraRenderInfo)
    : RenderPass()
    , m_cameraRenderInfo(cameraRenderInfo)
    , m_clusterMaxSize(0u)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildClusterPassComputeShaderPath, {});
}

BuildClusterPass::~BuildClusterPass() = default;

void BuildClusterPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    m_cameraRenderInfo->sceneInfoBuffer()->setField(offsetof(SceneInfoDescription, clusterSize), glm::uvec4(m_clusterMaxSize, 0u));
    m_cameraRenderInfo->clusterNodesBuffer()->resize(glm::compMul(m_clusterMaxSize));
    renderer->compute(
        m_clusterMaxSize,
        m_program,
        { m_cameraRenderInfo });
}

void BuildClusterPass::setCluster(const glm::uvec3& clusterMaxSize)
{
    m_clusterMaxSize = clusterMaxSize;
}

RenderDrawDataGeometryPass::RenderDrawDataGeometryPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<const CameraRenderInfo>& cameraRenderInfo,
    const graphics::PDrawArraysIndirectCommandsConstBuffer& opaqueCommandsBuffer,
    const graphics::PConstBufferRange& opaqueParameterBuffer,
    const graphics::PDrawArraysIndirectCommandsConstBuffer& transparentCommandsBuffer,
    const graphics::PConstBufferRange& transparentParameterBuffer)
    : RenderPass()
    , m_cameraRenderInfo(cameraRenderInfo)
    , m_opaqueCommandsBuffer(opaqueCommandsBuffer)
    , m_transparentCommandsBuffer(transparentCommandsBuffer)
    , m_opaqueParameterBuffer(opaqueParameterBuffer)
    , m_transparentParameterBuffer(transparentParameterBuffer)

{
    m_opaqueProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderDrawDataGeometryPassVertexShaderPath,
        resources::RenderOpaqueDrawDataGeometryPassFragmentShaderPath,
        {});

    m_transparentProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderDrawDataGeometryPassVertexShaderPath,
        resources::RenderTransparentDrawDataGeometryPassFragmentShaderPath,
        {});

    m_clearOITIndicesImageProgram = programsManager->loadOrGetComputeProgram(
        resources::ClearOITIndicesImagePassComputeShaderPath,
        {});

    m_sortOITNodesProgram = programsManager->loadOrGetComputeProgram(
        resources::SortOITNodesPassComputeShaderPath,
        {});
}

RenderDrawDataGeometryPass::~RenderDrawDataGeometryPass() = default;

void RenderDrawDataGeometryPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->colorTexture0());
    framebuffer->attach(graphics::FrameBufferAttachment::DepthStencil, geometryBuffer->depthStencilTexture());

    framebuffer->setClearDepthStencil(1.f, 0x00u);
    framebuffer->setClearColor(0u, glm::uvec4(0u));
    framebuffer->setClearMask({
        graphics::FrameBufferAttachment::Color0,
        core::graphics::FrameBufferAttachment::Depth,
        core::graphics::FrameBufferAttachment::Stencil });

    framebuffer->setFaceCulling(false);
    framebuffer->setColorMasks(true);
    framebuffer->setDepthTest(true);
    framebuffer->setDepthMask(true);
    framebuffer->setStencilTest(true);
    framebuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Always, 0x01u, 0xFFu);
    framebuffer->setStencilOperations(graphics::FaceType::FrontAndBack, { graphics::StencilOperation::Keep,
                                                                           graphics::StencilOperation::Keep,
                                                                           graphics::StencilOperation::Replace });
    framebuffer->setBlending(false);

    renderer->multiDrawArraysIndirectCount(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_opaqueProgram,
        framebuffer,
        vertexArray,
        { m_cameraRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        m_opaqueCommandsBuffer,
        m_opaqueParameterBuffer);

    renderer->compute(
        glm::uvec3(geometryBuffer->size(), 1u),
        m_clearOITIndicesImageProgram,
        { geometryBuffer });

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::DepthStencil, geometryBuffer->depthStencilTexture());

    framebuffer->setClearMask({});

    framebuffer->setFaceCulling(false);
    framebuffer->setColorMasks(false);
    framebuffer->setDepthTest(true);
    framebuffer->setDepthMask(false);
    framebuffer->setStencilTest(false);
    framebuffer->setBlending(false);

    renderer->multiDrawArraysIndirectCount(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_transparentProgram,
        framebuffer,
        vertexArray,
        { geometryBuffer, m_cameraRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        m_transparentCommandsBuffer,
        m_transparentParameterBuffer);

    renderer->compute(
        glm::uvec3(geometryBuffer->size(), 1u),
        m_sortOITNodesProgram,
        { geometryBuffer });
}

ClusterLightPass::ClusterLightPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<CameraRenderInfo>& cameraRenderInfo)
    : RenderPass()
    , m_cameraRenderInfo(cameraRenderInfo)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::ClusterLightPassComputeShaderPath, {});
}

ClusterLightPass::~ClusterLightPass() = default;

void ClusterLightPass::run(const std::shared_ptr<graphics::RendererBase>&, const std::shared_ptr<graphics::IFrameBuffer>&, const std::shared_ptr<graphics::IVertexArray>&, const std::shared_ptr<GeometryBuffer>&, const std::shared_ptr<const SceneData>&)
{
}

RenderBackgroundPass::RenderBackgroundPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<const CameraRenderInfo>& cameraRenderInfo)
    : RenderPass()
    , m_cameraRenderInfo(cameraRenderInfo)
{
    m_program = programsManager->loadOrGetRenderProgram(
        resources::RenderBackgroundPassVertexShaderPath, resources::RenderBackgroundPassFragmentShaderPath, {});
}

RenderBackgroundPass::~RenderBackgroundPass() = default;

void RenderBackgroundPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->finalTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::DepthStencil, geometryBuffer->depthStencilTexture());

    framebuffer->setClearColor(0u, glm::vec4(glm::vec3(0.f), 1.f));
    framebuffer->setClearMask({ core::graphics::FrameBufferAttachment::Color0 });

    framebuffer->setFaceCulling(false);
    framebuffer->setColorMasks(true);
    framebuffer->setDepthTest(false);
    framebuffer->setStencilTest(true);
    framebuffer->setStencilFunc(graphics::FaceType::FrontAndBack, graphics::ComparingFunc::Equal, 0x00, 0xFF);
    framebuffer->setStencilOperations(graphics::FaceType::FrontAndBack,
        { graphics::StencilOperation::Keep, graphics::StencilOperation::Keep, graphics::StencilOperation::Keep });
    framebuffer->setBlending(false);

    renderer->drawArraysIndirect(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_program,
        framebuffer,
        vertexArray,
        { m_cameraRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        sceneData->screenQuadCommandsBuffer());
}

//BuildLightsCommandsBufferPass::BuildLightsCommandsBufferPass(const std::shared_ptr<ProgramsManager>& programsManager)
//    : Pass()
//{
//    m_program = programsManager->loadOrGetComputeProgram(resources::BuildLightsCommandsBufferPassComputeShaderPath, {});
//
//    m_lightsCommandsBuffer = graphics::DrawArraysIndirectCommandsBuffer::create();
//
//    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CommandsBuffer) =
//        graphics::BufferRange::create(m_lightsCommandsBuffer->buffer());
//}
//
//BuildLightsCommandsBufferPass::~BuildLightsCommandsBufferPass() = default;
//
//void BuildLightsCommandsBufferPass::run(
//    const std::shared_ptr<graphics::RendererBase>& renderer,
//    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
//    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
//    const std::shared_ptr<CameraRenderInfo>& geometryRenderInfo,
//    const std::shared_ptr<const SceneData>& sceneData)
//{
//    const auto totalLightsCount =
//        sceneData->pointLightsBuffer()->size() +
//        sceneData->spotLightsBuffer()->size() +
//        sceneData->directionalLightsBuffer()->size() +
//        sceneData->imageBasedLightsBuffer()->size();
//
//    m_lightsCommandsBuffer->resize(totalLightsCount);
//    m_lightsCommandsBuffer->setReservedData({ 0u, static_cast<uint32_t>(totalLightsCount), {0u, 0u} });
//
//    renderer->compute(glm::uvec3(static_cast<uint32_t>(totalLightsCount), 1u, 1u), m_program, { sceneData, shared_from_this() });
//}
//
//graphics::PDrawArraysIndirectCommandsConstBuffer BuildLightsCommandsBufferPass::lightsCommandsBuffer()
//{
//    return m_lightsCommandsBuffer;
//}

//RenderDrawDataStencilPass::RenderDrawDataStencilPass(
//    const std::shared_ptr<ProgramsManager>& programsManager,
//    const graphics::PDrawArraysIndirectCommandsConstBuffer& lightsCommandsBuffer)
//    : Pass()
//    , m_lightsCommandsBuffer(lightsCommandsBuffer)
//{
//    m_program = programsManager->loadOrGetRenderProgram(
//        resources::RenderDrawDataStencilPassVertexShaderPath,
//        resources::RenderDrawDataStencilPassFragmentShaderPath,
//        {});
//}
//
//RenderDrawDataStencilPass::~RenderDrawDataStencilPass() = default;
//
//void RenderDrawDataStencilPass::run(
//    const std::shared_ptr<graphics::RendererBase>& renderer,
//    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
//    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
//    const std::shared_ptr<CameraRenderInfo>& cameraRenderInfo,
//    const std::shared_ptr<const SceneData>& sceneData)
//{
//}

BlendPass::BlendPass(const std::shared_ptr<ProgramsManager>& programsManager)
    : RenderPass()
{
    m_program = programsManager->loadOrGetRenderProgram(
        resources::BlendPassVertexShaderPath,
        resources::BlendPassFragmentShaderPath,
        {});
}

BlendPass::~BlendPass() = default;

void BlendPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->finalTexture());

    framebuffer->setClearMask({ });

    framebuffer->setFaceCulling(false);
    framebuffer->setColorMasks(true);
    framebuffer->setDepthTest(false);
    framebuffer->setStencilTest(false);
    framebuffer->setBlending(true);
    framebuffer->setBlendEquation(0u, graphics::BlendEquation::Add, graphics::BlendEquation::Add);
    framebuffer->setBlendFactor(0u,
        graphics::BlendFactor::SrcAlpha, graphics::BlendFactor::OneMinusSrcAlpha,
        graphics::BlendFactor::One, graphics::BlendFactor::SrcAlpha);

    renderer->drawArraysIndirect(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_program,
        framebuffer,
        vertexArray,
        { geometryBuffer, sceneData },
        utils::PrimitiveType::Triangles,
        sceneData->screenQuadCommandsBuffer());
}

}
}