#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

SpotLightNodePrivate::SpotLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

SpotLightNodePrivate::~SpotLightNodePrivate() = default;

glm::vec2 &SpotLightNodePrivate::radiuses()
{
    return m_radiuses;
}

glm::vec2 &SpotLightNodePrivate::halfAngles()
{
    return m_halfAngles;
}

}
}
