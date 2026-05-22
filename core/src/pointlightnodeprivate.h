#ifndef CORE_POINTLIGHTNODEPRIVATE_H
#define CORE_POINTLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class PointLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(PointLightNode)

public:
    PointLightNodePrivate(PointLightNode&, const std::string&);
    ~PointLightNodePrivate() override;

    uint32_t shadowLayersCount() const override;

    glm::vec3& color();
    glm::vec2& radiuses();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(LightHandler&) const override;

    glm::vec3 m_color;
    glm::vec2 m_radiuses;
};

} // namespace core
} // namespace simplex

#endif // CORE_POINTLIGHTNODEPRIVATE_H
