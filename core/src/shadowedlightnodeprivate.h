#ifndef CORE_SHADOWEDLIGHTNODEPRIVATE_H
#define CORE_SHADOWEDLIGHTNODEPRIVATE_H

#include "coloredlightnodeprivate.h"

namespace simplex
{
namespace core
{

class ShadowedLightNodePrivate : public ColoredLightNodePrivate
{
    PUBLIC_IMPL(ShadowedLightNodePrivate)

public:
    ShadowedLightNodePrivate(ShadowedLightNode&, const std::string&, LightType);
    ~ShadowedLightNodePrivate() override;

    virtual uint32_t shadowLayersCount() const = 0;

    std::shared_ptr<Shadow>& shadow();
    uint32_t& shadowMapSize();
    utils::Range& shadowCullPlanesLimits();

    bool& isVolumetricScatteringEnabled();

protected:
    std::shared_ptr<Shadow> m_shadow;
    uint32_t m_shadowMapSize;
    utils::Range m_shadowCullPlanesLimits;

    bool m_isVolumetricScatteringEnabled;
};

} // namespace core
} // namespace simplex

#endif // CORE_SHADOWEDLIGHTNODEPRIVATE_H
