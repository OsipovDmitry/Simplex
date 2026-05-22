#ifndef CORE_SPOTLIGHTNODEPRIVATE_H
#define CORE_SPOTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class SpotLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(SpotLightNode)

public:
    SpotLightNodePrivate(SpotLightNode&, const std::string&);
    ~SpotLightNodePrivate() override;

    uint32_t shadowLayersCount() const override;

    glm::vec3& color();
    glm::vec2& radiuses();
    glm::vec2& halfAngles();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(LightHandler&) const override;

    glm::vec3 m_color;
    glm::vec2 m_radiuses;
    glm::vec2 m_halfAngles;
};

} // namespace core
} // namespace simplex

#endif // CORE_SPOTLIGHTNODEPRIVATE_H
