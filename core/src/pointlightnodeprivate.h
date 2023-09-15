#ifndef CORE_POINTLIGHTNODEPRIVATE_H
#define CORE_POINTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class PointLightNodePrivate : public LightNodePrivate
{
public:
    PointLightNodePrivate(const std::string &name);
    ~PointLightNodePrivate() override;

    glm::vec2 &radiuses();

private:
    glm::vec2 m_radiuses;
};

}
}

#endif // CORE_POINTLIGHTNODEPRIVATE_H
