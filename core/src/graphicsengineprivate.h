#ifndef GRAPHICSENGINEPRIVATE_H
#define GRAPHICSENGINEPRIVATE_H

#include <string>
#include <memory>
#include <vector>

#include <utils/glm/vec4.hpp>

#include <core/forwarddecl.h>
#include <core/debuginformation.h>

namespace simplex
{
namespace core
{

class GraphicsEnginePrivate
{
public:
    GraphicsEnginePrivate(const std::string&);

    std::string &name();

    debug::GraphicsEngineInformation &debugInformation();

    std::shared_ptr<graphics::IRenderer> &renderer();
    std::shared_ptr<TexturesManager> &texturesManager();
    std::shared_ptr<ProgramsManager> &programsManager();

    std::shared_ptr<core::graphics::IBufferRange> &OITNodesBuffer();
    std::shared_ptr<core::graphics::IBufferRange> &OITNodesCounter();

    std::vector<std::shared_ptr<Scene>> &scenes();

    std::shared_ptr<graphics::IVertexArray> &screenQuadVertexArray();

    std::shared_ptr<graphics::IVertexArray> &pointLightAreaVertexArray();
    std::shared_ptr<graphics::IVertexArray> &spotLightAreaVertexArray();
    std::shared_ptr<graphics::IVertexArray> &directionalLightAreaVertexArray();

    std::shared_ptr<Drawable> &screenQuadDrawable();

    std::shared_ptr<Drawable> &finalScreenQuadDrawable();
    std::shared_ptr<PBRDrawable> &nodeBoundingBoxDrawable();
    std::shared_ptr<PBRDrawable> &cameraNodeCameraDrawable();
    std::shared_ptr<PBRDrawable> &cameraNodeFrustumDrawable();
    std::shared_ptr<PBRDrawable> &drawableNodeLocalBoundingBoxDrawable();

private:
    std::string m_name;

    debug::GraphicsEngineInformation m_debugInformation;

    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::shared_ptr<TexturesManager> m_texturesManager;
    std::shared_ptr<ProgramsManager> m_programsManager;

    glm::vec4 m_defaultBaseColor;
    float m_defaultMetallness;
    float m_defaultRoughness;

    std::shared_ptr<core::graphics::IBufferRange> m_OITNodesBuffer;
    std::shared_ptr<core::graphics::IBufferRange> m_OITNodesCounter;

    std::vector<std::shared_ptr<Scene>> m_scenes;

    std::shared_ptr<graphics::IVertexArray> m_screenQuadVertexArray;

    std::shared_ptr<graphics::IVertexArray> m_pointLightAreaVertexArray;
    std::shared_ptr<graphics::IVertexArray> m_spotLightAreaVertexArray;
    std::shared_ptr<graphics::IVertexArray> m_directionalLightAreaVertexArray;

    std::shared_ptr<Drawable> m_screenQuadDrawable;

    std::shared_ptr<Drawable> m_finalScreenQuadDrawable;
    std::shared_ptr<PBRDrawable> m_nodeBoundingBoxDrawable;
    std::shared_ptr<PBRDrawable> m_cameraNodeCameraDrawable;
    std::shared_ptr<PBRDrawable> m_cameraNodeFrustumDrawable;
    std::shared_ptr<PBRDrawable> m_drawableNodeLocalBoundingBoxDrawable;
};

}
}

#endif // GRAPHICSENGINEPRIVATE_H
