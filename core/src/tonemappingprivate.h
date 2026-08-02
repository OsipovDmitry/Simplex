#ifndef CORE_TONEMAPPINGPRIVATE_H
#define CORE_TONEMAPPINGPRIVATE_H

#include <utils/glm/exponential.hpp>
#include <utils/range.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

class ToneMappingPrivate
{
public:
    ToneMappingPrivate(CameraNode&);
    ~ToneMappingPrivate();

    CameraNode& cameraNode();

    utils::Range& luminanceRange();
    utils::Range& luminanceClampRange();
    std::pair<float, float>& pixelsFractionToTrim();
    float& tauLight();
    float& tauDark();
    float& baseLuminance();

private:
    CameraNode& m_cameraNode;

    utils::Range m_luminanceRange = utils::Range(glm::exp2(glm::vec2(-5.f, 10.0f)));
    utils::Range m_luminanceClampRange = utils::Range(glm::vec2(0.2f, 12.0f));
    std::pair<float, float> m_pixelsFractionToTrim{0.05f, 0.05f};
    float m_tauLight = 2.f;
    float m_tauDark = 1.f;
    float m_baseLuminance = 0.18f;
};

} // namespace core
} // namespace simplex

#endif // CORE_TONEMAPPINGPRIVATE_H
