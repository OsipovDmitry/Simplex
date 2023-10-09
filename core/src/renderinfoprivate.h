#ifndef RENDERINFOPRIVATE_H
#define RENDERINFOPRIVATE_H

#include <memory>

#include <utils/glm/vec3.hpp>
#include <utils/glm/mat4x4.hpp>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class RenderInfoPrivate
{
public:
    RenderInfoPrivate();

    PUniform<glm::mat4x4> &viewMatrixUniform();
    PUniform<glm::mat4x4> &viewMatrixInverseUniform();
    PUniform<glm::mat4x4> &projectionMatrixUniform();
    PUniform<glm::mat4x4> &projectionMatrixInverseUniform();
    PUniform<glm::mat4x4> &viewProjectionMatrixUniform();
    PUniform<glm::mat4x4> &viewProjectionMatrixInverseUniform();
    PUniform<glm::vec3> &viewPositionUniform();
    PUniform<glm::vec3> &viewXDirectionUniform();
    PUniform<glm::vec3> &viewYDirectionUniform();
    PUniform<glm::vec3> &viewZDirectionUniform();

    PUniform<graphics::PConstTexture> &cameraDepthStencilTextureUniform();

    PUniform<graphics::PConstTexture> &BRDFLutTextureUniform();

    PUniform<graphics::PConstTexture> &GBufferColor0TextureUniform();
    PUniform<graphics::PConstTexture> &GBufferColor1TextureUniform();

    graphics::PConstBufferRange &OITDataBuffer();
    PUniform<graphics::PConstBufferRange> &OITNodesCounterUniform();
    PUniform<graphics::PConstImage> &OITIndicesImageUniform();

private:
    PUniform<glm::mat4x4> m_viewMatrixUniform;
    PUniform<glm::mat4x4> m_viewMatrixInverseUniform;
    PUniform<glm::mat4x4> m_projectionMatrixUniform;
    PUniform<glm::mat4x4> m_projectionMatrixInverseUniform;
    PUniform<glm::mat4x4> m_viewProjectionMatrixUniform;
    PUniform<glm::mat4x4> m_viewProjectionMatrixInverseUniform;
    PUniform<glm::vec3> m_viewPositionUniform;
    PUniform<glm::vec3> m_viewXDirectionUniform;
    PUniform<glm::vec3> m_viewYDirectionUniform;
    PUniform<glm::vec3> m_viewZDirectionUniform;

    PUniform<graphics::PConstTexture> m_cameraDepthStencilTextureUniform;

    PUniform<graphics::PConstTexture> m_BRDFLutTextureUniform;

    PUniform<graphics::PConstTexture> m_GBufferColor0TextureUniform;
    PUniform<graphics::PConstTexture> m_GBufferColor1TextureUniform;

    graphics::PConstBufferRange m_OITDataBuffer;
    PUniform<graphics::PConstBufferRange> m_OITNodesCounterUniform;
    PUniform<graphics::PConstImage> m_OITIndicesImageUniform;

};

}
}

#endif // RENDERINFOPRIVATE_H
