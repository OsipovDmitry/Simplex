#ifndef CORE_GFRAMEBUFFER_H
#define CORE_GFRAMEBUFFER_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class GFramebuffer
{
public:
    GFramebuffer(const glm::uvec2&);
    ~GFramebuffer() = default;

    const glm::uvec2& size() const;
    void resize(const glm::uvec2&);

    graphics::PConstTexture colorTexture0() const;
    graphics::PConstTexture colorTexture1() const;
    graphics::PConstTexture colorTexture2() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture stencilTexture() const;
    graphics::PConstTexture OITDepthTexture() const;
    graphics::PConstTexture OITIndicesTexture() const;

private:
    glm::uvec2 m_size;

    graphics::PTexture m_colorTexture0;
    graphics::PTexture m_colorTexture1;
    graphics::PTexture m_colorTexture2;
    graphics::PTexture m_depthTexture;
    graphics::PTexture m_stencilTexture;
    graphics::PTexture m_OITDepthTexture;
    graphics::PTexture m_OITIndicesTexture;
};

}
}

#endif // CORE_GFRAMEBUFFER_H
