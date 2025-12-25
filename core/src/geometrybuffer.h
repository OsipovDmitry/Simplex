#ifndef CORE_GEOMETRYBUFFER_H
#define CORE_GEOMETRYBUFFER_H

#include <core/stateset.h>

namespace simplex
{
namespace core
{

struct OITBufferNode
{
    glm::u32vec4 packedPBRData;
    float depth;
    uint32_t next;
};

using POITBuffer = std::shared_ptr<graphics::VectorBuffer<OITBufferNode>>;
using PConstOITBuffer = std::shared_ptr<const graphics::VectorBuffer<OITBufferNode>>;

class GeometryBuffer : public StateSet
{
public:
    GeometryBuffer(const glm::uvec2&);
    ~GeometryBuffer() = default;

    const glm::uvec2& size() const;
    void resize(const glm::uvec2&);

    graphics::PConstTexture colorTexture0() const;
    graphics::PConstTexture depthStencilTexture() const;
    graphics::PConstTexture OITDepthTexture() const;
    graphics::PConstTexture OITIndicesTexture() const;
    graphics::PConstTexture finalTexture() const;
    PConstOITBuffer OITBuffer() const;

private:
    glm::uvec2 m_size;

    graphics::PTexture m_colorTexture0;
    graphics::PTexture m_depthStencilTexture;
    graphics::PTexture m_OITDepthTexture;
    graphics::PTexture m_OITIndicesTexture;
    graphics::PTexture m_finalTexture;
    POITBuffer m_OITBuffer;
};

}
}

#endif // CORE_GEOMETRYBUFFER_H
