#ifndef GRAPHICSENGINEPRIVATE_H
#define GRAPHICSENGINEPRIVATE_H

#include <string>
#include <memory>
#include <vector>

#include <utils/glm/vec4.hpp>

#include <core/forwarddecl.h>
#include <core/lightnode.h>

namespace simplex
{
namespace core
{

class GraphicsEnginePrivate
{
public:
    GraphicsEnginePrivate(const std::string&);

    std::string &name();

    std::shared_ptr<graphics::IRenderer> &renderer();
    std::shared_ptr<TexturesManager> &texturesManager();
    std::shared_ptr<ProgramsManager> &programsManager();

    std::shared_ptr<graphics::IComputeProgram> &OITClearComputeProgram();
    std::shared_ptr<graphics::IComputeProgram> &OITSortNodesComputeProgram();

    std::shared_ptr<graphics::IRenderProgram> &finalRenderProgram();

    glm::vec4 &defaultBaseColor();
    float &defaultMetallness();
    float &defaultRoughness();

    std::shared_ptr<core::graphics::IBufferRange> &OITNodesBuffer();
    std::shared_ptr<core::graphics::IBufferRange> &OITNodesCounter();

    std::vector<std::shared_ptr<Scene>> &scenes();

    std::array<std::shared_ptr<DrawableBase>, numElementsLightNodeType()> &lightAreaDrawables();

    std::shared_ptr<DrawableBase> &screenQuadDrawable();
    std::shared_ptr<StandardDrawable> &nodeBoundingBoxDrawable();
    std::shared_ptr<StandardDrawable> &cameraNodeCameraDrawable();
    std::shared_ptr<StandardDrawable> &cameraNodeFrustumDrawable();
    std::shared_ptr<StandardDrawable> &drawableNodeLocalBoundingBoxDrawable();

private:
    std::string m_name;

    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::shared_ptr<TexturesManager> m_texturesManager;
    std::shared_ptr<ProgramsManager> m_programsManager;

    std::shared_ptr<graphics::IRenderProgram> m_deferredOpaqueGeometryPassRenderProgram;
    std::shared_ptr<graphics::IRenderProgram> m_deferredTransparentGeometryPassRenderProgram;
    std::shared_ptr<graphics::IComputeProgram> m_OITClearComputeProgram;
    std::shared_ptr<graphics::IComputeProgram> m_OITSortNodesComputeProgram;
    std::shared_ptr<graphics::IRenderProgram> m_finalRenderProgram;

    glm::vec4 m_defaultBaseColor;
    float m_defaultMetallness;
    float m_defaultRoughness;

    std::shared_ptr<core::graphics::IBufferRange> m_OITNodesBuffer;
    std::shared_ptr<core::graphics::IBufferRange> m_OITNodesCounter;

    std::vector<std::shared_ptr<Scene>> m_scenes;

    std::array<std::shared_ptr<DrawableBase>, numElementsLightNodeType()> m_lightAreaDrawables;

    std::shared_ptr<DrawableBase> m_screenQuadDrawable;
    std::shared_ptr<StandardDrawable> m_nodeBoundingBoxDrawable;
    std::shared_ptr<StandardDrawable> m_cameraNodeCameraDrawable;
    std::shared_ptr<StandardDrawable> m_cameraNodeFrustumDrawable;
    std::shared_ptr<StandardDrawable> m_drawableNodeLocalBoundingBoxDrawable;
};

}
}

#endif // GRAPHICSENGINEPRIVATE_H
