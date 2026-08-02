#include "tonemappingprivate.h"

namespace simplex
{
namespace core
{

ToneMappingPrivate::ToneMappingPrivate(CameraNode& cameraNode)
    : m_cameraNode(cameraNode)
{
}

ToneMappingPrivate::~ToneMappingPrivate() = default;

CameraNode& ToneMappingPrivate::cameraNode()
{
    return m_cameraNode;
}

utils::Range& ToneMappingPrivate::luminanceRange()
{
    return m_luminanceRange;
}

utils::Range& ToneMappingPrivate::luminanceClampRange()
{
    return m_luminanceClampRange;
}

std::pair<float, float>& ToneMappingPrivate::pixelsFractionToTrim()
{
    return m_pixelsFractionToTrim;
}

float& ToneMappingPrivate::tauLight()
{
    return m_tauLight;
}

float& ToneMappingPrivate::tauDark()
{
    return m_tauDark;
}

float& ToneMappingPrivate::baseLuminance()
{
    return m_baseLuminance;
}

} // namespace core
} // namespace simplex
