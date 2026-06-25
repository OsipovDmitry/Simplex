#ifndef CORE_POINTLIGHTNODEPRIVATE_H
#define CORE_POINTLIGHTNODEPRIVATE_H

#include "shadowedlightnodeprivate.h"

namespace simplex
{
namespace core
{

class PointLightNodePrivate : public ShadowedLightNodePrivate
{
    PUBLIC_IMPL(PointLightNode)

public:
    PointLightNodePrivate(PointLightNode&, const std::string&);
    ~PointLightNodePrivate() override;

    uint32_t shadowLayersCount() const override;

    utils::Range& radiuses();

private:
    std::shared_ptr<LightHandler> createLightInSceneData(SceneData&) const override;
    void updateLightInSceneData(LightHandler&) const override;

    utils::Range m_radiuses;
};

} // namespace core
} // namespace simplex

#endif // CORE_POINTLIGHTNODEPRIVATE_H
