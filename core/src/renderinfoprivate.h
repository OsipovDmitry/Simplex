#ifndef RENDERINFOPRIVATE_H
#define RENDERINFOPRIVATE_H

#include <memory>

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>

#include <core/igraphicsrenderer.h>

namespace simplex
{
namespace core
{

class RenderInfoPrivate
{
public:
    RenderInfoPrivate() {}

    std::shared_ptr<graphics::IFrameBuffer> &frameBuffer() { return m_frameBuffer; }
    glm::uvec4 &viewport() { return m_viewport; }

    glm::mat4 &viewMatrix() { return m_viewMatrix; }
    glm::mat4 &viewMatrixInverse() { return m_viewMatrixInverse; }
    glm::mat4 &projectionMatrix() { return m_projectionMatrix; }
    glm::mat4 &projectionMatrixInverse() { return m_projectionMatrixInverse; }
    glm::mat4 &viewProjectionMatrix() { return m_viewProjectionMatrix; }
    glm::mat4 &viewProjectionMatrixInverse() { return m_viewProjectionMatrixInverse; }
    glm::vec3 &viewPosition() { return m_viewPosition; }
    glm::vec3 &viewXDirection() { return m_viewXDirection; }
    glm::vec3 &viewYDirection() { return m_viewYDirection; }
    glm::vec3 &viewZDirection() { return m_viewZDirection; }

    std::shared_ptr<const graphics::ITexture> &gBufferColor0Map() { return m_gBufferColor0Map; }
    std::shared_ptr<const graphics::ITexture> &gBufferColor1Map() { return m_gBufferColor1Map; }
    std::shared_ptr<const graphics::ITexture> &gBufferDepthMap() { return m_gBufferDepthMap; }

    bool &faceCulling() { return m_faceCulling; }
    graphics::FaceType &cullFaceType() { return m_cullFaceType; }

    std::array<bool, graphics::FrameBufferColorAttachmentsCount()> & colorMaks() { return m_colorMasks; }
    bool &depthTest() { return m_depthTest; }
    graphics::DepthFunc &depthFunc() { return m_depthFunc; }
    bool &depthMask() { return m_depthMask; }
    bool &stencilTest() { return m_stencilTest; }

private:
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    glm::uvec4 m_viewport;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_viewMatrixInverse;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_projectionMatrixInverse;
    glm::mat4 m_viewProjectionMatrix;
    glm::mat4 m_viewProjectionMatrixInverse;
    glm::vec3 m_viewPosition;
    glm::vec3 m_viewXDirection;
    glm::vec3 m_viewYDirection;
    glm::vec3 m_viewZDirection;

    std::shared_ptr<const graphics::ITexture> m_gBufferColor0Map;
    std::shared_ptr<const graphics::ITexture> m_gBufferColor1Map;
    std::shared_ptr<const graphics::ITexture> m_gBufferDepthMap;

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
