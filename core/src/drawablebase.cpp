#include <core/drawablebase.h>

#include <utils/logger.h>
#include <utils/boundingbox.h>
#include <utils/primitiveset.h>

#include "drawablebaseprivate.h"

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
    numComponents = glm::max(numComponents, VecLength);

    for (uint32_t i = 0u; i < numComponents; ++i)
        readDataToType<T>(data + i * typeSize, type, result[static_cast<int>(i)]);
}

DrawableBase::DrawableBase(std::shared_ptr<graphics::IRenderProgram> renderProgram, std::shared_ptr<graphics::IVertexArray> vertexArray)
    : DrawableBase(std::make_unique<DrawableBasePrivate>(renderProgram, vertexArray))
{
}

DrawableBase::~DrawableBase()
{
}

utils::BoundingBox DrawableBase::calculateBoundingBox()
{
    utils::BoundingBox result;
    auto vao = m_->vertexArray();

    if (auto bindingIndex = vao->vertexAttributeBindingIndex(utils::VertexAttribute::Position);
            bindingIndex != static_cast<uint32_t>(-1))
    {
        auto vertexBuffer = vao->vertexBuffer(bindingIndex);
        assert(vertexBuffer);

        auto vertexBufferOffset = vao->vertexBufferOffset(bindingIndex);
        auto vertexBufferStride = vao->vertexBufferStride(bindingIndex);

        auto vertexAttributeNumComponents = vao->vertexAttributeNumComponents(utils::VertexAttribute::Position);
        auto vertexAttributeType = vao->vertexAttributeComponentType(utils::VertexAttribute::Position);
        auto vertexAttributeRelativeOffset = vao->vertexAttributeRelativeOffset(utils::VertexAttribute::Position);
        auto vertexAttributeComponentSize = utils::VertexBuffer::componentSize(vertexAttributeType);

        auto vertexBufferData = vertexBuffer->map(graphics::IBuffer::MapAccess::ReadOnly, vertexBufferOffset);

        for (auto &primitiveSet : vao->primitiveSets())
        {
            if (auto drawArray = primitiveSet->asDrawArrays(); drawArray)
            {
                auto drawArraysFirst = drawArray->first();
                auto drawArraysCount = drawArray->count();

                utils::BoundingBox::PointType p(0.f);

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
                assert(indexBuffer);

                auto indexBufferData = indexBuffer->map(graphics::IBuffer::MapAccess::ReadOnly,
                                                        drawElementsOffset,
                                                        drawElementsCount * drawElementsIndexSize);

                uint32_t index;
                utils::BoundingBox::PointType p(0.f);

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
    }

    return result;
}

std::shared_ptr<graphics::IRenderProgram> DrawableBase::renderProgram()
{
    return m_->renderProgram();
}

std::shared_ptr<graphics::IVertexArray> DrawableBase::vertexArray()
{
    return m_->vertexArray();
}

std::shared_ptr<const AbstractUniform> DrawableBase::uniform(const graphics::UniformInfo &uniformInfo) const
{
    std::shared_ptr<const AbstractUniform> result;

    if (auto it = m_->uniforms().find(uniformInfo.location); it != m_->uniforms().end())
        result = it->second;
    else
    {
        auto uniformName = m_->renderProgram()->uniformNameByIndex(uniformInfo.index);
        LOG_ERROR << "Undefined uniform name \"" << uniformName << "\" in render program";
    }

    return result;
}

DrawableBase::DrawableBase(std::unique_ptr<DrawableBasePrivate> drawableBasePrivate)
    : m_(std::move(drawableBasePrivate))
{
    initialize();
}

void DrawableBase::addUniform(int32_t location, std::shared_ptr<AbstractUniform> uniform)
{
    m_->uniforms().insert({location, uniform});
}

void DrawableBase::removeUniform(int32_t location)
{
    m_->uniforms().erase(location);
}

std::shared_ptr<AbstractUniform> DrawableBase::uniform(int32_t location) const
{
    auto it = m_->uniforms().find(location);
    return (it != m_->uniforms().end()) ? it->second : nullptr;
}

void DrawableBase::initialize()
{
    auto vao = m_->vertexArray();
    auto rp = m_->renderProgram();

    assert(vao);
    assert(rp);

    for (const auto &attributeInfo : rp->attributesInfo())
    {
        auto bindingIndex = vao->vertexAttributeBindingIndex(attributeInfo.id);
        if (bindingIndex == static_cast<uint32_t>(-1))
        {
            LOG_ERROR << "Vertex array does not have vertex attribute \"" <<
                         rp->attributeNameByIndex(attributeInfo.index) <<
                         "\"";
        }
        else if ((attributeInfo.numComponents != vao->vertexAttributeNumComponents(attributeInfo.id)) ||
                 (attributeInfo.componentType != vao->vertexAttributeComponentType(attributeInfo.id)))
        {
            LOG_ERROR << "Attributes of vertex array and render program have different types (" <<
                         rp->attributeNameByIndex(attributeInfo.index) <<
                         ")";
        }
    }
}

}
}
