#include <core/graphicsrendererbase.h>

#include "geometrybuffer.h"
#include "renderpasshelpers.h"
#include "renderpipeline.h"
#include "scenedata.h"

namespace simplex
{
namespace core
{

RenderPipeLine::~RenderPipeLine() = default;

void RenderPipeLine::run(
    uint64_t time,
    const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData,
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& cullPlaneLimits,
    const glm::uvec3& clusterMaxSize)
{
    const auto drawDataCount = sceneData->drawDataCount();
    m_opaqueCommandsBuffer->resize(drawDataCount);
    m_transparentCommandsBuffer->resize(drawDataCount);

    const auto skeletalAnimatedDataCount = sceneData->skeletalAnimatedDataCount();

    const auto clusterNodesCount = glm::compMul(clusterMaxSize);
    m_clusterNodesBuffer->resize(clusterNodesCount);

    const auto lightsCount = sceneData->lightsCount();
    const auto lightNodesCount = clusterNodesCount * lightsCount;
    m_lightNodesBuffer->resize(lightNodesCount);

    m_sceneInfoBuffer->set(SceneInfoDescription::make(
        static_cast<uint32_t>(time),
        static_cast<uint32_t>(drawDataCount),
        static_cast<uint32_t>(skeletalAnimatedDataCount),
        static_cast<uint32_t>(lightsCount),
        static_cast<uint32_t>(lightNodesCount)));
        
    m_cameraBuffer->set(CameraDescription::make(clusterMaxSize, viewTransform, clipSpace, cullPlaneLimits));

    for (auto& pass : m_passes)
        pass->run(m_renderer, m_frameBuffer, m_vertexArray, geometryBuffer, sceneData);
}

SceneInfoBuffer& RenderPipeLine::sceneInfoBuffer()
{
    return m_sceneInfoBuffer;
}

CameraBuffer& RenderPipeLine::cameraBuffer()
{
    return m_cameraBuffer;
}

ClusterNodesBuffer& RenderPipeLine::clusterNodesBuffer()
{
    return m_clusterNodesBuffer;
}

LightNodesBuffer& RenderPipeLine::lightNodesBuffer()
{
    return m_lightNodesBuffer;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::opaqueCommandsBuffer()
{
    return m_opaqueCommandsBuffer;
}

graphics::PDrawArraysIndirectCommandsBuffer& RenderPipeLine::transparentCommandsBuffer()
{
    return m_transparentCommandsBuffer;
}

graphics::PBufferRange& RenderPipeLine::opaqueParameterBuffer()
{
    return m_opaqueParameterBuffer;
}

graphics::PBufferRange& RenderPipeLine::transparentParameterBuffer()
{
    return m_transparentParameterBuffer;
}

std::shared_ptr<RenderPipeLine> RenderPipeLine::create(
    const std::shared_ptr<ProgramsLoader>& programsManager,
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray)
{
    static const auto clear = [](
        const std::shared_ptr<graphics::RendererBase>& renderer,
        const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
        const std::shared_ptr<const SceneData>&)
        {
            geometryBuffer->clear(renderer, frameBuffer);
        };

    static const auto sort = [](
        const std::shared_ptr<graphics::RendererBase>& renderer,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&,
        const std::shared_ptr<const GeometryBuffer>& geometryBuffer,
        const std::shared_ptr<const SceneData>&)
        {
            geometryBuffer->sortOITNodes(renderer);
        };

    auto result = std::shared_ptr<RenderPipeLine>(new RenderPipeLine(programsManager, renderer, frameBuffer, vertexArray));

    auto& passes = result->m_passes;
    passes.push_back(std::make_shared<FrustumCullingPass>(programsManager, result));
    passes.push_back(std::make_shared<UpdateCameraInfoPass>(programsManager, result));
    passes.push_back(std::make_shared<CalculateBonesTransformsDataPass>(programsManager, result));
    passes.push_back(std::make_shared<SimplePass>(result, clear));
    passes.push_back(std::make_shared<RenderDrawDataGeometryPass>(programsManager, result));
    passes.push_back(std::make_shared<SimplePass>(result, sort));
    passes.push_back(std::make_shared<BuildClusterPass>(programsManager, result));
    passes.push_back(std::make_shared<ClusterLightPass>(programsManager, result));
    passes.push_back(std::make_shared<RenderBackgroundPass>(programsManager, result));
    passes.push_back(std::make_shared<BlendPass>(programsManager, result));

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
    m_sceneInfoBuffer = SceneInfoBuffer::element_type::create();
    m_cameraBuffer = CameraBuffer::element_type::create();
    m_clusterNodesBuffer = ClusterNodesBuffer::element_type::create();
    m_lightNodesBuffer = LightNodesBuffer::element_type::create();
    m_opaqueCommandsBuffer = graphics::DrawArraysIndirectCommandsBuffer::create();
    m_transparentCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create();
    m_opaqueParameterBuffer = graphics::PBufferRange::element_type::create(
        m_sceneInfoBuffer->buffer(),
        offsetof(SceneInfoDescription, opaqueCommandsCount),
        sizeof(SceneInfoDescription::opaqueCommandsCount));
    m_transparentParameterBuffer = graphics::PBufferRange::element_type::create(
        m_sceneInfoBuffer->buffer(),
        offsetof(SceneInfoDescription, transparentCommandsCount),
        sizeof(SceneInfoDescription::transparentCommandsCount));
}

}
}
