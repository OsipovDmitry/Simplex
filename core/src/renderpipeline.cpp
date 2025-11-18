#include "renderpasshelpers.h"
#include "renderpipeline.h"

namespace simplex
{
namespace core
{

RenderPipeLine::RenderPipeLine(
    const std::shared_ptr<ProgramsManager>& programsManager,
    const std::shared_ptr<graphics::RendererBase>& renderer,
    const std::shared_ptr<graphics::IFrameBuffer>& frameBuffer,
    const std::shared_ptr<graphics::IVertexArray>& vertexArray)
    : m_renderer(renderer)
    , m_frameBuffer(frameBuffer)
    , m_vertexArray(vertexArray)
{
    m_frustumCullingPass = std::make_shared<FrustumCullingPass>(programsManager);
    m_passes.push_back(m_frustumCullingPass);

    auto renderDrawDataGeometryPass = std::make_shared<RenderDrawDataGeometryPass>(
        programsManager,
        m_frustumCullingPass->cameraRenderInfo(),
        m_frustumCullingPass->opaqueCommandsBuffer(),
        m_frustumCullingPass->transparentCommandsBuffer());
    m_passes.push_back(renderDrawDataGeometryPass);

    auto renderBackgroundPass = std::make_shared<RenderBackgroundPass>(
        programsManager,
        m_frustumCullingPass->cameraRenderInfo());
    m_passes.push_back(renderBackgroundPass);

    auto blendPass = std::make_shared<BlendPass>(programsManager);
    m_passes.push_back(blendPass);
}

RenderPipeLine::~RenderPipeLine() = default;

void RenderPipeLine::setFrustum(
    const utils::Transform& viewTransform,
    const utils::ClipSpace& clipSpace,
    const utils::Range& ZRange)
{
    m_frustumCullingPass->setFrustum(viewTransform, clipSpace, ZRange);
}

void RenderPipeLine::run(
    const std::shared_ptr<GeometryBuffer>& geometryBuffer,
    const std::shared_ptr<const SceneData>& sceneData)
{
    for (auto& pass : m_passes)
        pass->run(m_renderer, m_frameBuffer, m_vertexArray, geometryBuffer, sceneData);
}

}
}
