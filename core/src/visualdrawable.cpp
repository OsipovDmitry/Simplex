#include <utils/epsilon.h>

#include <core/graphicsrendererbase.h>
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

    if (vertexArray()->vao()->vertexAttributeNumComponents(utils::VertexAttribute::Color) > 3u)
        result = true;

    if (auto uni = uniform_cast<glm::vec4>(uniform(UniformID::BaseColor)); uni && (uni->data().a < 1.f - utils::epsilon<float>()))
        return true;

    if (auto uni = uniform_cast<graphics::PConstTexture>(uniform(UniformID::BaseColorMap)); uni && uni->data()->hasAlpha())
        return true;

    return result;
}

float VisualDrawable::alphaCutoff() const
{
    auto uni = uniform_cast<float>(uniform(UniformID::AlphaCutoff));
    return uni ? uni->data() : 1.f;
}

void VisualDrawable::setAlphaCutoff(float value)
{
    getOrCreateUniform(UniformID::AlphaCutoff) = makeUniform(value);
}

bool VisualDrawable::isLighted() const
{
    auto uni = uniform_cast<bool>(uniform(UniformID::IsLightedFlag));
    return uni ? uni->data() : false;
}

bool VisualDrawable::isShadowed() const
{
    auto uni = uniform_cast<bool>(uniform(UniformID::IsShadowedFlag));
    return uni ? uni->data() : false;
}

bool VisualDrawable::isShadowCasted() const
{
    return m().isShadowCastedFlag();
}

graphics::PConstBufferRange VisualDrawable::bonesBuffer() const
{
    return shaderStorageBlock(ShaderStorageBlockID::BonesBuffer);
}

void VisualDrawable::setBonesBuffer(const graphics::PConstBufferRange &value)
{
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::BonesBuffer) = value;
}

const DrawableComponentSet &VisualDrawable::componentSet()
{
    static const DrawableComponentSet s_set {
        UniformID::AlphaCutoff,
        UniformID::BaseColor,
        UniformID::BaseColorMap,
        UniformID::Metalness,
        UniformID::MetalnessMap,
        UniformID::Roughness,
        UniformID::RoughnessMap,
        UniformID::NormalMap,
        UniformID::NormalMapScale,
        UniformID::OcclusionMap,
        UniformID::OcclusionMapStrength,
        UniformID::Emission,
        UniformID::EmissionMap
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
    getOrCreateUniform(UniformID::IsLightedFlag) = makeUniform(value);
}

void VisualDrawable::setShadowedFlag(bool value)
{
    getOrCreateUniform(UniformID::IsShadowedFlag) = makeUniform(value);
}

void VisualDrawable::setShadowCastedFlag(bool value)
{
    m().isShadowCastedFlag() = value;
}



}
}
