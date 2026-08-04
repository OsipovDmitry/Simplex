#ifndef CORE_GEOMETRYBUFFER_H
#define CORE_GEOMETRYBUFFER_H

#include <optional>

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
    GeometryBuffer();
    ~GeometryBuffer() override;

    void initialize(const std::shared_ptr<ProgramsLoader>&);

    const std::optional<glm::uvec2>& fixedSize() const;
    void setFixedSize(const std::optional<glm::uvec2>&);

    const glm::uvec2& resize(const glm::uvec2&, const std::shared_ptr<graphics::RendererBase>&); // returns actual viewport size

    uint32_t maxOITNodesCount() const;
    uint32_t OITNodesCountPerPixel() const;
    void setOITNodesCount(
        uint32_t maxOITNodesCount,
        uint32_t OITNodesCountPerPixel,
        const std::shared_ptr<graphics::RendererBase>&);

    void clear(const std::shared_ptr<graphics::RendererBase>&, const std::shared_ptr<graphics::IFrameBuffer>&) const;
    void sortOITNodes(const std::shared_ptr<graphics::RendererBase>&) const;

    PConstGBuffer GBuffer() const;
    PConstOITBuffer OITBuffer() const;

    graphics::PConstTexture colorTexture() const;
    graphics::PConstTexture depthTexture() const;

private:
    void recreateBuffers(const std::shared_ptr<graphics::RendererBase>&);

    std::optional<glm::uvec2> m_fixedSize;

    glm::uvec2 m_size = glm::uvec2(0u);
    uint32_t m_maxOITNodesCount = 0u;
    uint32_t m_OITNodesCountPerPixel = 0u;

    bool m_isInitialized = false;

    PGBuffer m_GBuffer;
    POITBuffer m_OITBuffer;

    graphics::PTextureHandle m_colorTextureHandle;
    graphics::PTextureHandle m_depthTextureHandle;
    graphics::PImageHandle m_OITNodeIDImageHandle;

    std::shared_ptr<graphics::IComputeProgram> m_clearOITNodeIDImageProgram;
    std::shared_ptr<graphics::IComputeProgram> m_sortOITNodesProgram;
};

} // namespace core
} // namespace simplex

#endif // CORE_GEOMETRYBUFFER_H
