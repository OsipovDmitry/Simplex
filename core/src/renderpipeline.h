#ifndef CORE_RENDERPIPELINE_H
#define CORE_RENDERPIPELINE_H

#include <memory>
#include <vector>

#include <core/forwarddecl.h>

#include "descriptions.h"

namespace simplex
{
namespace core
{

class GeometryBuffer;
class ProgramsManager;
class RenderPass;
class SceneData;

using SceneInfoBuffer = std::shared_ptr<graphics::StructBuffer<SceneInfoDescription>>;
using CameraBuffer = std::shared_ptr<graphics::StructBuffer<CameraDescription>>;
using ClusterNodesBuffer = std::shared_ptr<graphics::VectorBuffer<ClusterNodeDescription>>;
using LightNodesBuffer = std::shared_ptr<graphics::VectorBuffer<LightNodeDescription>>;

class RenderPipeLine
{
public:
    ~RenderPipeLine();

    void run(
        uint64_t time,
        const std::shared_ptr<const GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&,
        const utils::Transform&,
        const utils::ClipSpace&,
        const utils::Range&,
        const glm::uvec3&);

    SceneInfoBuffer& sceneInfoBuffer();
    CameraBuffer& cameraBuffer();
    ClusterNodesBuffer& clusterNodesBuffer();
    LightNodesBuffer& lightNodesBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& opaqueCommandsBuffer();
    graphics::PDrawArraysIndirectCommandsBuffer& transparentCommandsBuffer();
    graphics::PBufferRange& opaqueParameterBuffer();
    graphics::PBufferRange& transparentParameterBuffer();

    static std::shared_ptr<RenderPipeLine> create(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&);

private:
    RenderPipeLine(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&);

    SceneInfoBuffer m_sceneInfoBuffer;
    CameraBuffer m_cameraBuffer;
    ClusterNodesBuffer m_clusterNodesBuffer;
    LightNodesBuffer m_lightNodesBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_opaqueCommandsBuffer;
    graphics::PDrawArraysIndirectCommandsBuffer m_transparentCommandsBuffer;
    graphics::PBufferRange m_opaqueParameterBuffer;
    graphics::PBufferRange m_transparentParameterBuffer;

    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;

    std::vector<std::shared_ptr<RenderPass>> m_passes;
};

}
}

#endif // CORE_RENDERPIPELINE_H