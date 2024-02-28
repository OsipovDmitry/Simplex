#include "backgrounddrawableprivate.h"

namespace simplex
{
namespace core
{

std::weak_ptr<graphics::IVertexArray> BackgroundDrawablePrivate::s_screenQuadVertexArray;
std::weak_ptr<const graphics::ITexture> BackgroundDrawablePrivate::s_defaultColorMap;
glm::vec3 BackgroundDrawablePrivate::s_defaultBaseColor(1.f);
float BackgroundDrawablePrivate::s_defaultRoughness(1.f);

BackgroundDrawablePrivate::BackgroundDrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao)
    : DrawablePrivate(vao)
{
}

std::weak_ptr<graphics::IVertexArray> &BackgroundDrawablePrivate::screenQuadVertexArray()
{
    return s_screenQuadVertexArray;
}

BackgroundDrawablePrivate::~BackgroundDrawablePrivate() = default;

std::weak_ptr<const graphics::ITexture> &BackgroundDrawablePrivate::defaultColorMap()
{
    return s_defaultColorMap;
}

glm::vec3 &BackgroundDrawablePrivate::defaultBaseColor()
{
    return s_defaultBaseColor;
}

float &BackgroundDrawablePrivate::defaultRoughness()
{
    return s_defaultRoughness;
}

}
}
