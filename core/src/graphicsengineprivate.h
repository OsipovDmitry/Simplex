#ifndef GRAPHICSENGINEPRIVATE_H
#define GRAPHICSENGINEPRIVATE_H

#include <string>
#include <memory>
#include <vector>

#include <core/forwarddecl.h>

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
    std::shared_ptr<RenderProgramsManager> &renderProgramsManager();
    std::shared_ptr<RenderSurface> &defaultRenderSurface();
    std::vector<std::shared_ptr<Scene>> &scenes();
    std::shared_ptr<DrawableBase> &screenQuadFinalDrawable();
    std::shared_ptr<StandardDrawable> &nodeBoundingBoxDrawable();
    std::shared_ptr<StandardDrawable> &cameraNodeCameraDrawable();
    std::shared_ptr<StandardDrawable> &cameraNodeFrustumDrawable();
    std::shared_ptr<StandardDrawable> &drawableNodeLocalBoundingBoxDrawable();

private:
    std::string m_name;
    std::shared_ptr<graphics::IRenderer> m_renderer;
    std::shared_ptr<RenderProgramsManager> m_renderProgramsManager;
    std::shared_ptr<RenderSurface> m_defaultRenderSurface;
    std::vector<std::shared_ptr<Scene>> m_scenes;
    std::shared_ptr<DrawableBase> m_screenQuadFinalDrawable;
    std::shared_ptr<StandardDrawable> m_nodeBoundingBoxDrawable;
    std::shared_ptr<StandardDrawable> m_cameraNodeCameraDrawable;
    std::shared_ptr<StandardDrawable> m_cameraNodeFrustumDrawable;
    std::shared_ptr<StandardDrawable> m_drawableNodeLocalBoundingBoxDrawable;
};

}
}

#endif // GRAPHICSENGINEPRIVATE_H
