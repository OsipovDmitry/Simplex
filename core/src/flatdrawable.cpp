#include <core/settings.h>
#include <core/flatdrawable.h>
#include <core/uniform.h>

#include "visualdrawableprivate.h"

namespace simplex
{
namespace core
{

FlatDrawable::FlatDrawable(const std::shared_ptr<graphics::VAOMesh> &vao, const utils::BoundingBox &bb)
    : VisualDrawable(std::make_unique<VisualDrawablePrivate>(vao, bb))
{
}

FlatDrawable::~FlatDrawable() = default;

const glm::vec4 &FlatDrawable::color() const
{
    auto uni = uniform_cast<glm::vec4>(uniform(UniformId::BaseColor));
    return uni ? uni->data() : settings::Settings::instance().graphics().flat().color();
}

void FlatDrawable::setColor(const glm::vec4 &value)
{
    getOrCreateUniform(UniformId::BaseColor) = makeUniform(value);
}

graphics::PConstTexture FlatDrawable::colorMap() const
{
    auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::BaseColorMap));
    return uni ? uni->data() : nullptr;
}

void FlatDrawable::setColorMap(const graphics::PConstTexture &value)
{
    static UniformId s_uniformId = UniformId::BaseColorMap;

    if (value)
        getOrCreateUniform(s_uniformId) = makeUniform(value);
    else
        removeUniform(s_uniformId);
}

}
}
