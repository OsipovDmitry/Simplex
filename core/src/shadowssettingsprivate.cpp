#include "shadowssettingsprivate.h"

namespace simplex
{
namespace core
{

ShadowsSettingsPrivate::ShadowsSettingsPrivate(CameraNode& cameraNode)
    : m_cameraNode(cameraNode)
{
}

ShadowsSettingsPrivate::~ShadowsSettingsPrivate() = default;

CameraNode& ShadowsSettingsPrivate::cameraNode()
{
    return m_cameraNode;
}

ShadowFilter& ShadowsSettingsPrivate::filter()
{
    return m_filter;
}

float& ShadowsSettingsPrivate::blurSigma()
{
    return m_blurSigma;
}

float& ShadowsSettingsPrivate::lightBleedingAmount()
{
    return m_lightBleedingAmount;
}

float& ShadowsSettingsPrivate::positiveExponent()
{
    return m_positiveExponent;
}

float& ShadowsSettingsPrivate::negativeExponent()
{
    return m_negativeExponent;
}

float& ShadowsSettingsPrivate::momentsBias()
{
    return m_momentsBias;
}

float& ShadowsSettingsPrivate::depthBiasFactor()
{
    return m_depthBiasFactor;
}

float& ShadowsSettingsPrivate::cascadesBlendDistanceFactor()
{
    return m_cascadesBlendDistanceFactor;
}

float& ShadowsSettingsPrivate::cascadesDistancePower()
{
    return m_cascadesDistancePower;
}

} // namespace core
} // namespace simplex
