#ifndef CORE_GEOMETRYBUFFER_H
#define CORE_GEOMETRYBUFFER_H

#include <core/stateset.h>

#include "descriptions.h"

namespace simplex
{
namespace core
{

using POITBuffer = std::shared_ptr<graphics::VectorBuffer<OITNodeDescription>>;
using PConstOITBuffer = std::shared_ptr<const graphics::VectorBuffer<OITNodeDescription>>;

using PGBuffer = std::shared_ptr<graphics::StructBuffer<GBufferDescription>>;
using PConstGBuffer = std::shared_ptr<const graphics::StructBuffer<GBufferDescription>>;

class GeometryBuffer : public StateSet, public std::enable_shared_from_this<GeometryBuffer>
{
public:
    GeometryBuffer(const glm::uvec2&);
    ~GeometryBuffer() = default;

    const glm::uvec2& size() const;
    void resize(const glm::uvec2&, const std::shared_ptr<graphics::RendererBase>&);

    void initialize(const std::shared_ptr<ProgramsManager>&);
    void clear(
        const std::shared_ptr<graphics::RendererBase>&,
        const std::shared_ptr<graphics::IFrameBuffer>&) const;
    void sortOITNodes(const std::shared_ptr<graphics::RendererBase>&) const;

    PConstGBuffer GBuffer() const;
    PConstOITBuffer OITBuffer() const;

    graphics::PConstTexture colorTexture() const;
    graphics::PConstTexture depthTexture() const;
    graphics::PConstTexture finalTexture() const;

private:
    glm::uvec2 m_size;

    PGBuffer m_GBuffer;
    POITBuffer m_OITBuffer;

    graphics::PTextureHandle m_colorTextureHandle;
    graphics::PTextureHandle m_depthTextureHandle;
    graphics::PImageHandle m_OITNodeIDImageHandle;
    graphics::PTextureHandle m_finalTextureHandle;

    bool m_isInitialized;
    std::shared_ptr<graphics::IComputeProgram> m_clearOITNodeIDImageProgram;
    std::shared_ptr<graphics::IComputeProgram> m_sortOITNodesProgram;
};

}
}

#endif // CORE_GEOMETRYBUFFER_H
