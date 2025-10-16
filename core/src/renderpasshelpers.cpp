#include <core/programsmanager.h>
#include <core/graphicsrendererbase.h>
#include <core/uniform.h>

#include "geometrybuffer.h"
#include "geometryrenderinfo.h"
#include "resources.h"
#include "renderpasshelpers.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

BuildDrawDataCommandsBufferPass::BuildDrawDataCommandsBufferPass(const std::shared_ptr<ProgramsManager>& programsManager)
    : Pass()
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildDrawDataCommandsBufferPassComputeShaderPath, {});

    m_opaqueCommandsBuffer = graphics::DrawArraysIndirectCommandsBuffer::create();
    m_transparentCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create();

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OpaqueCommandsBuffer) =
        graphics::BufferRange::create(m_opaqueCommandsBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::TransparentCommandsBuffer) =
        graphics::BufferRange::create(m_transparentCommandsBuffer->buffer());
}

BuildDrawDataCommandsBufferPass::~BuildDrawDataCommandsBufferPass() = default;

void BuildDrawDataCommandsBufferPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<GeometryRenderInfo>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    const auto bufferSize = sceneData->drawDataBuffer()->size();

    m_opaqueCommandsBuffer->resize(bufferSize);
    m_opaqueCommandsBuffer->setReservedData({ 0u, static_cast<uint32_t>(bufferSize), {0u, 0u}});

    m_transparentCommandsBuffer->resize(bufferSize);
    m_transparentCommandsBuffer->setReservedData({ 0u, static_cast<uint32_t>(bufferSize), {0u, 0u} });

    renderer->compute(glm::uvec3(static_cast<uint32_t>(bufferSize), 1u, 1u), m_program, { sceneData, shared_from_this()});
}

graphics::PDrawArraysIndirectCommandsConstBuffer BuildDrawDataCommandsBufferPass::opaqueCommandsBuffer() const
{
    return m_opaqueCommandsBuffer;
}

graphics::PDrawArraysIndirectCommandsConstBuffer BuildDrawDataCommandsBufferPass::transparentCommandsBuffer() const
{
    return m_transparentCommandsBuffer;
}

RenderDrawDataGeometryPass::RenderDrawDataGeometryPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const graphics::PDrawArraysIndirectCommandsConstBuffer& opaqueDrawDataCommandsBuffer,
    const graphics::PDrawArraysIndirectCommandsConstBuffer& transparentDrawDataCommandsBuffer)
    : Pass()
    , m_opaqueDrawDataCommandsBuffer(opaqueDrawDataCommandsBuffer)
    , m_transparentDrawDataCommandsBuffer(transparentDrawDataCommandsBuffer)
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
    const std::shared_ptr<GeometryRenderInfo>& geometryRenderInfo,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto geometryBuffer = geometryRenderInfo->geometryBuffer();

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->colorTexture0());
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::Stencil, geometryBuffer->stencilTexture());

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
        { geometryRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        m_opaqueDrawDataCommandsBuffer);

    geometryBuffer->clearOITBuffer();
    renderer->compute(
        glm::uvec3(geometryBuffer->size(), 1u),
        m_clearOITIndicesImageProgram,
        { geometryRenderInfo });

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::Stencil, geometryBuffer->stencilTexture());

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
        { geometryRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        m_transparentDrawDataCommandsBuffer);

    renderer->compute(
        glm::uvec3(geometryBuffer->size(), 1u),
        m_sortOITNodesProgram,
        { geometryRenderInfo });
}

RenderBackgroundPass::RenderBackgroundPass(const std::shared_ptr<ProgramsManager>& programsManager)
    : Pass()
{
    m_program = programsManager->loadOrGetRenderProgram(
        resources::RenderBackgroundPassVertexShaderPath, resources::RenderBackgroundPassFragmentShaderPath, {});
}

RenderBackgroundPass::~RenderBackgroundPass() = default;

void RenderBackgroundPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<GeometryRenderInfo>& geometryRenderInfo,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto geometryBuffer = geometryRenderInfo->geometryBuffer();

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->finalTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::Stencil, geometryBuffer->stencilTexture());

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

    renderer->multiDrawArraysIndirectCount(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_program,
        framebuffer,
        vertexArray,
        { geometryRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        sceneData->screenQuadCommandsBuffer());
}

BuildLightsCommandsBufferPass::BuildLightsCommandsBufferPass(const std::shared_ptr<ProgramsManager>& programsManager)
    : Pass()
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildLightsCommandsBufferPassComputeShaderPath, {});

    m_lightsCommandsBuffer = graphics::DrawArraysIndirectCommandsBuffer::create();

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::CommandsBuffer) =
        graphics::BufferRange::create(m_lightsCommandsBuffer->buffer());
}

BuildLightsCommandsBufferPass::~BuildLightsCommandsBufferPass() = default;

void BuildLightsCommandsBufferPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<GeometryRenderInfo>& geometryRenderInfo,
    const std::shared_ptr<const SceneData>& sceneData)
{
    const auto totalLightsCount =
        sceneData->pointLightsBuffer()->size() +
        sceneData->spotLightsBuffer()->size() +
        sceneData->directionalLightsBuffer()->size() +
        sceneData->imageBasedLightsBuffer()->size();

    m_lightsCommandsBuffer->resize(totalLightsCount);
    m_lightsCommandsBuffer->setReservedData({ 0u, static_cast<uint32_t>(totalLightsCount), {0u, 0u} });

    renderer->compute(glm::uvec3(static_cast<uint32_t>(totalLightsCount), 1u, 1u), m_program, { sceneData, shared_from_this() });
}

graphics::PDrawArraysIndirectCommandsConstBuffer BuildLightsCommandsBufferPass::lightsCommandsBuffer()
{
    return m_lightsCommandsBuffer;
}

RenderDrawDataStencilPass::RenderDrawDataStencilPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const graphics::PDrawArraysIndirectCommandsConstBuffer& lightsCommandsBuffer)
    : Pass()
    , m_lightsCommandsBuffer(lightsCommandsBuffer)
{
    m_program = programsManager->loadOrGetRenderProgram(
        resources::RenderDrawDataStencilPassVertexShaderPath,
        resources::RenderDrawDataStencilPassFragmentShaderPath,
        {});
}

RenderDrawDataStencilPass::~RenderDrawDataStencilPass() = default;

void RenderDrawDataStencilPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<GeometryRenderInfo>& geometryRenderInfo,
    const std::shared_ptr<const SceneData>& sceneData)
{
}

BlendPass::BlendPass(const std::shared_ptr<ProgramsManager>& programsManager)
    : Pass()
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
    const std::shared_ptr<GeometryRenderInfo>& geometryRenderInfo,
    const std::shared_ptr<const SceneData>& sceneData)
{
    auto geometryBuffer = geometryRenderInfo->geometryBuffer();

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

    renderer->multiDrawArraysIndirectCount(
        glm::uvec4(0u, 0u, geometryBuffer->size()),
        m_program,
        framebuffer,
        vertexArray,
        { geometryRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        sceneData->screenQuadCommandsBuffer());
}

}
}