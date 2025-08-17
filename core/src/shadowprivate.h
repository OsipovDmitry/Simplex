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

class ShadowBuffer;
class ShadowFrameBuffer;
class Blur;

class ShadowPrivate
{
public:
    ShadowPrivate();
    ~ShadowPrivate();

    ShadingMode &mode();
    ShadingFilter &filter();
    float &depthBias();
    glm::uvec2 &mapSize();
    utils::Range &cullPlanesLimits();

    std::shared_ptr<ShadowBuffer>& shadowBuffer();
    std::shared_ptr<ShadowFrameBuffer> &frameBuffer();
    std::shared_ptr<Blur> &blur();

    struct LayeredMatricesBufferReservedData { uint32_t numLayers; uint32_t padding[3]; };
    using LayeredMatricesBuffer = std::shared_ptr<graphics::DynamicBufferT<glm::mat4x4, LayeredMatricesBufferReservedData>>;
    LayeredMatricesBuffer& layeredMatricesBuffer();

    void update(const std::shared_ptr<graphics::IFrameBuffer>&, const std::vector<glm::mat4x4>&);

private:
    ShadingMode m_mode;
    ShadingFilter m_filter;
    float m_depthBias;
    glm::uvec2 m_mapSize;
    utils::Range m_cullPlanesLimits;

    std::shared_ptr<ShadowBuffer> m_shadowBuffer;
    std::shared_ptr<ShadowFrameBuffer> m_frameBuffer;
    std::shared_ptr<Blur> m_blur;

    LayeredMatricesBuffer m_layeredMatricesBuffer;

};

}
}

#endif // CORE_SHADOWPRIVATE_H
