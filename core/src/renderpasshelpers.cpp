#include <core/programsmanager.h>
#include <core/graphicsrendererbase.h>
#include <core/uniform.h>

#include "geometrybuffer.h"
#include "resources.h"
#include "renderpasshelpers.h"
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

FrustumCullingPass::FrustumCullingPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::FrustumCullingPassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SceneInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->sceneInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OpaqueCommandsBuffer) =
        graphics::BufferRange::create(renderPipeLine->opaqueCommandsBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::TransparentCommandsBuffer) =
        graphics::BufferRange::create(renderPipeLine->transparentCommandsBuffer()->buffer());
}

FrustumCullingPass::~FrustumCullingPass() = default;

void FrustumCullingPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(sceneData->drawDataBuffer()->size()), 1u, 1u),
        m_program,
        { sceneData, shared_from_this()});
}

UpdateCameraInfoPass::UpdateCameraInfoPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::UpdateCameraInfoPassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());
}

UpdateCameraInfoPass::~UpdateCameraInfoPass() = default;

void UpdateCameraInfoPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>&)
{
    renderer->compute(
        glm::uvec3(1u),
        m_program,
        { shared_from_this() });
}

RenderDrawDataGeometryPass::RenderDrawDataGeometryPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)

{
    m_opaqueProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderDrawDataGeometryPassVertexShaderPath,
        resources::RenderOpaqueDrawDataGeometryPassFragmentShaderPath,
        {});

    m_transparentProgram = programsManager->loadOrGetRenderProgram(
        resources::RenderDrawDataGeometryPassVertexShaderPath,
        resources::RenderTransparentDrawDataGeometryPassFragmentShaderPath,
        {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SceneInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->sceneInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());
}

RenderDrawDataGeometryPass::~RenderDrawDataGeometryPass() = default;

void RenderDrawDataGeometryPass::run(
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

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->colorTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());

    framebuffer->setFaceCulling(false);
    framebuffer->setColorMasks(true);
    framebuffer->setDepthTest(true);
    framebuffer->setDepthMask(true);
    framebuffer->setStencilTest(false);
    framebuffer->setBlending(false);

    renderer->multiDrawArraysIndirectCount(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_opaqueProgram,
        framebuffer,
        vertexArray,
        { sceneData, shared_from_this() },
        utils::PrimitiveType::Triangles,
        renderPipeLine->opaqueCommandsBuffer(),
        renderPipeLine->opaqueParameterBuffer());

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());

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
        { geometryBuffer, sceneData, shared_from_this() },
        utils::PrimitiveType::Triangles,
        renderPipeLine->transparentCommandsBuffer(),
        renderPipeLine->transparentParameterBuffer());
}

BuildClusterPass::BuildClusterPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
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
        glm::uvec3(static_cast<uint32_t>(renderPipeLine->clusterNodesBuffer()->size()), 1u, 1u),
        m_program,
        { shared_from_this() });
}

ClusterLightPass::ClusterLightPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetComputeProgram(resources::ClusterLightPassComputeShaderPath, {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SceneInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->sceneInfoBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CameraBuffer) =
        graphics::BufferRange::create(renderPipeLine->cameraBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ClusterNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->clusterNodesBuffer()->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::LightNodesBuffer) =
        graphics::BufferRange::create(renderPipeLine->lightNodesBuffer()->buffer());
}

ClusterLightPass::~ClusterLightPass() = default;

void ClusterLightPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<const GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    const auto lightsCount = sceneData->lightsBuffer()->size();
    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(lightsCount), 1u, 1u),
        m_program,
        { sceneData, shared_from_this()});
}

RenderBackgroundPass::RenderBackgroundPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
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
    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->finalTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());

    framebuffer->setFaceCulling(false);
    framebuffer->setColorMasks(true);
    framebuffer->setDepthTest(false);
    framebuffer->setStencilTest(false);
    framebuffer->setBlending(false);

    renderer->drawArraysIndirect(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_program,
        framebuffer,
        vertexArray,
        { sceneData, shared_from_this() },
        utils::PrimitiveType::Triangles,
        sceneData->screenQuadCommandsBuffer());
}

BlendPass::BlendPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<RenderPipeLine>& renderPipeLine)
    : RenderPass(renderPipeLine)
{
    m_program = programsManager->loadOrGetRenderProgram(
        resources::BlendPassVertexShaderPath,
        resources::BlendPassFragmentShaderPath,
        {});

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SceneInfoBuffer) =
        graphics::BufferRange::create(renderPipeLine->sceneInfoBuffer()->buffer());

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
    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->finalTexture());

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
        { geometryBuffer, sceneData, shared_from_this()},
        utils::PrimitiveType::Triangles,
        sceneData->screenQuadCommandsBuffer());
}

}
}