#include <utils/logger.h>

#include <core/uniform.h>

#include "bilaterialblur.h"

namespace simplex
{
namespace core
{

float BilaterialBlur::s_defaultMaxDepthDifference = .03f;

BilaterialBlur::BilaterialBlur()
    : Blur()
{
    setMaxDepthDifference(s_defaultMaxDepthDifference);
}

BilaterialBlur::~BilaterialBlur() = default;

graphics::PConstTexture BilaterialBlur::sourceDepthMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::BlurSourceDepthMap));
    return uni ? uni->data() : nullptr;
}

void BilaterialBlur::setSourceDepthMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::BlurSourceDepthMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

float BilaterialBlur::maxDepthDifference() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::BlurMaxDepthDifference));
    return uni ? uni->data() : s_defaultMaxDepthDifference;
}

void BilaterialBlur::setMaxDepthDifference(float value)
{
    if (value <= 0.f)
        LOG_CRITICAL << "Blur max depth difference must be greater than 0.0";

    getOrCreateUniform(UniformId::BlurMaxDepthDifference) = makeUniform(value);
}


}
}
