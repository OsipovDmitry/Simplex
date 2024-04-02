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

    if (vertexArray()->vertexAttributeNumComponents(utils::VertexAttribute::Color) > 3u)
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
    return it != m_->uniforms().end() ? it->second : (m_->uniforms()[id] = PAbstractUniform());
}

void Drawable::removeUniform(graphics::UniformId id)
{
    m_->uniforms().erase(id);
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
    return it != m_->userUniforms().end() ? it->second : (m_->userUniforms()[name] = PAbstractUniform());
}

void Drawable::removeUserUniform(const std::string &name)
{
    m_->userUniforms().erase(name);
}

graphics::PConstBufferRange Drawable::SSBO(graphics::SSBOId id) const
{
    auto it = m_->SSBOs().find(id);
    return it != m_->SSBOs().end() ? it->second : nullptr;
}

graphics::PConstBufferRange &Drawable::getOrCreateSSBO(graphics::SSBOId id)
{
    auto it = m_->SSBOs().find(id);
    return it != m_->SSBOs().end() ? it->second : (m_->SSBOs()[id] = graphics::PConstBufferRange());
}

void Drawable::removeSSBO(graphics::SSBOId id)
{
    m_->SSBOs().erase(id);
}

utils::VertexAttributesSet Drawable::vertexAttrubitesSet() const
{
    utils::VertexAttributesSet result;

    auto vao = vertexArray();
    for (typename std::underlying_type<utils::VertexAttribute>::type i = 0u; i < utils::numElementsVertexAttribute(); ++i)
        if (auto attrib = utils::castToVertexAttribute(i); vao->vertexAttributeBindingIndex(attrib) != static_cast<uint32_t>(-1))
            result.insert(attrib);

    return result;
}

Drawable::Drawable(std::unique_ptr<DrawablePrivate> drawablePrivate)
    : m_(std::move(drawablePrivate))
{
    setAlphaCutoff(DrawablePrivate::defaultAlphaCutoff());
    setDoubleSided(false);
}

}
}
