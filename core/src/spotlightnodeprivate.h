#ifndef CORE_SPOTLIGHTNODEPRIVATE_H
#define CORE_SPOTLIGHTNODEPRIVATE_H

#include "shadowedlightnodeprivate.h"

namespace simplex
{
namespace core
{

class SpotLightNodePrivate : public ShadowedLightNodePrivate
{
    PUBLIC_IMPL(SpotLightNode)

public:
    SpotLightNodePrivate(SpotLightNode&, const std::string&);
    ~SpotLightNodePrivate() override;

    uint32_t shadowLayersCount() const override;

    utils::Range& radiuses();
    utils::Range& halfAngles();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(LightHandler&) const override;

    utils::Range m_radiuses;
    utils::Range m_halfAngles;
};

} // namespace core
} // namespace simplex

#endif // CORE_SPOTLIGHTNODEPRIVATE_H
