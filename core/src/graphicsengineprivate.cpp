#include "graphicsengineprivate.h"

namespace simplex
{
namespace core
{

GraphicsEnginePrivate::GraphicsEnginePrivate(const std::string &name)
    : m_name(name)
{
}

std::string &GraphicsEnginePrivate::name()
{
    return m_name;
}

std::shared_ptr<graphics::IRenderer> &GraphicsEnginePrivate::renderer()
{
    return m_renderer;
}

std::shared_ptr<RenderProgramsManager> &GraphicsEnginePrivate::renderProgramsManager()
{
    return m_renderProgramsManager;
}

std::shared_ptr<RenderSurface> &GraphicsEnginePrivate::defaultRenderSurface()
{
    return m_defaultRenderSurface;
}

std::vector<std::shared_ptr<Scene>> &GraphicsEnginePrivate::scenes()
{
    return m_scenes;
}

std::shared_ptr<DrawableBase> &GraphicsEnginePrivate::screenQuadFinalDrawable()
{
    return m_screenQuadFinalDrawable;
}

std::shared_ptr<StandardDrawable> &GraphicsEnginePrivate::nodeBoundingBoxDrawable()
{
    return m_nodeBoundingBoxDrawable;
}

std::shared_ptr<StandardDrawable> &GraphicsEnginePrivate::cameraNodeCameraDrawable()
{
    return m_cameraNodeCameraDrawable;
}

std::shared_ptr<StandardDrawable> &GraphicsEnginePrivate::cameraNodeFrustumDrawable()
{
    return m_cameraNodeFrustumDrawable;
}

std::shared_ptr<StandardDrawable> &GraphicsEnginePrivate::drawableNodeLocalBoundingBoxDrawable()
{
    return m_drawableNodeLocalBoundingBoxDrawable;
}

}
}
