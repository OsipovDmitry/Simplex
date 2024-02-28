#include "renderinfoprivate.h"

namespace simplex
{
namespace core
{

RenderInfoPrivate::RenderInfoPrivate()
{}

PUniform<glm::mat4x4> &RenderInfoPrivate::viewMatrixUniform()
{
    return m_viewMatrixUniform;
}

PUniform<glm::mat4x4> &RenderInfoPrivate::viewMatrixInverseUniform()
{
    return m_viewMatrixInverseUniform;
}

PUniform<glm::mat4x4> &RenderInfoPrivate::projectionMatrixUniform()
{
    return m_projectionMatrixUniform;
}

PUniform<glm::mat4x4> &RenderInfoPrivate::projectionMatrixInverseUniform()
{
    return m_projectionMatrixInverseUniform;
}

PUniform<glm::mat4x4> &RenderInfoPrivate::viewProjectionMatrixUniform()
{
    return m_viewProjectionMatrixUniform;
}

PUniform<glm::mat4x4> &RenderInfoPrivate::viewProjectionMatrixInverseUniform()
{
    return m_viewProjectionMatrixInverseUniform;
}

PUniform<glm::vec3> &RenderInfoPrivate::viewPositionUniform()
{
    return m_viewPositionUniform;
}

PUniform<glm::vec3> &RenderInfoPrivate::viewXDirectionUniform()
{
    return m_viewXDirectionUniform;
}

PUniform<glm::vec3> &RenderInfoPrivate::viewYDirectionUniform()
{
    return m_viewYDirectionUniform;
}

PUniform<glm::vec3> &RenderInfoPrivate::viewZDirectionUniform()
{
    return m_viewZDirectionUniform;
}

PUniform<graphics::PConstTexture> &RenderInfoPrivate::GBufferColor0TextureUniform()
{
    return m_GBufferColor0TextureUniform;
}

PUniform<graphics::PConstTexture> &RenderInfoPrivate::GBufferColor1TextureUniform()
{
    return m_GBufferColor1TextureUniform;
}

PUniform<graphics::PConstTexture> &RenderInfoPrivate::GBufferColor2TextureUniform()
{
    return m_GBufferColor2TextureUniform;
}

PUniform<graphics::PConstTexture> &RenderInfoPrivate::GBufferDepthTextureUniform()
{
    return m_GBufferDepthTextureUniform;
}

graphics::PConstBufferRange &RenderInfoPrivate::OITDataBuffer()
{
    return m_OITDataBuffer;
}

PUniform<graphics::PConstBufferRange> &RenderInfoPrivate::OITNodesCounterUniform()
{
    return m_OITNodesCounterUniform;
}

PUniform<graphics::PConstImage> &RenderInfoPrivate::OITDepthImageUniform()
{
    return m_OITDepthImageUniform;
}

PUniform<graphics::PConstImage> &RenderInfoPrivate::OITIndicesImageUniform()
{
    return m_OITIndicesImageUniform;
}

PUniform<graphics::PConstTexture> &RenderInfoPrivate::lightBufferColorTextureUniform()
{
    return m_lightBufferColorTextureUniform;
}

PUniform<graphics::PConstTexture> &RenderInfoPrivate::finalBufferColorTextureUniform()
{
    return m_finalBufferColorTextureUniform;
}

}
}
