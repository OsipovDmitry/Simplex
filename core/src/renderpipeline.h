#ifndef CORE_RENDERPIPELINE_H
#define CORE_RENDERPIPELINE_H

#include <memory>
#include <vector>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class CameraRenderInfo;
class GeometryBuffer;
class ProgramsManager;
class RenderPass;
class SceneData;

class FrustumCullingPass;
class BuildClusterPass;

class RenderPipeLine
{
public:
    RenderPipeLine(
        const std::shared_ptr<ProgramsManager>&,
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&,
        const std::shared_ptr<graphics::IVertexArray>&);
    ~RenderPipeLine();

    void setFrustum(const utils::Transform&, const utils::ClipSpace&, const utils::Range&);
    void setCluster(const glm::uvec3&);

    void run(
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&);

    //tmp 
    std::shared_ptr<CameraRenderInfo> cameraRenderInfo() const { return m_cameraRenderInfo; }

private:
    std::shared_ptr<FrustumCullingPass> m_frustumCullingPass;
    std::shared_ptr<BuildClusterPass> m_buildClusterPass;
    std::vector<std::shared_ptr<RenderPass>> m_passes;

    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;

    std::shared_ptr<CameraRenderInfo> m_cameraRenderInfo;
};

}
}

#endif // CORE_RENDERPIPELINE_H