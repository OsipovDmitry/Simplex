#include "spotlightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> SpotLightNodePrivate::s_lightAreaVertexArray;

SpotLightNodePrivate::SpotLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

SpotLightNodePrivate::~SpotLightNodePrivate() = default;

std::weak_ptr<graphics::IVertexArray> &SpotLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

}
}
