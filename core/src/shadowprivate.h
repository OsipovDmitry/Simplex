#ifndef CORE_SHADOWPRIVATE_H
#define CORE_SHADOWPRIVATE_H

#include <vector>

#include <utils/glm/vec2.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/forwarddecl.h>
#include <utils/range.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ShadowFrameBuffer;
class Blur;

class ShadowPrivate
{
public:
    ShadowPrivate();
    virtual ~ShadowPrivate();

    ShadingMode &mode();
    ShadingFilter &filter();
    float &depthBias();
    glm::uvec2 &mapSize();
    utils::Range &cullPlanesLimits();

    std::shared_ptr<ShadowFrameBuffer> &frameBuffer();
    std::shared_ptr<Blur> &blur();

    graphics::PBuffer &layeredMatricesBuffer();

    virtual std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::RendererBase>&) const = 0;
    void update(const std::shared_ptr<graphics::RendererBase>&, const std::vector<glm::mat4x4>&);

private:
    ShadingMode m_mode;
    ShadingFilter m_filter;
    float m_depthBias;
    glm::uvec2 m_mapSize;
    utils::Range m_cullPlanesLimits;

    std::shared_ptr<ShadowFrameBuffer> m_frameBuffer;
    std::shared_ptr<Blur> m_blur;

    graphics::PBuffer m_layeredMatricesBuffer;

};

}
}

#endif // CORE_SHADOWPRIVATE_H
