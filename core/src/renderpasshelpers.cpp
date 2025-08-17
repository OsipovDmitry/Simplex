#include <core/programsmanager.h>
#include <core/graphicsrendererbase.h>
#include <core/uniform.h>

#include "geometrybuffer.h"
#include "geometryrenderinfo.h"
#include "resources.h"
#include "renderpasshelpers.h"

namespace simplex
{
namespace core
{

BuildBackgroundsCommandsBufferPass::BuildBackgroundsCommandsBufferPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<SceneData>& sceneData)
    : Pass()
    , m_commandsBuffer(sceneData->backgroundsCommandsBuffer())
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildBackgroundsCommandsBufferPassComputeShaderPath, {});

    getOrCreateSSBO(SSBOId::MeshesBuffer) = graphics::BufferRange::create(sceneData->meshesBuffer()->buffer());
    getOrCreateSSBO(SSBOId::BackgroundsBuffer) = graphics::BufferRange::create(sceneData->backgroundsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::CommandsBuffer) = graphics::BufferRange::create(sceneData->backgroundsCommandsBuffer()->buffer());
}

BuildBackgroundsCommandsBufferPass::~BuildBackgroundsCommandsBufferPass() = default;

void BuildBackgroundsCommandsBufferPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<GeometryRenderInfo>&)
{
    m_commandsBuffer->setReservedData({ 0u, /*{0u, 0u, 0u}*/ });
    renderer->compute(m_program, glm::uvec3(m_commandsBuffer->size(), 1u, 1u), { shared_from_this() });
}

BuildDrawDataCommandsBufferPass::BuildDrawDataCommandsBufferPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<SceneData>& sceneData)
    : Pass()
    , m_drawDataBuffer(sceneData->drawDataBuffer())
    , m_opaqueCommandsBuffer(sceneData->opaqueDrawDataCommandsBuffer())
    , m_transparentCommandsBuffer(sceneData->transparentDrawDataCommandsBuffer())
{
    m_program = programsManager->loadOrGetComputeProgram(resources::BuildDrawDataCommandsBufferPassComputeShaderPath, {});

    getOrCreateSSBO(SSBOId::MeshesBuffer) = graphics::BufferRange::create(sceneData->meshesBuffer()->buffer());
    getOrCreateSSBO(SSBOId::MaterialsBuffer) = graphics::BufferRange::create(sceneData->materialsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::DrawablesBuffer) = graphics::BufferRange::create(sceneData->drawablesBuffer()->buffer());
    getOrCreateSSBO(SSBOId::DrawDataBuffer) = graphics::BufferRange::create(sceneData->drawDataBuffer()->buffer());
    getOrCreateSSBO(SSBOId::OpaqueCommandsBuffer) =
        graphics::BufferRange::create(sceneData->opaqueDrawDataCommandsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::TransparentCommandsBuffer) =
        graphics::BufferRange::create(sceneData->transparentDrawDataCommandsBuffer()->buffer());

    auto reserved0 = m_drawDataBuffer->reservedData();
    std::vector<utils::IDsGenerator::value_type> data0;
    for (size_t i = 0u; i < m_drawDataBuffer->size(); ++i)
        data0.push_back(glm::floatBitsToUint(m_drawDataBuffer->get(i).normalMatrixAndDrawableOffset[0u][3u]));

    std::vector<simplex::core::DrawablesBuffer::element_type::value_type> data1;
    for (size_t i = 0u; i < data0.size(); ++i)
        data1.push_back(sceneData->drawablesBuffer()->get(data0[i]));

    std::vector<simplex::core::MeshesBuffer::element_type::value_type> data2;
    for (size_t i = 0u; i < data1.size(); ++i)
        data2.push_back(sceneData->meshesBuffer()->get(data1[i].meshOffset));

    std::vector<uint32_t> data3;
    for (size_t i = 0u; i < 100u; ++i)
        data3.push_back(sceneData->indicesBuffer()->get(i));

    int i = sizeof(simplex::core::DrawDataBuffer::element_type::value_type);
}

BuildDrawDataCommandsBufferPass::~BuildDrawDataCommandsBufferPass() = default;

void BuildDrawDataCommandsBufferPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>&,
    const std::shared_ptr<graphics::IVertexArray>&,
    const std::shared_ptr<GeometryRenderInfo>&)
{
    m_opaqueCommandsBuffer->setReservedData({ 0u, {0u, 0u, 0u} });
    m_transparentCommandsBuffer->setReservedData({ 0u, {0u, 0u, 0u} });

    renderer->compute(m_program, glm::uvec3(m_drawDataBuffer->size(), 1u, 1u), { shared_from_this() });
}

RenderOpaqueDrawDataGeometryPass::RenderOpaqueDrawDataGeometryPass(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<SceneData>& sceneData)
    : Pass()
{
    m_program = programsManager->loadOrGetRenderProgram(
        resources::RenderOpaqueDrawDataPassVertexShaderPath,
        resources::RenderOpaqueDrawDataPassFragmentShaderPath,
        {});

    m_drawIndirectBufferRange = graphics::BufferRange::create(
        sceneData->opaqueDrawDataCommandsBuffer()->buffer(),
        DrawIndirectElementsCommandsBuffer::element_type::sizeofReservedType());

    m_parameterBufferRange = graphics::BufferRange::create(
        sceneData->opaqueDrawDataCommandsBuffer()->buffer(),
        0u,
        DrawIndirectElementsCommandsBuffer::element_type::sizeofReservedType());

    getOrCreateSSBO(SSBOId::PositionsBuffer) = graphics::BufferRange::create(sceneData->positionsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::NormalsBuffer) = graphics::BufferRange::create(sceneData->normalsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::TexCoordsBuffer) = graphics::BufferRange::create(sceneData->texCoordsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::BonesBuffer) = graphics::BufferRange::create(sceneData->bonesBuffer()->buffer());
    getOrCreateSSBO(SSBOId::TangentsBuffer) = graphics::BufferRange::create(sceneData->tangentsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::ColorsBuffer) = graphics::BufferRange::create(sceneData->colorsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::IndicesBuffer) = graphics::BufferRange::create(sceneData->indicesBuffer()->buffer());
    getOrCreateSSBO(SSBOId::MeshesBuffer) = graphics::BufferRange::create(sceneData->meshesBuffer()->buffer());
    getOrCreateSSBO(SSBOId::MaterialMapsBuffer) = graphics::BufferRange::create(sceneData->materialMapsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::MaterialsBuffer) = graphics::BufferRange::create(sceneData->materialsBuffer()->buffer());
    getOrCreateSSBO(SSBOId::DrawablesBuffer) = graphics::BufferRange::create(sceneData->drawablesBuffer()->buffer());
    getOrCreateSSBO(SSBOId::DrawDataBuffer) = graphics::BufferRange::create(sceneData->drawDataBuffer()->buffer());
}

RenderOpaqueDrawDataGeometryPass::~RenderOpaqueDrawDataGeometryPass() = default;

void RenderOpaqueDrawDataGeometryPass::run(
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray,
    const std::shared_ptr<GeometryRenderInfo>& geometryRenderInfo)
{
    auto geometryBuffer = geometryRenderInfo->geometryBuffer();

    framebuffer->detachAll();
    framebuffer->attach(graphics::FrameBufferAttachment::Color0, geometryBuffer->colorTexture0());
    framebuffer->attach(graphics::FrameBufferAttachment::Color1, geometryBuffer->colorTexture1());
    framebuffer->attach(graphics::FrameBufferAttachment::Color2, geometryBuffer->colorTexture2());
    framebuffer->attach(graphics::FrameBufferAttachment::Depth, geometryBuffer->depthTexture());
    framebuffer->attach(graphics::FrameBufferAttachment::Stencil, geometryBuffer->stencilTexture());

    framebuffer->setClearDepthStencil(1.f, 0x00u);
    framebuffer->setClearColor(0u, glm::vec4(1.f, 0.f, 0.f, 1.f));
    framebuffer->setClearColor(1u, glm::vec4(0.f));
    framebuffer->setClearColor(2u, glm::vec4(0.f));
    framebuffer->setClearMask({
        graphics::FrameBufferAttachment::Color0,
        graphics::FrameBufferAttachment::Color1,
        graphics::FrameBufferAttachment::Color2,
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

    renderer->multiDrawElementsIndirectCount(
        m_program,
        framebuffer,
        vertexArray,
        { geometryRenderInfo, shared_from_this() },
        utils::PrimitiveType::Triangles,
        utils::DrawElementsIndexType::Uint32,
        m_drawIndirectBufferRange,
        m_parameterBufferRange,
        m_drawIndirectBufferRange->size());
}

}
}