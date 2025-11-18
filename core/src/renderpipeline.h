#ifndef CORE_RENDERPIPELINE_H
#define CORE_RENDERPIPELINE_H

#include <memory>
#include <vector>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class FrustumCullingPass;
class GeometryBuffer;
class ProgramsManager;
class RenderPass;
class SceneData;

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

    void run(
        const std::shared_ptr<GeometryBuffer>&,
        const std::shared_ptr<const SceneData>&);

private:
    std::shared_ptr<FrustumCullingPass> m_frustumCullingPass;
    std::vector<std::shared_ptr<RenderPass>> m_passes;
    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
};

}
}

#endif // CORE_RENDERPIPELINE_H