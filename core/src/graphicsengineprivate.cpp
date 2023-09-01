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

std::shared_ptr<graphics::IRenderProgram> &GraphicsEnginePrivate::deferredOpaqueGeometryPassRenderProgram()
{
    return m_deferredOpaqueGeometryPassRenderProgram;
}

std::shared_ptr<graphics::IRenderProgram> &GraphicsEnginePrivate::deferredTransparentGeometryPassRenderProgram()
{
    return m_deferredTransparentGeometryPassRenderProgram;
}

std::shared_ptr<graphics::IRenderProgram> &GraphicsEnginePrivate::OITSortNodesRenderProgram()
{
    return m_OITSortNodesRenderProgram;
}

std::shared_ptr<graphics::IRenderProgram> &GraphicsEnginePrivate::finalRenderProgram()
{
    return m_finalRenderProgram;
}

glm::vec4 &GraphicsEnginePrivate::defaultVertexPosition()
{
    return m_defaultVertexPosition;
}

glm::vec4 &GraphicsEnginePrivate::defaultVertexNormal()
{
    return m_defaultVertexNormal;
}

glm::vec4 &GraphicsEnginePrivate::defaultVertexTexCoord()
{
    return m_defaultVertexTexCoord;
}

glm::uvec4 &GraphicsEnginePrivate::defaultVertexBonesIDs()
{
    return m_defaultVertexBonesIDs;
}

glm::vec4 &GraphicsEnginePrivate::defaultVertexBonesWeights()
{
    return m_defaultVertexBonesWeights;
}

glm::vec4 &GraphicsEnginePrivate::defaultVertexTangent()
{
    return m_defaultVertexTangent;
}

glm::vec4 &GraphicsEnginePrivate::defaultVertexColor()
{
    return m_defaultVertexColor;
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

std::shared_ptr<graphics::ITexture> &GraphicsEnginePrivate::defaultBaseColorMap()
{
    return m_defaultBaseColorMap;
}

std::shared_ptr<graphics::ITexture> &GraphicsEnginePrivate::defaultMetalnessMap()
{
    return m_defaultMetalnessMap;
}

std::shared_ptr<graphics::ITexture> &GraphicsEnginePrivate::defaultRoughnessMap()
{
    return m_defaultRoughnessMap;
}

std::shared_ptr<graphics::ITexture> &GraphicsEnginePrivate::defaultNormalMap()
{
    return m_defaultNormalMap;
}

std::shared_ptr<graphics::IBufferRange> &GraphicsEnginePrivate::defaultBonesBuffer()
{
    return m_defaultBonesBuffer;
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
