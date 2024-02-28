#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/primitiveset.h>

#include <core/drawable.h>
#include <core/uniform.h>

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

Drawable::Drawable(const std::shared_ptr<graphics::IVertexArray> &vertexArray)
    : Drawable(std::make_unique<DrawablePrivate>(vertexArray))
{
}

Drawable::~Drawable() = default;

const utils::BoundingBox &Drawable::boundingBox() const
{
    static utils::BoundingBox s_emptyBoundingBox;
    return s_emptyBoundingBox;
}

bool Drawable::isDoubleSided() const
{
    return m_->isDoubleSided();
}

void Drawable::setDoubleSided(bool value)
{
    m_->isDoubleSided() = value;
}

float Drawable::alphaCutoff() const
{
    auto uni = uniform_cast<float>(uniform(graphics::UniformId::AlphaCutoff));
    return uni ? uni->data() : DrawablePrivate::defaultAlphaCutoff();
}

void Drawable::setAlphaCutoff(float value)
{
    getOrCreateUniform(graphics::UniformId::AlphaCutoff) = makeUniform(value);
}

DrawableAlphaMode Drawable::alphaMode() const
{
    auto result = DrawableAlphaMode::Opaque;

    if (alphaCutoff() > 0.f)
        result = DrawableAlphaMode::Mask;
    else if (vertexArray()->vertexAttributeNumComponents(utils::VertexAttribute::Color) > 3u)
        result = DrawableAlphaMode::Transparent;
    else
        result = DrawableAlphaMode::Opaque;

    return result;
}

std::shared_ptr<const graphics::IVertexArray> Drawable::vertexArray() const
{
    return const_cast<Drawable*>(this)->vertexArray();
}

std::shared_ptr<graphics::IVertexArray> Drawable::vertexArray()
{
    return m_->vertexArray();
}

PConstAbstractUniform Drawable::uniform(graphics::UniformId id) const
{
    return const_cast<Drawable*>(this)->uniform(id);
}

PAbstractUniform Drawable::uniform(graphics::UniformId id)
{
    auto it = m_->uniforms().find(id);
    return it != m_->uniforms().end() ? it->second : nullptr;
}

PAbstractUniform &Drawable::getOrCreateUniform(graphics::UniformId id)
{
    auto it = m_->uniforms().find(id);
    return it != m_->uniforms().end() ? it->second : (m_->uniforms()[id] = std::shared_ptr<AbstractUniform>());
}

PConstAbstractUniform Drawable::userUniform(const std::string& name) const
{
    return const_cast<Drawable*>(this)->userUniform(name);
}

PAbstractUniform Drawable::userUniform(const std::string& name)
{
    auto it = m_->userUniforms().find(name);
    return it != m_->userUniforms().end() ? it->second : nullptr;
}

PAbstractUniform &Drawable::getOrCreateUserUniform(const std::string& name)
{
    auto it = m_->userUniforms().find(name);
    return it != m_->userUniforms().end() ? it->second : (m_->userUniforms()[name] = std::shared_ptr<AbstractUniform>());
}

graphics::PConstBufferRange Drawable::SSBO(graphics::SSBOId id) const
{
    auto it = m_->SSBOs().find(id);
    return it != m_->SSBOs().end() ? it->second : nullptr;
}

utils::VertexAttributesSet Drawable::vertexAttrubitesSet(const std::shared_ptr<const Drawable> &drawable)
{
    utils::VertexAttributesSet result;

    auto vao = drawable->vertexArray();
    for (uint16_t i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        if (auto attrib = utils::castToVertexAttribute(i); vao->vertexAttributeBindingIndex(attrib) != static_cast<uint32_t>(-1))
            result.insert(attrib);

    return result;
}

Drawable::Drawable(std::unique_ptr<DrawablePrivate> drawablPrivate)
    : m_(std::move(drawablPrivate))
{
    setAlphaCutoff(DrawablePrivate::defaultAlphaCutoff());
}

}
}
