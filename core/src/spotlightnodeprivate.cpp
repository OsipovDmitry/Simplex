#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> SpotLightNodePrivate::s_lightAreaVertexArray;
utils::BoundingBox SpotLightNodePrivate::s_lightAreaBoundingBox;

SpotLightNodePrivate::SpotLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

SpotLightNodePrivate::~SpotLightNodePrivate() = default;

glm::vec2 &SpotLightNodePrivate::halfAngles()
{
    return m_halfAngles;
}

std::weak_ptr<graphics::IVertexArray> &SpotLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

utils::BoundingBox &SpotLightNodePrivate::lightAreaBoundingBox()
{
    return s_lightAreaBoundingBox;
}

}
}
