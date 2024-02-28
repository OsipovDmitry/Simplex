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
{
}

LightNodePrivate::~LightNodePrivate() = default;

bool &LightNodePrivate::isLightingEnabled()
{
    return m_isLightingEnabled;
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

}
}
