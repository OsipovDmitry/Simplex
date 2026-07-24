#ifndef CORE_SHADOWSSETTINGS_H
#define CORE_SHADOWSSETTINGS_H

#include <memory>

#include <utils/enumclass.h>
#include <utils/noncopyble.h>

#include <core/coreglobal.h>

namespace simplex
{
namespace core
{

ENUMCLASS(ShadowFilter, uint16_t, Discrete, VSM, EVSM, HamburgerMSM, HausdorffMSM)

class ShadowsSettingsPrivate;
class CORE_SHARED_EXPORT ShadowsSettings
{
    NONCOPYBLE(ShadowsSettings)
public:
    ShadowsSettings(std::unique_ptr<ShadowsSettingsPrivate>&&);
    ~ShadowsSettings();

    ShadowFilter filter() const;
    void setFilter(ShadowFilter);

    float blurSigma() const;
    void setBlurSigma(float);

    float lightBleedingAmount() const;
    void setLightBleedingAmount(float);

    float positiveExponent() const;
    void setPositiveExponent(float);

    float negativeExponent() const;
    void setNegativeExponent(float);

    float momentsBias() const;
    void setMomentsBias(float);

    float depthBiasFactor() const;
    void setDepthsBiasFactor(float);

    float cascadesBlendDistanceFactor() const;
    void setCascadesBlendDistanceFactor(float) const;

    float cascadesDistancePower() const;
    void setCascadesDistancePower(float) const;

private:
    std::unique_ptr<ShadowsSettingsPrivate> m_;
};

} // namespace core
} // namespace simplex

#endif // CORE_SHADOWSSETTINGS_H
