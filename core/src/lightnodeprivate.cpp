#include "lightnodeprivate.h"
#include "shadowprivate.h"

namespace simplex
{
namespace core
{

LightNodePrivate::LightNodePrivate(LightNode &lightNode, const std::string &name, LightType type)
    : NodePrivate(lightNode, name)
    , m_type(type)
    , m_isLightingEnabled(true)
    , m_shadow(std::make_shared<Shadow>(type))
    , m_shadowTransform()
    , m_isAreaMatrixDirty(true)
    , m_isAreaBoundingBoxDirty(true)
{
}

LightNodePrivate::~LightNodePrivate() = default;

utils::BoundingBox LightNodePrivate::doBoundingBox()
{
    return areaBoundingBox();
}

LightType& LightNodePrivate::type()
{
    return m_type;
}

uint32_t LightNodePrivate::numLayeredShadowMatrices() const
{
    return numLayeredShadowMatrices(m_type);
}

uint32_t LightNodePrivate::numLayeredShadowMatrices(LightType type)
{
    uint32_t result = 0u;
    switch (type)
    {
    case LightType::Point:
    {
        result = 6u;
        break;
    }
    case LightType::Directional:
    case LightType::Spot:
    {
        result = 1u;
        break;
    }
    default:
    {
        break;
    }
    }
    return result;
}

bool &LightNodePrivate::isLightingEnabled()
{
    return m_isLightingEnabled;
}

Shadow &LightNodePrivate::shadow()
{
    return *m_shadow;
}

LightNodePrivate::ShadowTransform &LightNodePrivate::shadowTransform(const utils::Frustum::Points &cameraFrustumPoints)
{
    m_shadowTransform = doShadowTransform(cameraFrustumPoints);
    return m_shadowTransform;
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
