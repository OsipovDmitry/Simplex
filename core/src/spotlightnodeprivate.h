#ifndef CORE_SPOTLIGHTNODEPRIVATE_H
#define CORE_SPOTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class SpotLightNodePrivate : public LightNodePrivate
{
public:
    SpotLightNodePrivate(const std::string &name);
    ~SpotLightNodePrivate() override;

    glm::vec2 &radiuses();
    glm::vec2 &halfAngles();

private:
    glm::vec2 m_radiuses;
    glm::vec2 m_halfAngles;
};

}
}

#endif // CORE_SPOTLIGHTNODEPRIVATE_H
