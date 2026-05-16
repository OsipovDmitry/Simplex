#ifndef CORE_DIRECTIONALLIGHTNODEPRIVATE_H
#define CORE_DIRECTIONALLIGHTNODEPRIVATE_H

#include "lightnodeprivate.h"

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate : public LightNodePrivate
{
    PUBLIC_IMPL(DirectionalLightNode)

public:
    DirectionalLightNodePrivate(DirectionalLightNode&, const std::string&);
    ~DirectionalLightNodePrivate() override;

    uint32_t shadowLayersCount() const override;

    glm::vec3& color();
    uint32_t& shadowCascadesCount();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(SceneData&, utils::IDsGeneratorT<size_t>::value_type) const override;

    glm::vec3 m_color;
    uint32_t m_shadowCascadesCount;
};

} // namespace core
} // namespace simplex

#endif // CORE_DIRECTIONALLIGHTNODEPRIVATE_H
