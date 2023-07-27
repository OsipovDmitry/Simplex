#ifndef CORE_RENDERSURFACEPRIVATE_H
#define CORE_RENDERSURFACEPRIVATE_H

#include <memory>

#include <utils/glm/vec2.hpp>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class RenderSurfacePrivate
{
public:
    RenderSurfacePrivate(std::weak_ptr<graphics::IRenderer>, std::shared_ptr<graphics::IFrameBuffer>);

    std::weak_ptr<graphics::IRenderer> &renderer();
    std::shared_ptr<graphics::IFrameBuffer> &frameBuffer();
    glm::uvec2 &size();
    glm::uvec2 &fixedSize();
    float &sizeScale();
    bool &isSizeFixed();

private:
    std::weak_ptr<graphics::IRenderer> m_renderer;
    std::shared_ptr<graphics::IFrameBuffer> m_frameBuffer;
    glm::uvec2 m_size;
    glm::uvec2 m_fixedSize;
    float m_sizeScale;
    bool m_isSizeFixed;

};

}
}

#endif // CORE_RENDERSURFACEPRIVATE_H
