#include <core/lightnode.h>

#include "lightnodeprivate.h"
#include "shadowprivate.h"

namespace simplex
{
namespace core
{

LightNodePrivate::LightNodePrivate(LightNode &lightNode, const std::string &name, std::unique_ptr<ShadowPrivate> shadowPrivate)
    : NodePrivate(lightNode, name)
    , m_shadow(std::move(shadowPrivate))
    , m_isLightingEnabled(true)
    , m_shadowTransform()
    , m_areaDrawable()
    , m_isAreaMatrixDirty(true)
    , m_isAreaBoundingBoxDirty(true)
{
}

LightNodePrivate::~LightNodePrivate() = default;

utils::BoundingBox LightNodePrivate::doBoundingBox()
{
    return areaBoundingBox();
}

bool &LightNodePrivate::isLightingEnabled()
{
    return m_isLightingEnabled;
}

Shadow &LightNodePrivate::shadow()
{
    return m_shadow;
}

LightNodePrivate::ShadowTransform &LightNodePrivate::shadowTransform(const utils::Frustum::Points &cameraFrustumPoints)
{
    m_shadowTransform = doShadowTransform(cameraFrustumPoints);
    return m_shadowTransform;
}

std::shared_ptr<LightDrawable> &LightNodePrivate::areaDrawable()
{
    return m_areaDrawable;
}

const glm::mat4x4 &LightNodePrivate::areaMatrix()
{
    if (m_isAreaMatrixDirty)
    {
        m_areaMatrix = doAreaMatrix();
        m_isAreaMatrixDirty = false;
    }
    return m_areaMatrix;
}

const utils::BoundingBox &LightNodePrivate::areaBoundingBox()
{
    if (m_isAreaBoundingBoxDirty)
    {
        m_areaBoundingBox = doAreaBoundingBox();
        m_isAreaBoundingBoxDirty = false;
    }
    return m_areaBoundingBox;
}

void LightNodePrivate::dirtyAreaMatrix()
{
    m_isAreaMatrixDirty = true;
}

void LightNodePrivate::dirtyAreaBoundingBox()
{
    m_isAreaBoundingBoxDirty = true;
    dirtyBoundingBox();
}

LightNodePrivate::ShadowTransform LightNodePrivate::doShadowTransform(const utils::Frustum::Points&)
{
    return ShadowTransform();
}

}
}
