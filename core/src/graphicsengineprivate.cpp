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

std::shared_ptr<TexturesManager> &GraphicsEnginePrivate::texturesManager()
{
    return m_texturesManager;
}

std::shared_ptr<ProgramsManager> &GraphicsEnginePrivate::programsManager()
{
    return m_programsManager;
}

std::shared_ptr<graphics::IComputeProgram> &GraphicsEnginePrivate::OITClearComputeProgram()
{
    return m_OITClearComputeProgram;
}

std::shared_ptr<graphics::IComputeProgram> &GraphicsEnginePrivate::OITSortNodesComputeProgram()
{
    return m_OITSortNodesComputeProgram;
}

std::shared_ptr<graphics::IRenderProgram> &GraphicsEnginePrivate::finalRenderProgram()
{
    return m_finalRenderProgram;
}

glm::vec4 &GraphicsEnginePrivate::defaultBaseColor()
{
    return m_defaultBaseColor;
}

float &GraphicsEnginePrivate::defaultMetallness()
{
    return m_defaultMetallness;
}

float &GraphicsEnginePrivate::defaultRoughness()
{
    return m_defaultRoughness;
}

std::shared_ptr<graphics::IBufferRange> &GraphicsEnginePrivate::OITNodesBuffer()
{
    return m_OITNodesBuffer;
}

std::shared_ptr<graphics::IBufferRange> &GraphicsEnginePrivate::OITNodesCounter()
{
    return m_OITNodesCounter;
}

std::vector<std::shared_ptr<Scene>> &GraphicsEnginePrivate::scenes()
{
    return m_scenes;
}

std::array<std::shared_ptr<DrawableBase>, numElementsLightNodeType()> &GraphicsEnginePrivate::lightAreaDrawables()
{
    return m_lightAreaDrawables;
}

std::shared_ptr<DrawableBase> &GraphicsEnginePrivate::screenQuadDrawable()
{
    return m_screenQuadDrawable;
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
