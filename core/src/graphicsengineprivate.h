#ifndef GRAPHICSENGINEPRIVATE_H
#define GRAPHICSENGINEPRIVATE_H

#include <utils/boundingbox.h>

#include <core/forwarddecl.h>

#include "scenedata.h"

namespace simplex
{
namespace core
{

class Pass;
class GeometryBuffer;

class GraphicsEnginePrivate
{
public:
    GraphicsEnginePrivate(const std::string&);

    std::string& name();

    std::shared_ptr<graphics::RendererBase> &renderer();
    std::shared_ptr<TexturesManager> &texturesManager();
    std::shared_ptr<ProgramsManager> &programsManager();

    std::shared_ptr<Scene>& scene();

    std::vector<std::shared_ptr<Pass>> &passes();

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

    std::vector<std::shared_ptr<Pass>> m_passes;

    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    std::shared_ptr<graphics::IVertexArray> m_vertexArray;
    std::shared_ptr<GeometryBuffer> m_geometryBuffer;
};

}
}

#endif // GRAPHICSENGINEPRIVATE_H
