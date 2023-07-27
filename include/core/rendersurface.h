#ifndef CORE_RENDERSURFACE_H
#define CORE_RENDERSURFACE_H

#include <memory>

#include <utils/noncopyble.h>
#include <utils/glm/vec2.hpp>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class RenderSurfacePrivate;
class CORE_SHARED_EXPORT RenderSurface
{
    NONCOPYBLE(RenderSurface)
public:
    virtual ~RenderSurface();

    std::weak_ptr<graphics::IRenderer> renderer();

    void resize(const glm::uvec2&);
    const glm::uvec2 &viewportSize() const;

    bool isSizeFixed() const;
    float sizeScale() const;
    const glm::uvec2 &fixedSize() const;

    void setFixedSize(const glm::vec2&);
    void setFloatingSize(float = 1.f);

    std::shared_ptr<graphics::IFrameBuffer> frameBuffer();
    std::shared_ptr<const graphics::IFrameBuffer> frameBuffer() const;

protected:
    RenderSurface(std::weak_ptr<graphics::IRenderer>, std::shared_ptr<graphics::IFrameBuffer>);
    RenderSurface(std::unique_ptr<RenderSurfacePrivate>);

    virtual void doResize(const glm::uvec2&) {}

    std::unique_ptr<RenderSurfacePrivate> m_;
};

}
}

#endif // CORE_RENDERSURFACE_H
