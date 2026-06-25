#include "coloredlightnodeprivate.h"

#include <core/coloredlightnode.h>

namespace simplex
{
namespace core
{

ColoredLightNodePrivate::ColoredLightNodePrivate(ColoredLightNode& coloredLightNode, const std::string& name, LightType type)
    : LightNodePrivate(coloredLightNode, name, type)
{
}

ColoredLightNodePrivate::~ColoredLightNodePrivate() = default;

glm::vec3& ColoredLightNodePrivate::color()
{
    return m_color;
}

} // namespace core
} // namespace simplex
