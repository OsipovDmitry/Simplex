#ifndef RENDERINFOPRIVATE_H
#define RENDERINFOPRIVATE_H

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

    std::shared_ptr<IGraphicsRenderer::FrameBuffer> &frameBuffer() { return m_frameBuffer; }
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

private:
    std::shared_ptr<IGraphicsRenderer::FrameBuffer> m_frameBuffer;
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

};

}
}

#endif // RENDERINFOPRIVATE_H
