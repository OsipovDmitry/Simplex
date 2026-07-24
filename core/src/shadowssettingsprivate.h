#ifndef CORE_SHADOWSSETTINGSPRIVATE_H
#define CORE_SHADOWSSETTINGSPRIVATE_H

#include <core/forwarddecl.h>
#include <core/shadowssettings.h>

namespace simplex
{
namespace core
{

class ShadowsSettingsPrivate
{
public:
    ShadowsSettingsPrivate(CameraNode&);
    ~ShadowsSettingsPrivate();

    CameraNode& cameraNode();

    ShadowFilter& filter();
    float& blurSigma();
    float& lightBleedingAmount();
    float& positiveExponent();
    float& negativeExponent();
    float& momentsBias();
    float& depthBiasFactor();
    float& cascadesBlendDistanceFactor();
    float& cascadesDistancePower();

private:
    CameraNode& m_cameraNode;
    ShadowFilter m_filter = ShadowFilter::Discrete;
    float m_blurSigma = 1.f;
    float m_lightBleedingAmount = .2f;
    float m_positiveExponent = 1.f;
    float m_negativeExponent = 1.f;
    float m_momentsBias = 0.f;
    float m_depthBiasFactor = 0.f;
    float m_cascadesBlendDistanceFactor = .15f;
    float m_cascadesDistancePower = 1.5f;
};

} // namespace core
} // namespace simplex

#endif // CORE_SHADOWSSETTINGSPRIVATE_H
