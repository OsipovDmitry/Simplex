#include <cassert>
#include <array>

#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/meshpainter.h>
#include <utils/mesh.h>

#include "triangledata.h"
#include "tetrahedrondata.h"
#include "cubedata.h"
#include "boundingboxdata.h"
#include "monkeydata.h"
#include "teapotdata.h"
#include "cameradata.h"
#include "screenquaddata.h"

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
        switch (buffer->componentType())
        {
        case VertexComponentType::Single: {
            buffer->setVertex(idx, glm::value_ptr(v));
            break;
        }
        case VertexComponentType::Double: {
            buffer->setVertex(idx, glm::value_ptr(glm::dvec4(v)));
            break;
        }
        case VertexComponentType::Int32: {
            buffer->setVertex(idx, glm::value_ptr(glm::i32vec4(v)));
            break;
        }
        case VertexComponentType::Uint32: {
            buffer->setVertex(idx, glm::value_ptr(glm::u32vec4(v)));
            break;
        }
        default:
            break;
        }
    }

    static void setIndexToBuffer(std::shared_ptr<DrawElementsBuffer> buffer, uint32_t idx, const uint32_t &v)
    {
        switch (buffer->type())
        {
        case DrawElementsIndexType::Uint8: {
            auto castedV = static_cast<uint8_t>(v);
            buffer->setIndex(idx, &castedV);
            break;
        }
        case DrawElementsIndexType::Uint16: {
            auto castedV = static_cast<uint16_t>(v);
            buffer->setIndex(idx, &castedV);
            break;
        }
        case DrawElementsIndexType::Uint32: {
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
    setMesh(mesh);

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

std::shared_ptr<const Mesh> AbstractPainter::mesh() const
{
    return const_cast<AbstractPainter*>(this)->mesh();
}

std::shared_ptr<Mesh> AbstractPainter::mesh()
{
    return m_->mesh;
}

void AbstractPainter::setMesh(std::shared_ptr<Mesh> mesh)
{
    m_->mesh = mesh;
}

const glm::vec4 &AbstractPainter::defaultValue(VertexAttribute attrib) const
{
    return m_->defaultValues[castFromVertexAttribute(attrib)];
}

void AbstractPainter::setDefaultValue(VertexAttribute attrib, const glm::vec4 &value)
{
    m_->defaultValues[castFromVertexAttribute(attrib)] = value;
}

const glm::mat4 &AbstractPainter::vertexTransform() const
{
    return m_->transforms[VertexAttribute::Position];
}

const glm::mat4 &AbstractPainter::normalTransform() const
{
    return m_->transforms[VertexAttribute::Normal];
}

void AbstractPainter::setVertexTransform(const glm::mat4 &vertexTransform)
{
    m_->transforms[VertexAttribute::Position] = vertexTransform;
    m_->transforms[VertexAttribute::Normal] = glm::inverseTranspose(vertexTransform);
}

const glm::mat4 &AbstractPainter::texCoordTransform() const
{
    return m_->transforms[VertexAttribute::TexCoord];
}

void AbstractPainter::setTexCoordTransform(const glm::mat4 &texCoordTransform)
{
    m_->transforms[VertexAttribute::TexCoord] = texCoordTransform;
}

void AbstractPainter::drawArrays(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices, PrimitiveType primitiveType)
{
    assert(m_->mesh);
    auto firstCount = addVertices(vertices);
    m_->mesh->attachPrimitiveSet(std::make_shared<DrawArrays>(primitiveType, firstCount.first, firstCount.second));
}

void AbstractPainter::drawElements(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                                   PrimitiveType primitiveType,
                                   const std::vector<uint32_t> &indices,
                                   DrawElementsIndexType drawElemetsIndexType)
{
    assert(m_->mesh);
    auto firstCount = addVertices(vertices);

    auto drawElemetsBuffer = std::make_shared<DrawElementsBuffer>(primitiveType,
                                                                  static_cast<uint32_t>(indices.size()),
                                                                  drawElemetsIndexType,
                                                                  firstCount.first);
    for (uint32_t i = 0u; i < static_cast<uint32_t>(indices.size()); ++i)
        AbstractPainterPrivate::setIndexToBuffer(drawElemetsBuffer, i, indices[i]);

    m_->mesh->attachPrimitiveSet(drawElemetsBuffer);
}

std::pair<uint32_t, uint32_t> AbstractPainter::addVertices(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices)
{
    uint32_t numVertices = m_->mesh->vertexBuffers().empty() ? 0u : m_->mesh->vertexBuffers().begin()->second->numVertices();
    uint32_t addedVerticesCount = vertices.empty() ? 0u : static_cast<uint32_t>(vertices.begin()->second.size());

    for (const auto &[attrib, buffer] : m_->mesh->vertexBuffers())
    {
        buffer->setNumVertices(numVertices + addedVerticesCount);

        if (auto bufferIt = vertices.find(attrib); bufferIt != vertices.end())
        {
            assert(bufferIt->second.size() == addedVerticesCount);

            if (auto transformIt = m_->transforms.find(attrib); transformIt != m_->transforms.end())
            {
                const auto &transform = transformIt->second;
                for (uint32_t i = 0u; i < addedVerticesCount; ++i)
                    AbstractPainterPrivate::setVertexToBuffer(buffer, numVertices + i, transform * bufferIt->second[i]);
            }
            else
            {
                for (uint32_t i = 0u; i < addedVerticesCount; ++i)
                    AbstractPainterPrivate::setVertexToBuffer(buffer, numVertices + i, bufferIt->second[i]);
            }
        }
        else
        {
            const auto &defaultValue = m_->defaultValues[castFromVertexAttribute(attrib)];
            for (uint32_t i = 0u; i < addedVerticesCount; ++i)
                AbstractPainterPrivate::setVertexToBuffer(buffer, numVertices + i, defaultValue);
        }
    }

    return std::make_pair(numVertices, addedVerticesCount);
}

MeshPainter::MeshPainter(std::shared_ptr<Mesh> mesh)
    : AbstractPainter(mesh)
{
}

MeshPainter::~MeshPainter()
{
}

void MeshPainter::drawTriangle()
{
    drawElements({{VertexAttribute::Position, s_triangleVertices},
                  {VertexAttribute::Normal, s_triangleNormals},
                  {VertexAttribute::TexCoord, s_triangleTexCoords}},
                 PrimitiveType::Triangles,
                 s_triangleIndices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawTetrahedron()
{
    drawElements({{VertexAttribute::Position, s_tetrahedronVertices},
                  {VertexAttribute::Normal, s_tetrahedronNormals},
                  {VertexAttribute::TexCoord, s_tetrahedronTexCoords}},
                 PrimitiveType::Triangles,
                 s_tetrahedronIndices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawCube()
{
    drawElements({{VertexAttribute::Position, s_cubeVertices},
                  {VertexAttribute::Normal, s_cubeNormals},
                  {VertexAttribute::TexCoord, s_cubeTexCoords}},
                 PrimitiveType::Triangles,
                 s_cubeIndices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawBoundingBox()
{
    drawElements({{VertexAttribute::Position, s_boundingBoxVertices}},
                 PrimitiveType::Lines,
                 s_boundingBoxIndices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawMonkey()
{
    drawElements({{VertexAttribute::Position, s_monkeyVertices},
                  {VertexAttribute::Normal, s_monkeyNormals},
                  {VertexAttribute::TexCoord, s_monkeyTexCoords}},
                 PrimitiveType::Triangles,
                 s_monkeyIndices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawTeapot()
{
    drawElements({{VertexAttribute::Position, s_teapotVertices},
                  {VertexAttribute::Normal, s_teapotNormals},
                  {VertexAttribute::TexCoord, s_teapotTexCoords}},
                 PrimitiveType::Triangles,
                 s_teapotIndices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawSphere(uint32_t segs)
{
    segs = glm::max(segs, 4u);

    std::vector<glm::vec4> vertices(segs * segs);
    std::vector<glm::vec4> normals(segs * segs);
    std::vector<glm::vec4> texCoords(segs * segs);
    std::vector<uint32_t> indices(6 * (segs-1) * segs);

    for (uint32_t a = 0u; a < segs; ++a)
    {
        const float fA = static_cast<float>(a) / static_cast<float>(segs-1);
        const float angleA = glm::pi<float>() * (fA - .5f);
        const float sinA = glm::sin(angleA);
        const float cosA = glm::cos(angleA);

        for (uint32_t b = 0; b < segs; ++b)
        {
            const float fB = static_cast<float>(b) / static_cast<float>(segs-1);
            const float angleB = 2.f * glm::pi<float>() * fB;
            const float sinB = glm::sin(angleB);
            const float cosB = glm::cos(angleB);

            glm::vec3 p(cosA * sinB, sinA, cosA * cosB);

            vertices[a * segs + b] = glm::vec4(p, 1.0f);
            normals[a * segs + b] = glm::vec4(p, 0.0f);
            texCoords[a * segs + b] = glm::vec4(fA, fB, 0.f, 1.f);
        }
    }

    for (uint32_t a = 0; a < segs-1; ++a)
        for (uint32_t b = 0; b < segs; ++b)
        {
            indices[6 * (a*segs + b) + 0] = a * segs + b;
            indices[6 * (a*segs + b) + 1] = a * segs + (b + 1) % segs;
            indices[6 * (a*segs + b) + 2] = (a+1) * segs + b;
            indices[6 * (a*segs + b) + 3] = a * segs + (b + 1) % segs;
            indices[6 * (a*segs + b) + 4] = (a+1) * segs + (b + 1) % segs;
            indices[6 * (a*segs + b) + 5] = (a+1) * segs + b;
        }

    drawElements({{VertexAttribute::Position, vertices},
                  {VertexAttribute::Normal, normals},
                  {VertexAttribute::TexCoord, texCoords}},
                 PrimitiveType::Triangles,
                 indices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawCamera()
{
    drawElements({{VertexAttribute::Position, s_cameraVertices}},
                 PrimitiveType::Lines,
                 s_cameraIndices,
                 DrawElementsIndexType::Uint32);
}

void MeshPainter::drawScreenQuad()
{
    drawArrays({{VertexAttribute::Position, s_screenQuadVertices}},
                 PrimitiveType::TriangleStrip);
}


}
}
