#include "standarddrawableprivate.h"

namespace simplex
{
namespace core
{

glm::vec4 StandardDrawablePrivate::s_defaultBaseColor = glm::vec4(1.f);
float StandardDrawablePrivate::s_defaultMetalness = 1.f;
float StandardDrawablePrivate::s_defaultRoughness = 1.f;

StandardDrawablePrivate::StandardDrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao)
    : DrawablePrivate(vao)
{
}

glm::vec4 &StandardDrawablePrivate::defaultBaseColor()
{
    return s_defaultBaseColor;
}

float &StandardDrawablePrivate::defaultMetalness()
{
    return s_defaultMetalness;
}

float &StandardDrawablePrivate::defaultRoughness()
{
    return s_defaultRoughness;
}

}
}
