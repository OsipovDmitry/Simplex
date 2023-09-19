#include "pointlightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> PointLightNodePrivate::s_lightAreaVertexArray;

PointLightNodePrivate::PointLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

PointLightNodePrivate::~PointLightNodePrivate() = default;

std::weak_ptr<graphics::IVertexArray> &PointLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

}
}
