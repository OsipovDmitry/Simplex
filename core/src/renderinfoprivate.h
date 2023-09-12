#ifndef RENDERINFOPRIVATE_H
#define RENDERINFOPRIVATE_H

#include <memory>

#include <utils/glm/vec3.hpp>
#include <utils/glm/mat4x4.hpp>

#include <core/uniform.h>

namespace simplex
{
namespace core
{

class RenderInfoPrivate
{
public:
    RenderInfoPrivate() {}

    PUniform<glm::mat4x4> &viewMatrixUniform() { return m_viewMatrixUniform; }
    PUniform<glm::mat4x4> &viewMatrixInverseUniform() { return m_viewMatrixInverseUniform; }
    PUniform<glm::mat4x4> &projectionMatrixUniform() { return m_projectionMatrixUniform; }
    PUniform<glm::mat4x4> &projectionMatrixInverseUniform() { return m_projectionMatrixInverseUniform; }
    PUniform<glm::mat4x4> &viewProjectionMatrixUniform() { return m_viewProjectionMatrixUniform; }
    PUniform<glm::mat4x4> &viewProjectionMatrixInverseUniform() { return m_viewProjectionMatrixInverseUniform; }
    PUniform<glm::vec3> &viewPositionUniform() { return m_viewPositionUniform; }
    PUniform<glm::vec3> &viewXDirectionUniform() { return m_viewXDirectionUniform; }
    PUniform<glm::vec3> &viewYDirectionUniform() { return m_viewYDirectionUniform; }
    PUniform<glm::vec3> &viewZDirectionUniform() { return m_viewZDirectionUniform; }

    PUniform<graphics::PTexture> &gBufferColor0MapUniform() { return m_gBufferColor0MapUniform; }
    PUniform<graphics::PTexture> &gBufferColor1MapUniform() { return m_gBufferColor1MapUniform; }
    PUniform<graphics::PTexture> &gBufferDepthMapUniform() { return m_gBufferDepthMapUniform; }

    graphics::PBufferRange &OITDataBuffer() { return m_OITDataBuffer; }
    PUniform<graphics::PBufferRange> &OITNodesCounterUniform() { return m_OITNodesCounterUniform; }
    PUniform<graphics::PImage> &OITIndicesImageUniform() { return m_OITIndicesImageUniform; }

    bool &faceCulling() { return m_faceCulling; }
    graphics::FaceType &cullFaceType() { return m_cullFaceType; }

    std::array<bool, graphics::FrameBufferColorAttachmentsCount()> & colorMasks() { return m_colorMasks; }

    bool &depthTest() { return m_depthTest; }
    graphics::DepthFunc &depthFunc() { return m_depthFunc; }
    bool &depthMask() { return m_depthMask; }

    bool &stencilTest() { return m_stencilTest; }

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

    PUniform<graphics::PTexture> m_gBufferColor0MapUniform;
    PUniform<graphics::PTexture> m_gBufferColor1MapUniform;
    PUniform<graphics::PTexture> m_gBufferDepthMapUniform;

    graphics::PBufferRange m_OITDataBuffer;
    PUniform<graphics::PBufferRange> m_OITNodesCounterUniform;
    PUniform<graphics::PImage> m_OITIndicesImageUniform;

    bool m_faceCulling;
    graphics::FaceType m_cullFaceType;

    std::array<bool, graphics::FrameBufferColorAttachmentsCount()> m_colorMasks;
    bool m_depthTest;
    graphics::DepthFunc m_depthFunc;
    bool m_depthMask;
    bool m_stencilTest;

};

}
}

#endif // RENDERINFOPRIVATE_H
