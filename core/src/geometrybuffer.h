#ifndef CORE_GEOMETRYBUFFER_H
#define CORE_GEOMETRYBUFFER_H

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

struct OITNode
{
    uint32_t packedBaseColor;
    uint32_t packedFinalColor;
    uint32_t packedORMAlpha;
    uint32_t packedNormalFlags;
    float depth;
    uint32_t next;
};

struct OITBufferReservedData
{
    uint32_t maxNumOITNodes;
    uint32_t numOITNodes;
};

using POITNodesBuffer = std::shared_ptr<graphics::DynamicBufferT<OITNode, OITBufferReservedData>>;
using PConstOITNodesBuffer = std::shared_ptr<const graphics::DynamicBufferT<OITNode, OITBufferReservedData>>;

class GeometryBuffer
{
public:
    GeometryBuffer(const glm::uvec2&);
    ~GeometryBuffer() = default;

    const glm::uvec2& size() const;
    void resize(const glm::uvec2&);

    void clearOITNodesBuffer();

    graphics::PConstTexture colorTexture0() const;
    graphics::PConstTexture colorTexture1() const;
    graphics::PConstTexture colorTexture2() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture stencilTexture() const;
    graphics::PConstTexture OITDepthTexture() const;
    graphics::PConstTexture OITIndicesTexture() const;
    graphics::PConstTexture finalTexture() const;
    PConstOITNodesBuffer OITNodesBuffer() const;

private:
    glm::uvec2 m_size;

    graphics::PTexture m_colorTexture0;
    graphics::PTexture m_colorTexture1;
    graphics::PTexture m_colorTexture2;
    graphics::PTexture m_depthTexture;
    graphics::PTexture m_stencilTexture;
    graphics::PTexture m_OITDepthTexture;
    graphics::PTexture m_OITIndicesTexture;
    graphics::PTexture m_finalTexture;
    POITNodesBuffer m_OITNodesBuffer;
};

}
}

#endif // CORE_GEOMETRYBUFFER_H
