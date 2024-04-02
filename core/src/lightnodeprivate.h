#ifndef CORE_LIGHTNODEPRIVATE_H
#define CORE_LIGHTNODEPRIVATE_H

#include <core/forwarddecl.h>

#include "nodeprivate.h"
#include "framebufferhelpers.h"

namespace simplex
{
namespace core
{

class LightNodePrivate : public NodePrivate
{
public:
    LightNodePrivate(const std::string &name);
    ~LightNodePrivate() override;

    bool &isLightingEnabled();
    LightShadingMode &shadingMode();

    glm::uvec2 &shadowMapSize();
    utils::Range &shadowCullPlanesLimits();

    std::shared_ptr<LightDrawable> &areaDrawable();

    bool &isAreaMatrixDirty();
    glm::mat4x4 &areaMatrix();

    bool &isAreaBoundingBoxDirty();
    utils::BoundingBox &areaBoundingBox();

    bool &isShadowFrameBufferDirty();
    std::shared_ptr<ShadowFrameBuffer> &shadowFrameBuffer();

    graphics::PBufferRange &layeredShadowMatricesBuffer();

    virtual std::shared_ptr<ShadowFrameBuffer> createShadowFrameBuffer(const std::shared_ptr<graphics::IRenderer>&) const = 0;
    virtual const glm::mat4x4 &shadowBiasMatrix() const = 0;

private:
    bool m_isLightingEnabled;
    LightShadingMode m_shadingMode;

    glm::uvec2 m_shadowMapSize;
    utils::Range m_shadowCullPlanesLimits;

    std::shared_ptr<LightDrawable> m_areaDrawable;

    bool m_isAreaMatrixDirty;
    glm::mat4x4 m_areaMatrix;

    bool m_isAreaBoundingBoxDirty;
    utils::BoundingBox m_areaBoundingBox;

    bool m_isShadowFrameBufferDirty;
    std::shared_ptr<ShadowFrameBuffer> m_shadowFrameBuffer;

    graphics::PBufferRange m_layeredShadowMatricesBuffer;

};

}
}

#endif // CORE_LIGHTNODEPRIVATE_H
