#include <cassert>
#include <array>

#include <utils/meshpainter.h>
#include <utils/mesh.h>
#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/glm/gtc/type_ptr.hpp>

#include "triangledata.h"
#include "cubedata.h"
#include "monkeydata.h"
#include "teapotdata.h"

namespace simplex
{
namespace utils
{

class AbstractPainterPrivate
{
public:
    AbstractPainterPrivate(std::shared_ptr<Mesh> m)
        : mesh(m)
    {
        defaultValues.fill(glm::vec4(0.f));
    }

    static void setVertexToBuffer(std::shared_ptr<VertexBuffer> buffer, uint32_t idx, const glm::vec4 &v)
    {
        switch (buffer->type())
        {
        case Type::Single: {
            buffer->setVertex(idx, glm::value_ptr(v));
            break;
        }
        case Type::Double: {
            buffer->setVertex(idx, glm::value_ptr(glm::dvec4(v)));
            break;
        }
        case Type::Int8: {
            buffer->setVertex(idx, glm::value_ptr(glm::i8vec4(v)));
            break;
        }
        case Type::Int16: {
            buffer->setVertex(idx, glm::value_ptr(glm::i16vec4(v)));
            break;
        }
        case Type::Int32: {
            buffer->setVertex(idx, glm::value_ptr(glm::i32vec4(v)));
            break;
        }
        case Type::Uint8: {
            buffer->setVertex(idx, glm::value_ptr(glm::u8vec4(v)));
            break;
        }
        case Type::Uint16: {
            buffer->setVertex(idx, glm::value_ptr(glm::u16vec4(v)));
            break;
        }
        case Type::Uint32: {
            buffer->setVertex(idx, glm::value_ptr(glm::u32vec4(v)));
            break;
        }
        default:
            break;
        }
    }

    static void setIndexToBuffer(std::shared_ptr<IndexBuffer> buffer, uint32_t idx, const uint32_t &v)
    {
        switch (buffer->type())
        {
        case Type::Uint8: {
            auto castedV = static_cast<uint8_t>(v);
            buffer->setIndex(idx, &castedV);
            break;
        }
        case Type::Uint16: {
            auto castedV = static_cast<uint16_t>(v);
            buffer->setIndex(idx, &castedV);
            break;
        }
        case Type::Uint32: {
            buffer->setIndex(idx, &v);
            break;
        }
        default:
            break;
        }
    }

    std::shared_ptr<Mesh> mesh;
    std::array<glm::vec4, numElementsVertexAttribute()> defaultValues;
    std::unordered_map<VertexAttribute, glm::mat4> transforms;
};

AbstractPainter::AbstractPainter(std::shared_ptr<Mesh> mesh)
    : m_(std::make_unique<AbstractPainterPrivate>(mesh))
{
    assert(mesh);

    setDefaultValue(VertexAttribute::Position, glm::vec4(0.f, 0.f, 0.f, 1.f));
    setDefaultValue(VertexAttribute::Normal, glm::vec4(0.f, 0.f, 1.f, 0.f));
    setDefaultValue(VertexAttribute::TexCoord, glm::vec4(0.f, 0.f, 0.f, 1.f));
    setDefaultValue(VertexAttribute::Tangent, glm::vec4(1.f, 0.f, 0.f, 1.f));
    setDefaultValue(VertexAttribute::Color, glm::vec4(.7f, .7f, .7f, 1.f));

    setVertexTransform(glm::mat4(1.f));
    setTexCoordTransform(glm::mat4(1.f));
}

AbstractPainter::~AbstractPainter()
{
}

std::shared_ptr<Mesh> AbstractPainter::mesh() const
{
    return m_->mesh;
}

const glm::vec4 &AbstractPainter::defaultValue(VertexAttribute attrib) const
{
    return m_->defaultValues[castFromVertexAttribute(attrib)];
}

void AbstractPainter::setDefaultValue(VertexAttribute attrib, const glm::vec4 &value)
{
    m_->defaultValues[castFromVertexAttribute(attrib)] = value;
}

const glm::mat4x4 &AbstractPainter::vertexTransform() const
{
    return m_->transforms[VertexAttribute::Position];
}

const glm::mat4x4 &AbstractPainter::normalTransform() const
{
    return m_->transforms[VertexAttribute::Normal];
}

void AbstractPainter::setVertexTransform(const glm::mat4x4 &vertexTransform)
{
    m_->transforms[VertexAttribute::Position] = vertexTransform;
    m_->transforms[VertexAttribute::Normal] = glm::inverseTranspose(vertexTransform);
}

const glm::mat4x4 &AbstractPainter::texCoordTransform() const
{
    return m_->transforms[VertexAttribute::TexCoord];
}

void AbstractPainter::setTexCoordTransform(const glm::mat4x4 &texCoordTransform)
{
    m_->transforms[VertexAttribute::TexCoord] = texCoordTransform;
}

MeshPainter::MeshPainter(std::shared_ptr<Mesh> mesh)
    : AbstractPainter(mesh)
{
    assert(mesh->primitiveType() == PrimitiveType::Triangles);
}

void MeshPainter::drawMesh(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                           const std::vector<uint32_t> &indices)
{
    const uint32_t vertexOffset = m_->mesh->vertexBuffers().empty() ? 0u : m_->mesh->vertexBuffers().begin()->second->numVertices();
    const uint32_t numVertices = vertices.empty() ? 0u : static_cast<uint32_t>(vertices.begin()->second.size());

    for (const auto &[attrib, buffer] : m_->mesh->vertexBuffers())
    {
        buffer->setNumVertices(vertexOffset + numVertices);

        if (auto bufferIt = vertices.find(attrib); bufferIt != vertices.end())
        {
            assert(bufferIt->second.size() == numVertices);
            if (auto transformIt = m_->transforms.find(attrib); transformIt != m_->transforms.end())
            {
                const auto &transform = transformIt->second;
                for (uint32_t i = 0u; i < numVertices; ++i)
                    AbstractPainterPrivate::setVertexToBuffer(buffer, vertexOffset + i, transform * bufferIt->second[i]);
            }
            else
            {
                for (uint32_t i = 0u; i < numVertices; ++i)
                    AbstractPainterPrivate::setVertexToBuffer(buffer, vertexOffset + i, bufferIt->second[i]);
            }
        }
        else
        {
            for (uint32_t i = 0u; i < numVertices; ++i)
                AbstractPainterPrivate::setVertexToBuffer(buffer, vertexOffset + i, m_->defaultValues[castFromVertexAttribute(attrib)]);
        }
    }

    if (auto indexBuffer = m_->mesh->indexBuffer(); indexBuffer && !indices.empty())
    {
        uint32_t offset = indexBuffer->numIndices();
        indexBuffer->setNumIndices(offset + static_cast<uint32_t>(indices.size()));

        for (uint32_t i = 0u; i < indices.size(); ++i)
            AbstractPainterPrivate::setIndexToBuffer(indexBuffer, offset + i, vertexOffset + indices[i]);
    }
}

void MeshPainter::drawTriangle()
{
    drawMesh({{VertexAttribute::Position, s_triangleVertices},
              {VertexAttribute::Normal, s_triangleNormals},
              {VertexAttribute::TexCoord, s_triangleTexCoords}},
             s_triangleIndices);
}

void MeshPainter::drawCube()
{
    drawMesh({{VertexAttribute::Position, s_cubeVertices},
              {VertexAttribute::Normal, s_cubeNormals},
              {VertexAttribute::TexCoord, s_cubeTexCoords}},
             s_cubeIndices);
}

void MeshPainter::drawMonkey()
{
    drawMesh({{VertexAttribute::Position, s_monkeyVertices},
              {VertexAttribute::Normal, s_monkeyNormals},
              {VertexAttribute::TexCoord, s_monkeyTexCoords}},
             s_monkeyIndices);
}

void MeshPainter::drawTeapot()
{
    drawMesh({{VertexAttribute::Position, s_teapotVertices},
              {VertexAttribute::Normal, s_teapotNormals},
              {VertexAttribute::TexCoord, s_teapotTexCoords}},
             s_teapotIndices);
}



}
}
