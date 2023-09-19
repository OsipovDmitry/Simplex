#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/primitiveset.h>

#include <core/igraphicsrenderer.h>
#include <core/drawable.h>
#include <core/uniform.h>

#include "drawableprivate.h"

namespace simplex
{
namespace core
{

template<typename T>
static void readDataToType(const uint8_t* data, utils::VertexComponentType type, T &result)
{
    switch (type)
    {
    case utils::VertexComponentType::Single: { result = static_cast<T>(*reinterpret_cast<const float*>(data)); break; }
    case utils::VertexComponentType::Double: { result = static_cast<T>(*reinterpret_cast<const double*>(data)); break; }
    case utils::VertexComponentType::Int32: { result = static_cast<T>(*reinterpret_cast<const int32_t*>(data)); break; }
    case utils::VertexComponentType::Uint32: { result = static_cast<T>(*reinterpret_cast<const uint32_t*>(data)); break; }
    default: { break; }
    }
}

template<typename T>
static void readDataToType(const uint8_t* data, utils::DrawElementsIndexType type, T &result)
{
    switch (type)
    {
    case utils::DrawElementsIndexType::Uint8: { result = static_cast<T>(*reinterpret_cast<const uint8_t*>(data)); break; }
    case utils::DrawElementsIndexType::Uint16: { result = static_cast<T>(*reinterpret_cast<const uint16_t*>(data)); break; }
    case utils::DrawElementsIndexType::Uint32: { result = static_cast<T>(*reinterpret_cast<const uint32_t*>(data)); break; }
    default: { break; }
    }
}

template<uint32_t VecLength, typename T>
static void readDataToVec(const uint8_t* data,
                          uint32_t numComponents,
                          utils::VertexComponentType type,
                          uint32_t typeSize,
                          glm::vec<VecLength, T> &result)
{
    numComponents = glm::min(numComponents, VecLength);

    for (uint32_t i = 0u; i < numComponents; ++i)
        readDataToType<T>(data + i * typeSize, type, result[static_cast<glm::length_t>(i)]);
}

Drawable::Drawable(const std::shared_ptr<graphics::IVertexArray> &vertexArray)
    : Drawable(std::make_unique<DrawablePrivate>(vertexArray))
{
}

Drawable::~Drawable()
{
}

utils::BoundingBox Drawable::calculateBoundingBox() const
{
    utils::BoundingBox result;

    auto vao = m_->vertexArray();
    if (!vao)
        return result;

    auto bindingIndex = vao->vertexAttributeBindingIndex(utils::VertexAttribute::Position);
    if (bindingIndex == static_cast<uint32_t>(-1))
        return result;

    auto vertexBuffer = vao->vertexBuffer(bindingIndex);
    if (!vertexBuffer)
        return result;

    auto vertexBufferOffset = vao->vertexBufferOffset(bindingIndex);
    auto vertexBufferStride = vao->vertexBufferStride(bindingIndex);

    auto vertexAttributeNumComponents = vao->vertexAttributeNumComponents(utils::VertexAttribute::Position);
    auto vertexAttributeType = vao->vertexAttributeComponentType(utils::VertexAttribute::Position);
    auto vertexAttributeRelativeOffset = vao->vertexAttributeRelativeOffset(utils::VertexAttribute::Position);
    auto vertexAttributeComponentSize = utils::VertexBuffer::componentSize(vertexAttributeType);

    auto vertexBufferData = vertexBuffer->map(graphics::IBuffer::MapAccess::ReadOnly, vertexBufferOffset);

    for (auto &primitiveSet : vao->primitiveSets())
    {
        if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
        {
            auto drawArraysFirst = drawArrays->first();
            auto drawArraysCount = drawArrays->count();

            utils::BoundingBox::PointType p(static_cast<utils::BoundingBox::value_type>(0));

            for (uint32_t i = 0; i < drawArraysCount; ++i)
            {
                readDataToVec<utils::BoundingBox::length(), utils::BoundingBox::value_type>(
                            vertexBufferData->get() + (i + drawArraysFirst) * vertexBufferStride + vertexAttributeRelativeOffset,
                            vertexAttributeNumComponents,
                            vertexAttributeType,
                            vertexAttributeComponentSize,
                            p);
                result += p;
            }
        }
        else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
        {
            auto drawElementsCount = drawElements->count();
            auto drawElementsType = drawElements->type();
            auto drawElementsOffset = drawElements->offset();
            auto drawElementsBaseVertex = drawElements->baseVertex();
            auto drawElementsIndexSize = utils::DrawElements::indexSize(drawElementsType);

            auto indexBuffer = vao->indexBuffer();
            if (!indexBuffer)
                continue;

            auto indexBufferData = indexBuffer->map(graphics::IBuffer::MapAccess::ReadOnly,
                                                    drawElementsOffset,
                                                    drawElementsCount * drawElementsIndexSize);

            uint32_t index;
            utils::BoundingBox::PointType p(static_cast<utils::BoundingBox::value_type>(0));

            for (uint32_t i = 0; i < drawElementsCount; ++i)
            {
                readDataToType<uint32_t>(indexBufferData->get() + i * drawElementsIndexSize, drawElementsType, index);
                readDataToVec<utils::BoundingBox::length(), utils::BoundingBox::value_type>(
                            vertexBufferData->get() + (index + drawElementsBaseVertex) * vertexBufferStride + vertexAttributeRelativeOffset,
                            vertexAttributeNumComponents,
                            vertexAttributeType,
                            vertexAttributeComponentSize,
                            p);
                result += p;
            }
        }
    }

    return result;
}

bool Drawable::isTransparent() const
{
    return false;
    //return m_->vertexArray()->vertexAttributeNumComponents(utils::VertexAttribute::Color) == 4u;
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

graphics::PBRComponentsSet Drawable::PBRComponentsSet(const std::shared_ptr<const Drawable> &drawable)
{
    graphics::PBRComponentsSet result;

    for (uint16_t i = 0u; i < graphics::numElementsPBRComponent(); ++i)
    {
        auto pbrComponent = graphics::castToPBRComponent(i);
        if (auto uniformId = graphics::IProgram::uniformIdByPBRComponent(pbrComponent);
                (uniformId != graphics::UniformId::Undefined) && drawable->uniform(uniformId))
            result.insert(pbrComponent);
    }

    return result;
}

Drawable::Drawable(std::unique_ptr<DrawablePrivate> drawableBasePrivate)
    : m_(std::move(drawableBasePrivate))
{
}

}
}
