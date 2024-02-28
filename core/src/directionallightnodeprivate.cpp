#include "directionallightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> DirectionalLightNodePrivate::s_lightAreaVertexArray;
utils::BoundingBox DirectionalLightNodePrivate::s_lightAreaBoundingBox;

DirectionalLightNodePrivate::DirectionalLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

DirectionalLightNodePrivate::~DirectionalLightNodePrivate() = default;

std::weak_ptr<graphics::IVertexArray> &DirectionalLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

utils::BoundingBox &DirectionalLightNodePrivate::lightAreaBoundingBox()
{
    return s_lightAreaBoundingBox;
}

}
}
