#include "pbrdrawableprivate.h".h"

namespace simplex
{
namespace core
{

glm::u32vec4 PBRDrawablePrivate::s_defaultORMSwizzleMask = glm::u32vec4(0u);
glm::vec4 PBRDrawablePrivate::s_defaultBaseColor = glm::vec4(1.f);
float PBRDrawablePrivate::s_defaultMetalness = 1.f;
float PBRDrawablePrivate::s_defaultRoughness = 1.f;
float PBRDrawablePrivate::s_defaultNormalMapScale = 1.f;
float PBRDrawablePrivate::s_defaultOcclusionMapStrength = 1.f;

PBRDrawablePrivate::PBRDrawablePrivate(const std::shared_ptr<graphics::IVertexArray> &vao, const utils::BoundingBox &bb)
    : DrawablePrivate(vao)
    , m_boundingBox(bb)
{
}

utils::BoundingBox &PBRDrawablePrivate::boundingBox()
{
    return m_boundingBox;
}

glm::u32vec4 &PBRDrawablePrivate::defaultORMSwizzleMask()
{
    return s_defaultORMSwizzleMask;
}

glm::vec4 &PBRDrawablePrivate::defaultBaseColor()
{
    return s_defaultBaseColor;
}

float &PBRDrawablePrivate::defaultMetalness()
{
    return s_defaultMetalness;
}

float &PBRDrawablePrivate::defaultRoughness()
{
    return s_defaultRoughness;
}

float &PBRDrawablePrivate::defaultNormalMapScale()
{
    return s_defaultNormalMapScale;
}

float &PBRDrawablePrivate::defaultOcclusionMapStrength()
{
    return s_defaultOcclusionMapStrength;
}

}
}
