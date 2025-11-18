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

FrustumCullingPass::FrustumCullingPass(const std::shared_ptr<ProgramsManager>& programsManager)
    : RenderPass()
{
    m_program = programsManager->loadOrGetComputeProgram(resources::FrustumCullingPassComputeShaderPath, {});

    m_opaqueCommandsBuffer = graphics::DrawArraysIndirectCommandsBuffer::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::OpaqueCommandsBuffer) =
        graphics::BufferRange::create(m_opaqueCommandsBuffer->buffer());

    m_transparentCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::TransparentCommandsBuffer) =
        graphics::BufferRange::create(m_transparentCommandsBuffer->buffer());

    m_ZRangeBuffer = graphics::StructBuffer<glm::uvec2>::create();
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ZRangeBuffer) =
        graphics::BufferRange::create(m_ZRangeBuffer->buffer());

    m_cameraRenderInfo = std::make_shared<CameraRenderInfo>();
}

FrustumCullingPass::~FrustumCullingPass() = default;

void FrustumCullingPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<GeometryBuffer>&,
    const std::shared_ptr<const SceneData>& sceneData)
{
    const auto bufferSize = sceneData->drawDataBuffer()->size();

    m_opaqueCommandsBuffer->resize(bufferSize);
    m_opaqueCommandsBuffer->setReservedData({ 0u, static_cast<uint32_t>(bufferSize), {0u, 0u}});

    m_transparentCommandsBuffer->resize(bufferSize);
    m_transparentCommandsBuffer->setReservedData({ 0u, static_cast<uint32_t>(bufferSize), {0u, 0u} });

    m_ZRangeBuffer->set(glm::uvec2(glm::floatBitsToUint(FLT_MAX), glm::floatBitsToUint(0.f)));
    m_cameraRenderInfo->setFrustum(m_viewTransform, m_clipSpace, m_cullPlanesLimits);

    renderer->compute(
        glm::uvec3(static_cast<uint32_t>(bufferSize), 1u, 1u),
        m_program,
        { m_cameraRenderInfo, sceneData, shared_from_this()});

    utils::Range ZRange(m_cullPlanesLimits.nearValue(), m_cullPlanesLimits.nearValue() + 1.f);

    const auto ZRangeData = m_ZRangeBuffer->get();
    if (auto calculatedZRange = utils::Range(glm::uintBitsToFloat(ZRangeData[0u]), glm::uintBitsToFloat(ZRangeData[1u]));
        !calculatedZRange.isEmpty())
    {
        static const auto s_nearCoef = 0.99f;
        static const auto s_farCoef = 1.01f;
        ZRange = calculatedZRange.translated(m_cullPlanesLimits.nearValue());
        ZRange = m_cullPlanesLimits * utils::Range(ZRange.nearValue() * s_nearCoef, ZRange.farValue() * s_farCoef);
    }

    m_cameraRenderInfo->setFrustum(m_viewTransform, m_clipSpace, ZRange);
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

std::shared_ptr<const CameraRenderInfo> FrustumCullingPass::cameraRenderInfo() const
{
    return m_cameraRenderInfo;
}

graphics::PDrawArraysIndirectCommandsConstBuffer FrustumCullingPass::opaqueCommandsBuffer() const
{
    return m_opaqueCommandsBuffer;
}

graphics::PDrawArraysIndirectCommandsConstBuffer FrustumCullingPass::transparentCommandsBuffer() const
{
    return m_transparentCommandsBuffer;
}

RenderDrawDataGeometryPass::RenderDrawDataGeometryPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<const CameraRenderInfo>& cameraRenderInfo,
    const graphics::PDrawArraysIndirectCommandsConstBuffer& opaqueDrawDataCommandsBuffer,
    const graphics::PDrawArraysIndirectCommandsConstBuffer& transparentDrawDataCommandsBuffer)
    : RenderPass()
    , m_cameraRenderInfo(cameraRenderInfo)
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
    const std::shared_ptr<GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
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
        { m_cameraRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        m_opaqueDrawDataCommandsBuffer);

    geometryBuffer->clearOITBuffer();
    renderer->compute(
        glm::uvec3(geometryBuffer->size(), 1u),
        m_clearOITIndicesImageProgram,
        { geometryBuffer });

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
        { geometryBuffer, m_cameraRenderInfo, sceneData },
        utils::PrimitiveType::Triangles,
        m_transparentDrawDataCommandsBuffer);

    renderer->compute(
        glm::uvec3(geometryBuffer->size(), 1u),
        m_sortOITNodesProgram,
        { geometryBuffer });
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

    renderer->multiDrawArraysIndirectCount(
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