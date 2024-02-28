#include "standarddrawableprivate.h"

namespace simplex
{
namespace core
{

glm::u32vec4 StandardDrawablePrivate::s_defaultORMSwizzleMask = glm::u32vec4(0u);
glm::vec4 StandardDrawablePrivate::s_defaultBaseColor = glm::vec4(1.f);
float StandardDrawablePrivate::s_defaultMetalness = 1.f;
float StandardDrawablePrivate::s_defaultRoughness = 1.f;
float StandardDrawablePrivate::s_defaultNormalMapScale = 1.f;
float StandardDrawablePrivate::s_defaultOcclusionMapStrength = 1.f;

StandardDrawablePrivate::StandardDrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao, const utils::BoundingBox &bb)
    : DrawablePrivate(vao)
    , m_boundingBox(bb)
{
}

utils::BoundingBox &StandardDrawablePrivate::boundingBox()
{
    return m_boundingBox;
}

glm::u32vec4 &StandardDrawablePrivate::defaultORMSwizzleMask()
{
    return s_defaultORMSwizzleMask;
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

float &StandardDrawablePrivate::defaultNormalMapScale()
{
    return s_defaultNormalMapScale;
}

float &StandardDrawablePrivate::defaultOcclusionMapStrength()
{
    return s_defaultOcclusionMapStrength;
}

}
}
