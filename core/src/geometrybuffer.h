#ifndef CORE_GEOMETRYBUFFER_H
#define CORE_GEOMETRYBUFFER_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

struct OITBufferReservedData
{
    uint32_t maxNodesCount;
    uint32_t nodesCount;
    uint32_t padding[2u];
};

struct OITBufferNode
{
    glm::u32vec4 packedPBRData;
    float depth;
    uint32_t next;
};

using POITBuffer = std::shared_ptr<graphics::VectorBuffer<OITBufferNode, OITBufferReservedData>>;
using PConstOITBuffer = std::shared_ptr<const graphics::VectorBuffer<OITBufferNode, OITBufferReservedData>>;

class GeometryBuffer
{
public:
    GeometryBuffer(const glm::uvec2&);
    ~GeometryBuffer() = default;

    const glm::uvec2& size() const;
    void resize(const glm::uvec2&);

    void clearOITBuffer();

    graphics::PConstTexture colorTexture0() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture stencilTexture() const;
    graphics::PConstTexture OITDepthTexture() const;
    graphics::PConstTexture OITIndicesTexture() const;
    graphics::PConstTexture finalTexture() const;
    PConstOITBuffer OITBuffer() const;

private:
    glm::uvec2 m_size;

    graphics::PTexture m_colorTexture0;
    graphics::PTexture m_depthTexture;
    graphics::PTexture m_stencilTexture;
    graphics::PTexture m_OITDepthTexture;
    graphics::PTexture m_OITIndicesTexture;
    graphics::PTexture m_finalTexture;
    POITBuffer m_OITBuffer;
};

}
}

#endif // CORE_GEOMETRYBUFFER_H
