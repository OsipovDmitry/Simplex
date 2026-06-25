#ifndef CORE_DIRECTIONALLIGHTNODEPRIVATE_H
#define CORE_DIRECTIONALLIGHTNODEPRIVATE_H

#include "shadowedlightnodeprivate.h"

namespace simplex
{
namespace core
{

class DirectionalLightNodePrivate : public ShadowedLightNodePrivate
{
    PUBLIC_IMPL(DirectionalLightNode)

public:
    DirectionalLightNodePrivate(DirectionalLightNode&, const std::string&);
    ~DirectionalLightNodePrivate() override;

    uint32_t shadowLayersCount() const override;

    uint32_t& shadowCascadesCount();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(LightHandler&) const override;

    uint32_t m_shadowCascadesCount;
};

} // namespace core
} // namespace simplex

#endif // CORE_DIRECTIONALLIGHTNODEPRIVATE_H
