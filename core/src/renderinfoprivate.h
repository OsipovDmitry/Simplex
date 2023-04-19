#ifndef RENDERINFOPRIVATE_H
#define RENDERINFOPRIVATE_H

#include <utils/glm/vec2.hpp>
#include <utils/glm/mat4x4.hpp>

namespace simplex
{
namespace core
{

class RenderInfoPrivate
{
public:
    RenderInfoPrivate() {}

    glm::uvec2 &viewport() { return m_viewport; }
    glm::mat4 &viewMatrix() { return m_viewMatrix; }
    glm::mat4 &projectionMatrix() { return m_projectionMatrix; }


private:
    glm::uvec2 m_viewport;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_projectionMatrix;

};

}
}

#endif // RENDERINFOPRIVATE_H
