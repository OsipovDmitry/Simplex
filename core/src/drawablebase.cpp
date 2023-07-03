#include <core/drawablebase.h>

#include <utils/logger.h>
#include <utils/typeinfo.h>
#include <utils/boundingbox.h>
#include <utils/primitiveset.h>

#include "drawablebaseprivate.h"

namespace simplex
{
namespace core
{

DrawableBase::DrawableBase(std::shared_ptr<IGraphicsRenderer::RenderProgram> renderProgram,
                           std::shared_ptr<IGraphicsRenderer::VertexArray> vertexArray)
    : m_(std::make_unique<DrawableBasePrivate>(renderProgram, vertexArray))
{
    initialize();
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
        auto vertexAttributeType = vao->vertexAttributeType(utils::VertexAttribute::Position);
        auto vertexAttributeRelativeOffset = vao->vertexAttributeRelativeOffset(utils::VertexAttribute::Position);
        auto vertexAttributeTypeSize = utils::TypeInfo::size(vertexAttributeType);

        auto vertexBufferData = vertexBuffer->map(IGraphicsRenderer::Buffer::MapAccess::ReadOnly, vertexBufferOffset);

        for (auto &primitiveSet : vao->primitiveSets())
        {
            if (auto drawArray = primitiveSet->asDrawArrays(); drawArray)
            {
                auto drawArraysFirst = drawArray->first();
                auto drawArraysCount = drawArray->count();

                for (uint32_t i = 0; i < drawArraysCount; ++i)
                {
                    result += DrawableBasePrivate::readDataToVec<utils::BoundingBox::length(), utils::BoundingBox::value_type>(
                                vertexBufferData->get() + (i + drawArraysFirst) * vertexBufferStride + vertexAttributeRelativeOffset,
                                vertexAttributeNumComponents,
                                vertexAttributeType,
                                vertexAttributeTypeSize);
                }
            }
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
            {
                auto drawElementsCount = drawElements->count();
                auto drawElementsType = drawElements->type();
                auto drawElementsOffset = drawElements->offset();
                auto drawElementsBaseVertex = drawElements->baseVertex();
                auto indexSize = utils::TypeInfo::size(drawElementsType);

                auto indexBuffer = vao->indexBuffer();
                assert(indexBuffer);

                auto indexBufferData = indexBuffer->map(IGraphicsRenderer::Buffer::MapAccess::ReadOnly,
                                                        drawElementsOffset,
                                                        drawElementsCount * indexSize);

                for (uint32_t i = 0; i < drawElementsCount; ++i)
                {
                    auto index = DrawableBasePrivate::readDataToType<uint32_t>(
                                indexBufferData->get() + i * indexSize,
                                drawElementsType);

                    result += DrawableBasePrivate::readDataToVec<utils::BoundingBox::length(), utils::BoundingBox::value_type>(
                                vertexBufferData->get() + (index + drawElementsBaseVertex) * vertexBufferStride + vertexAttributeRelativeOffset,
                                vertexAttributeNumComponents,
                                vertexAttributeType,
                                vertexAttributeTypeSize);
                }
            }
        }
    }

    return result;
}

std::shared_ptr<IGraphicsRenderer::RenderProgram> DrawableBase::renderProgram()
{
    return m_->renderProgram();
}

std::shared_ptr<IGraphicsRenderer::VertexArray> DrawableBase::vertexArray()
{
    return m_->vertexArray();
}

void DrawableBase::setupUniform(const IGraphicsRenderer::RenderProgram::UniformInfo&)
{
}

DrawableBase::DrawableBase(DrawableBasePrivate *drawableBasePrivate)
    : m_(std::unique_ptr<DrawableBasePrivate>(drawableBasePrivate))
{
    initialize();
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
            LOG_ERROR << "Vertex array does not have vertex attribute \"" <<
                         rp->attributeNameByIndex(attributeInfo.index) <<
                         "\"";
        if (attributeInfo.type != utils::TypeInfo::makeVecType(vao->vertexAttributeType(attributeInfo.id),
                                                               vao->vertexAttributeNumComponents(attributeInfo.id)))
            LOG_ERROR << "Attributes of vertex array and render program have different types (" <<
                         rp->attributeNameByIndex(attributeInfo.index) <<
                         ")";
    }
}

}
}
