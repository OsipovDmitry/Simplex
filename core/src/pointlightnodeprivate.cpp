#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

PointLightNodePrivate::PointLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

PointLightNodePrivate::~PointLightNodePrivate() = default;

glm::vec2 &PointLightNodePrivate::radiuses()
{
    return m_radiuses;
}

}
}
