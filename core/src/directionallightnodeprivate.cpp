#include "directionallightnodeprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> DirectionalLightNodePrivate::s_lightAreaVertexArray;

DirectionalLightNodePrivate::DirectionalLightNodePrivate(const std::string &name)
    : LightNodePrivate(name)
{
}

DirectionalLightNodePrivate::~DirectionalLightNodePrivate() = default;

std::weak_ptr<graphics::IVertexArray> &DirectionalLightNodePrivate::lightAreaVertexArray()
{
    return s_lightAreaVertexArray;
}

}
}
