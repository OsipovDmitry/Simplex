#ifndef CORE_RENDERINFO_H
#define CORE_RENDERINFO_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/glm/vec2.hpp>
#include <utils/glm/mat4x4.hpp>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

class RenderInfoPrivate;
class CORE_SHARED_EXPORT RenderInfo
{
    NONCOPYBLE(RenderInfo)
public:
    RenderInfo(const glm::uvec2 &viewport = glm::uvec2(1u, 1u),
               const glm::mat4 &viewMatrix = glm::mat4(1.f),
               const glm::mat4 &projectionMatrix = glm::mat4(1.f));
    ~RenderInfo();

    const glm::uvec2 &viewport() const;
    void setViewport(const glm::uvec2&);

    const glm::mat4 &viewMatrix() const;
    const glm::mat4 &viewMatrixInverse() const;
    const glm::vec3 &viewPosition() const;
    const glm::vec3 &viewXDirection() const;
    const glm::vec3 &viewYDirection() const;
    const glm::vec3 &viewZDirection() const;
    void setViewMatrix(const glm::mat4&);

    const glm::mat4 &projectionMatrix() const;
    const glm::mat4 &projectionMatrixInverse() const;
    void setProjectionMatrix(const glm::mat4&);

    const glm::mat4 &viewProjectionMatrix() const;
    const glm::mat4 &viewProjectionMatrixInverse() const;

private:
    std::unique_ptr<RenderInfoPrivate> m_;
};

}
}

#endif // CORE_RENDERINFO_H
