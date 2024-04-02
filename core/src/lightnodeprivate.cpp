#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

LightNodePrivate::LightNodePrivate(const std::string &name)
    : NodePrivate(name)
    , m_isLightingEnabled(true)
    , m_areaDrawable()
    , m_isAreaMatrixDirty(true)
    , m_isAreaBoundingBoxDirty(true)
    , m_isShadowFrameBufferDirty(true)
{
}

LightNodePrivate::~LightNodePrivate() = default;

bool &LightNodePrivate::isLightingEnabled()
{
    return m_isLightingEnabled;
}

LightShadingMode &LightNodePrivate::shadingMode()
{
    return m_shadingMode;
}

glm::uvec2 &LightNodePrivate::shadowMapSize()
{
    return m_shadowMapSize;
}

utils::Range &LightNodePrivate::shadowCullPlanesLimits()
{
    return m_shadowCullPlanesLimits;
}

std::shared_ptr<LightDrawable> &LightNodePrivate::areaDrawable()
{
    return m_areaDrawable;
}

bool &LightNodePrivate::isAreaMatrixDirty()
{
    return m_isAreaMatrixDirty;
}

glm::mat4x4 &LightNodePrivate::areaMatrix()
{
    return m_areaMatrix;
}

bool &LightNodePrivate::isAreaBoundingBoxDirty()
{
    return m_isAreaBoundingBoxDirty;
}

utils::BoundingBox &LightNodePrivate::areaBoundingBox()
{
    return m_areaBoundingBox;
}

bool &LightNodePrivate::isShadowFrameBufferDirty()
{
    return m_isShadowFrameBufferDirty;
}

std::shared_ptr<ShadowFrameBuffer> &LightNodePrivate::shadowFrameBuffer()
{
    return m_shadowFrameBuffer;
}

graphics::PBufferRange &LightNodePrivate::layeredShadowMatricesBuffer()
{
    return m_layeredShadowMatricesBuffer;
}

}
}
