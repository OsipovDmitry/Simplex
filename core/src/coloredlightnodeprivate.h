#ifndef CORE_COLOREDLIGHTNODEPRIVATE_H
#define CORE_COLOREDLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class ColoredLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(DirectionalLightNode)

public:
    ColoredLightNodePrivate(ColoredLightNode&, const std::string&, LightType);
    ~ColoredLightNodePrivate() override;

    glm::vec3& color();

protected:
    glm::vec3 m_color;
};

} // namespace core
} // namespace simplex

#endif // CORE_COLOREDLIGHTNODEPRIVATE_H
