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

    glm::vec2 &halfAngles();

    static std::weak_ptr<graphics::IVertexArray> &lightAreaVertexArray();
    static utils::BoundingBox &lightAreaBoundingBox();

private:
    glm::vec2 m_halfAngles;

    static std::weak_ptr<graphics::IVertexArray> s_lightAreaVertexArray;
    static utils::BoundingBox s_lightAreaBoundingBox;

};

}
}

#endif // CORE_SPOTLIGHTNODEPRIVATE_H
