#include <utils/epsilon.h>

#include <core/igraphicsrenderer.h>
#include <core/visualdrawable.h>
#include <core/uniform.h>

#include "visualdrawableprivate.h"

namespace simplex
{
namespace core
{

VisualDrawable::~VisualDrawable() = default;

const utils::BoundingBox &VisualDrawable::boundingBox() const
{
    return m().boundingBox();
}

bool VisualDrawable::isTransparent() const
{
    bool result = false;

    if (vertexArray()->vertexAttributeNumComponents(utils::VertexAttribute::Color) > 3u)
        result = true;

    if (auto uni = uniform_cast<glm::vec4>(uniform(UniformId::BaseColor)); uni && (uni->data().a < 1.f - utils::epsilon<float>()))
        return true;

    if (auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformId::BaseColorMap)); uni && uni->data()->hasAlpha())
        return true;

    return result;
}

float VisualDrawable::alphaCutoff() const
{
    auto uni = uniform_cast<float>(uniform(UniformId::AlphaCutoff));
    return uni ? uni->data() : 1.f;
}

void VisualDrawable::setAlphaCutoff(float value)
{
    getOrCreateUniform(UniformId::AlphaCutoff) = makeUniform(value);
}

bool VisualDrawable::isLighted() const
{
    auto uni = uniform_cast<bool>(uniform(UniformId::IsLightedFlag));
    return uni ? uni->data() : false;
}

bool VisualDrawable::isShadowed() const
{
    auto uni = uniform_cast<bool>(uniform(UniformId::IsShadowedFlag));
    return uni ? uni->data() : false;
}

bool VisualDrawable::isShadowCasted() const
{
    return m().isShadowCastedFlag();
}

graphics::PConstBufferRange VisualDrawable::bonesBuffer() const
{
    return SSBO(SSBOId::BonesBuffer);
}

void VisualDrawable::setBonesBuffer(const graphics::PConstBufferRange &value)
{
    getOrCreateSSBO(SSBOId::BonesBuffer) = value;
}

const DrawableComponentSet &VisualDrawable::componentSet()
{
    static const DrawableComponentSet s_set {
        UniformId::AlphaCutoff,
        UniformId::BaseColor,
        UniformId::BaseColorMap,
        UniformId::Metalness,
        UniformId::MetalnessMap,
        UniformId::Roughness,
        UniformId::RoughnessMap,
        UniformId::NormalMap,
        UniformId::NormalMapScale,
        UniformId::OcclusionMap,
        UniformId::OcclusionMapStrength,
        UniformId::Emission,
        UniformId::EmissionMap
    };
    return s_set;
}

VisualDrawable::VisualDrawable(std::unique_ptr<VisualDrawablePrivate> visualDrawablePrivate)
    : Drawable(std::move(visualDrawablePrivate))
{
    setLightedFlag(false);
    setShadowedFlag(false);
    setShadowCastedFlag(false);
}

void VisualDrawable::setLightedFlag(bool value)
{
    getOrCreateUniform(UniformId::IsLightedFlag) = makeUniform(value);
}

void VisualDrawable::setShadowedFlag(bool value)
{
    getOrCreateUniform(UniformId::IsShadowedFlag) = makeUniform(value);
}

void VisualDrawable::setShadowCastedFlag(bool value)
{
    m().isShadowCastedFlag() = value;
}



}
}
