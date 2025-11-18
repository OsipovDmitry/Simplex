#ifndef GRAPHICSENGINEPRIVATE_H
#define GRAPHICSENGINEPRIVATE_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class RenderPipeLine;
class GeometryBuffer;
class Scene;

class GraphicsEnginePrivate
{
public:
    GraphicsEnginePrivate(const std::string&);

    std::string& name();

    std::shared_ptr<graphics::RendererBase> &renderer();
    std::shared_ptr<TexturesManager> &texturesManager();
    std::shared_ptr<ProgramsManager> &programsManager();

    std::shared_ptr<Scene>& scene();

    std::shared_ptr<RenderPipeLine> &renderPipeLine();

    std::shared_ptr<graphics::IFrameBuffer>& frameBuffer();
    std::shared_ptr<graphics::IVertexArray>& vertexArray();
    std::shared_ptr<GeometryBuffer>& geometryBuffer();

    static const std::string &attributeNameByID(utils::VertexAttribute);
    static const std::string& outputNameByID(graphics::FrameBufferAttachment);
    static const std::string &uniformNameByID(UniformID);
    static const std::string& uniformBlockNameByID(UniformBlockID);
    static const std::string &shaderStorageBlockNameByID(ShaderStorageBlockID);

private:
    std::string m_name;

    std::shared_ptr<graphics::RendererBase> m_renderer;
    std::shared_ptr<TexturesManager> m_texturesManager;
    std::shared_ptr<ProgramsManager> m_programsManager;

    std::shared_ptr<Scene> m_scene;

    std::shared_ptr<RenderPipeLine> m_renderPipeLine;

    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
    std::shared_ptr<GeometryBuffer> m_geometryBuffer;
};

}
}

#endif // GRAPHICSENGINEPRIVATE_H
