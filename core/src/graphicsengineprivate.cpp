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

debug::GraphicsEngineInformation &GraphicsEnginePrivate::debugInformation()
{
    return m_debugInformation;
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

std::shared_ptr<graphics::IVertexArray> &GraphicsEnginePrivate::screenQuadVertexArray()
{
    return m_screenQuadVertexArray;
}

std::shared_ptr<graphics::IVertexArray> &GraphicsEnginePrivate::pointLightAreaVertexArray()
{
    return m_pointLightAreaVertexArray;
}

std::shared_ptr<graphics::IVertexArray> &GraphicsEnginePrivate::spotLightAreaVertexArray()
{
    return m_spotLightAreaVertexArray;
}

std::shared_ptr<graphics::IVertexArray> &GraphicsEnginePrivate::directionalLightAreaVertexArray()
{
    return m_directionalLightAreaVertexArray;
}

std::shared_ptr<Drawable> &GraphicsEnginePrivate::screenQuadDrawable()
{
    return m_screenQuadDrawable;
}

std::shared_ptr<Drawable> &GraphicsEnginePrivate::finalScreenQuadDrawable()
{
    return m_finalScreenQuadDrawable;
}

std::shared_ptr<PBRDrawable> &GraphicsEnginePrivate::nodeBoundingBoxDrawable()
{
    return m_nodeBoundingBoxDrawable;
}

std::shared_ptr<PBRDrawable> &GraphicsEnginePrivate::cameraNodeCameraDrawable()
{
    return m_cameraNodeCameraDrawable;
}

std::shared_ptr<PBRDrawable> &GraphicsEnginePrivate::cameraNodeFrustumDrawable()
{
    return m_cameraNodeFrustumDrawable;
}

std::shared_ptr<PBRDrawable> &GraphicsEnginePrivate::drawableNodeLocalBoundingBoxDrawable()
{
    return m_drawableNodeLocalBoundingBoxDrawable;
}

}
}
