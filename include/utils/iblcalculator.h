#ifndef UTILS_IBLCALCULATOR_H
#define UTILS_IBLCALCULATOR_H

#include <memory>
#include <vector>

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>

#include <utils/utilsglobal.h>
#include <utils/forwarddecl.h>
#include <utils/noncopyble.h>

namespace simplex
{
namespace utils
{

class SIMPEX_DECL_EXPORT IBLCalculator
{
    NONCOPYBLE(IBLCalculator)
public:
    explicit IBLCalculator() = delete;

    static glm::vec2 calculateBRDFLutOnePixel(uint32_t, uint32_t);
    static std::shared_ptr<Image> calclulateBRDFLut();

    static glm::vec3 calculateIrradianceOnePixel(const std::shared_ptr<Image>&, float x, float y); // x, y are [0..1]
    static std::shared_ptr<Image> calculateIrradiance(const std::shared_ptr<Image>&);

    static glm::vec3 calculateRadianceOnePixel(const std::shared_ptr<Image>&, float roughness, float x, float y); // x, y are [0..1]
    static std::vector<std::shared_ptr<Image>> calculateRadiance(const std::shared_ptr<Image>&, uint32_t numLevels = 0u);

    static void setBRDFLutSize(uint32_t);
    static void setBRDFLutSamplesCount(uint32_t);

    static void setIrradianseSize(uint32_t);
    static void setIrradianceSapmlesDeltaAngle(float);

    static void setRadianceSize(uint32_t);
    static void setRadianceSamplesCount(uint32_t);

private:
    static uint32_t s_BRDFLutSize;
    static uint32_t s_BRDFLutSamplesCount;

    static uint32_t s_irradianceSize;
    static float s_irradianceSapmlesDeltaAngle;

    static uint32_t s_radianceSize;
    static uint32_t s_radianceSamplesCount;
};

}
}

#endif // UTILS_IBLCALCULATOR_H
