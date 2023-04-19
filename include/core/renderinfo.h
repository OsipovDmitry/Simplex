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
    RenderInfo();
    ~RenderInfo();

    const glm::uvec2 &viewport() const;
    void setViewport(const glm::uvec2&);

    const glm::mat4 &viewMatrix() const;
    void setViewMatrix(const glm::mat4&);

    const glm::mat4 &projectionMatrix() const;
    void setProjectionMatrix(const glm::mat4&);


private:
    std::unique_ptr<RenderInfoPrivate> m_;
};

}
}

#endif // CORE_RENDERINFO_H
