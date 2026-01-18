#include <array>

#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/logger.h>
#include <utils/transform.h>
#include <utils/meshpainter.h>
#include <utils/mesh.h>

#include "meshimpl.h"
#include "triangledata.h"
#include "planedata.h"
#include "tetrahedrondata.h"
#include "cubedata.h"
#include "boundingboxdata.h"
#include "suzannedata.h"
#include "bunnydata.h"
#include "teapotdata.h"
#include "cameradata.h"
#include "screenquaddata.h"


namespace simplex
{
namespace utils
{

template<typename V>
inline void calculateNormalImpl(const std::shared_ptr<DrawArrays> &drawArrays,
                                const VertexBuffer &vertices,
                                VertexBuffer &normals)
{
    calculateNormal(drawArrays,
                    vertices.numComponents(),
                    reinterpret_cast<const V*>(vertices.data()),
                    normals.numComponents(),
                    reinterpret_cast<V*>(normals.data()));
}

template<typename V, typename I>
inline void calculateNormalImpl(const std::shared_ptr<DrawElements> &drawElements,
                                const VertexBuffer &vertices,
                                const Buffer &indices,
                                VertexBuffer &normals)
{
    calculateNormal(drawElements,
                    vertices.numComponents(),
                    reinterpret_cast<const V*>(vertices.data()),
                    reinterpret_cast<const I*>(indices.data()),
                    normals.numComponents(),
                    reinterpret_cast<V*>(normals.data()));
}

template<typename V>
inline void calculateNormalImpl(const std::shared_ptr<DrawElementsBuffer> &drawElementsBuffer,
                                const VertexBuffer &vertices,
                                VertexBuffer &normals)
{
    switch (drawElementsBuffer->indexType())
    {
    case DrawElementsIndexType::Uint8:
        calculateNormalImpl<V, uint8_t>(drawElementsBuffer, vertices, *drawElementsBuffer, normals);
        break;
    case DrawElementsIndexType::Uint16:
        calculateNormalImpl<V, uint16_t>(drawElementsBuffer, vertices, *drawElementsBuffer, normals);
        break;
    case DrawElementsIndexType::Uint32:
        calculateNormalImpl<V, uint32_t>(drawElementsBuffer, vertices, *drawElementsBuffer, normals);
        break;
    default:
        LOG_ERROR << "Undefined draw elements buffer type";
        break;
    }
}

template<typename V>
inline void normalizeImpl(size_t numVertices,
                          const VertexBuffer &normals,
                          VertexBuffer &result)
{
    normalize(numVertices,
              normals.numComponents(),
              reinterpret_cast<const V*>(normals.data()),
              result.numComponents(),
              reinterpret_cast<V*>(result.data()));
}

template<typename V>
inline void calculateTangentSpaceImpl(const std::shared_ptr<DrawArrays> &drawArrays,
                                      const VertexBuffer &vertices,
                                      const VertexBuffer &texCoords,
                                      VertexBuffer &tangents,
                                      VertexBuffer &binormals)
{
    calculateTangentSpace(drawArrays,
                          vertices.numComponents(),
                          reinterpret_cast<const V*>(vertices.data()),
                          texCoords.numComponents(),
                          reinterpret_cast<const V*>(texCoords.data()),
                          tangents.numComponents(),
                          reinterpret_cast<V*>(tangents.data()),
                          binormals.numComponents(),
                          reinterpret_cast<V*>(binormals.data()));
}

template<typename V, typename I>
inline void calculateTangentSpaceImpl(const std::shared_ptr<DrawElements> &drawElements,
                                      const VertexBuffer &vertices,
                                      const VertexBuffer &texCoords,
                                      const Buffer &indices,
                                      VertexBuffer &tangents,
                                      VertexBuffer &binormals)
{
    calculateTangentSpace(drawElements,
                          vertices.numComponents(),
                          reinterpret_cast<const V*>(vertices.data()),
                          texCoords.numComponents(),
                          reinterpret_cast<const V*>(texCoords.data()),
                          reinterpret_cast<const I*>(indices.data()),
                          tangents.numComponents(),
                          reinterpret_cast<V*>(tangents.data()),
                          binormals.numComponents(),
                          reinterpret_cast<V*>(binormals.data()));
}

template<typename V>
inline void calculateTangentSpaceImpl(const std::shared_ptr<DrawElementsBuffer> &drawElementsBuffer,
                                      const VertexBuffer &vertices,
                                      const VertexBuffer &texCoords,
                                      VertexBuffer &tangents,
                                      VertexBuffer &binormals)
{
    switch (drawElementsBuffer->indexType())
    {
    case DrawElementsIndexType::Uint8:
        calculateTangentSpaceImpl<V, uint8_t>(drawElementsBuffer, vertices, texCoords, *drawElementsBuffer, tangents, binormals);
        break;
    case DrawElementsIndexType::Uint16:
        calculateTangentSpaceImpl<V, uint16_t>(drawElementsBuffer, vertices, texCoords, *drawElementsBuffer, tangents, binormals);
        break;
    case DrawElementsIndexType::Uint32:
        calculateTangentSpaceImpl<V, uint32_t>(drawElementsBuffer, vertices, texCoords, *drawElementsBuffer, tangents, binormals);
        break;
    default:
        LOG_ERROR << "Undefined draw elements buffer type";
        break;
    }
}

template<typename V>
inline void orthogonalizeTangentSpaceImpl(size_t numVertices,
                                          const VertexBuffer &tangents,
                                          const VertexBuffer &binormals,
                                          const VertexBuffer &normals,
                                          VertexBuffer &result)
{
    orthogonalizeTangentSpace(numVertices,
                              tangents.numComponents(),
                              reinterpret_cast<const V*>(tangents.data()),
                              binormals.numComponents(),
                              reinterpret_cast<const V*>(binormals.data()),
                              normals.numComponents(),
                              reinterpret_cast<const V*>(normals.data()),
                              result.numComponents(),
                              reinterpret_cast<V*>(result.data()));
}

template<typename V>
inline BoundingBox calculateBoundingBoxImpl(const std::shared_ptr<DrawArrays> &drawArrays,
                                            const VertexBuffer &vertices)
{
    return calculateBoundingBox<V>(drawArrays,
                                   vertices.numComponents(),
                                   reinterpret_cast<const V*>(vertices.data()));
}

template<typename V, typename I>
inline BoundingBox calculateBoundingBoxImpl(const std::shared_ptr<DrawElements> &drawElements,
                                            const VertexBuffer &vertices,
                                            const Buffer &indices)
{
    return calculateBoundingBox<V, I>(drawElements,
                                      vertices.numComponents(),
                                      reinterpret_cast<const V*>(vertices.data()),
                                      reinterpret_cast<const I*>(indices.data()));
}

template<typename V>
inline BoundingBox calculateBoundingBoxImpl(const std::shared_ptr<DrawElementsBuffer> &drawElementsBuffer,
                                            const VertexBuffer &vertices)
{
    BoundingBox result;
    switch (drawElementsBuffer->indexType())
    {
    case DrawElementsIndexType::Uint8:
        result = calculateBoundingBoxImpl<V, uint8_t>(drawElementsBuffer, vertices, *drawElementsBuffer);
        break;
    case DrawElementsIndexType::Uint16:
        result = calculateBoundingBoxImpl<V, uint16_t>(drawElementsBuffer, vertices, *drawElementsBuffer);
        break;
    case DrawElementsIndexType::Uint32:
        result = calculateBoundingBoxImpl<V, uint32_t>(drawElementsBuffer, vertices, *drawElementsBuffer);
        break;
    default:
        LOG_ERROR << "Undefined draw elements buffer type";
        break;
    }

    return result;
}

inline void setVertexToBuffer(std::shared_ptr<VertexBuffer> buffer, size_t idx, const glm::vec4 &v)
{
    switch (buffer->componentType())
    {
    case VertexComponentType::Single: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(v)));
        break;
    }
    case VertexComponentType::Double: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(glm::dvec4(v))));
        break;
    }
    case VertexComponentType::Int8: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(glm::i8vec4(v))));
        break;
    }
    case VertexComponentType::Uint8: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(glm::u8vec4(v))));
        break;
    }
    case VertexComponentType::Int16: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(glm::i16vec4(v))));
        break;
    }
    case VertexComponentType::Uint16: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(glm::u16vec4(v))));
        break;
    }
    case VertexComponentType::Int32: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(glm::i32vec4(v))));
        break;
    }
    case VertexComponentType::Uint32: {
        buffer->setVertex(idx, reinterpret_cast<const uint8_t*>(glm::value_ptr(glm::u32vec4(v))));
        break;
    }
    default:
        break;
    }
}

inline void setIndexToBuffer(std::shared_ptr<DrawElementsBuffer> buffer, size_t idx, const uint32_t &v)
{
    switch (buffer->indexType())
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

class AbstractPainterPrivate
{
public:
    AbstractPainterPrivate(std::shared_ptr<Mesh> m)
        : mesh(m)
    {
        defaultValues.fill(glm::vec4(0.f));
    }

    void calculateNormals()
    {
        const auto &vertexBuffers = mesh->vertexBuffers();
        const auto &primitiveSets = mesh->primitiveSets();

        if (!vertexBuffers.count(VertexAttribute::Position))
        {
            LOG_ERROR << "Mesh doesn't have Position vertex buffer";
            return;
        }

        const auto& vertices = vertexBuffers.at(VertexAttribute::Position);
        const auto numVertices = vertices->numVertices();
        const auto vertexComponentType = vertices->componentType();

        auto normals = std::make_shared<VertexBuffer>(numVertices, 3u, vertexComponentType);
        mesh->attachVertexBuffer(VertexAttribute::Normal, normals);

        auto n = std::make_shared<VertexBuffer>(numVertices, 3u, vertexComponentType);
        std::memset(n->data(), 0, n->sizeInBytes());

        for (const auto &primitiveSet : primitiveSets)
        {
            if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
            {
                switch (vertexComponentType)
                {
                case VertexComponentType::Single:
                    calculateNormalImpl<float>(drawArrays, *vertices, *n);
                    break;
                case VertexComponentType::Double:
                    calculateNormalImpl<double>(drawArrays, *vertices, *n);
                    break;
                default:
                    LOG_ERROR << "Vertex component type must be floating point";
                    break;
                }
            }
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
            {
                if (auto drawElementsBuffer = drawElements->asDrawElementsBuffer(); drawElementsBuffer)
                {
                    switch (vertexComponentType)
                    {
                    case VertexComponentType::Single:
                        calculateNormalImpl<float>(drawElementsBuffer, *vertices, *n);
                        break;
                    case VertexComponentType::Double:
                        calculateNormalImpl<double>(drawElementsBuffer, *vertices, *n);
                        break;
                    default:
                        LOG_ERROR << "Vertex component type must be floating point";
                        break;
                    }
                }
            }
        }

        switch (vertexComponentType)
        {
        case VertexComponentType::Single:
            normalizeImpl<float>(numVertices, *n, *normals);
            break;
        case VertexComponentType::Double:
            normalizeImpl<double>(numVertices, *n, *normals);
            break;
        default:
            LOG_ERROR << "Vertex component type must be floating point";
            break;
        }
    }

    void calculateTangents()
    {
        const auto &vertexBuffers = mesh->vertexBuffers();
        const auto &primitiveSets = mesh->primitiveSets();

        if (!vertexBuffers.count(VertexAttribute::Position))
        {
            LOG_ERROR << "Mesh doesn't have Position vertex buffer";
            return;
        }

        if (!vertexBuffers.count(VertexAttribute::TexCoords))
        {
            LOG_ERROR << "Mesh doesn't have TexCoords vertex buffer";
            return;
        }

        if (!vertexBuffers.count(VertexAttribute::Normal))
        {
            LOG_ERROR << "Mesh doesn't have Normal vertex buffer";
            return;
        }

        const auto& vertices = vertexBuffers.at(VertexAttribute::Position);
        const auto& texCoords = vertexBuffers.at(VertexAttribute::TexCoords);
        const auto& normals = vertexBuffers.at(VertexAttribute::Normal);

        const auto numVertices = vertices->numVertices();
        if (numVertices != texCoords->numVertices())
        {
            LOG_ERROR << "Buffers have different sizes";
            return;
        }

        if (numVertices != normals->numVertices())
        {
            LOG_ERROR << "Buffers have different sizes";
            return;
        }

        const auto vertexComponentType = vertices->componentType();
        if (vertexComponentType != texCoords->componentType())
        {
            LOG_ERROR << "Buffers have different component types";
            return;
        }

        if (vertexComponentType != normals->componentType())
        {
            LOG_ERROR << "Buffers have different component types";
            return;
        }

        auto tangents = std::make_shared<VertexBuffer>(numVertices, 4u, vertexComponentType);
        mesh->attachVertexBuffer(VertexAttribute::Tangent, tangents);

        auto t = std::make_shared<VertexBuffer>(numVertices, 3u, vertexComponentType);
        std::memset(t->data(), 0, t->sizeInBytes());

        auto b = std::make_shared<VertexBuffer>(numVertices, 3u, vertexComponentType);
        std::memset(b->data(), 0, t->sizeInBytes());

        for (const auto &primitiveSet : primitiveSets)
        {
            if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
            {
                switch (vertexComponentType)
                {
                case VertexComponentType::Single:
                    calculateTangentSpaceImpl<float>(drawArrays, *vertices, *texCoords, *t, *b);
                    break;
                case VertexComponentType::Double:
                    calculateTangentSpaceImpl<double>(drawArrays, *vertices, *texCoords, *t, *b);
                    break;
                default:
                    LOG_ERROR << "Vertex component type must be floating point";
                    break;
                }
            }
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
            {
                if (auto drawElementsBuffer = drawElements->asDrawElementsBuffer(); drawElementsBuffer)
                {
                    switch (vertexComponentType)
                    {
                    case VertexComponentType::Single:
                        calculateTangentSpaceImpl<float>(drawElementsBuffer, *vertices, *texCoords, *t, *b);
                        break;
                    case VertexComponentType::Double:
                        calculateTangentSpaceImpl<double>(drawElementsBuffer, *vertices, *texCoords, *t, *b);
                        break;
                    default:
                        LOG_ERROR << "Vertex component type must be floating point";
                        break;
                    }
                }
            }
        }

        switch (vertexComponentType)
        {
        case VertexComponentType::Single:
            orthogonalizeTangentSpaceImpl<float>(numVertices, *t, *b, *normals, *tangents);
            break;
        case VertexComponentType::Double:
            orthogonalizeTangentSpaceImpl<double>(numVertices, *t, *b, *normals, *tangents);
            break;
        default:
            LOG_ERROR << "Vertex component type must be floating point";
            break;
        }
    }

    BoundingBox calculateBoundingBox() const
    {
        BoundingBox result;

        const auto &vertexBuffers = mesh->vertexBuffers();
        const auto &primitiveSets = mesh->primitiveSets();

        if (!vertexBuffers.count(VertexAttribute::Position))
        {
            LOG_ERROR << "Mesh doesn't have Position vertex buffer";
            return result;
        }

        const auto& vertices = vertexBuffers.at(VertexAttribute::Position);
        const auto vertexComponentType = vertices->componentType();

        for (const auto &primitiveSet : primitiveSets)
        {
            if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
            {
                switch (vertexComponentType)
                {
                case VertexComponentType::Single:
                    result += calculateBoundingBoxImpl<float>(drawArrays, *vertices);
                    break;
                case VertexComponentType::Double:
                    result += calculateBoundingBoxImpl<double>(drawArrays, *vertices);
                    break;
                default:
                    LOG_ERROR << "Vertex component type must be floating point";
                    break;
                }
            }
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
            {
                if (auto drawElementsBuffer = drawElements->asDrawElementsBuffer(); drawElementsBuffer)
                {
                    switch (vertexComponentType)
                    {
                    case VertexComponentType::Single:
                        result += calculateBoundingBoxImpl<float>(drawElementsBuffer, *vertices);
                        break;
                    case VertexComponentType::Double:
                        result += calculateBoundingBoxImpl<double>(drawElementsBuffer, *vertices);
                        break;
                    default:
                        LOG_ERROR << "Vertex component type must be floating point";
                        break;
                    }
                }
            }
        }

        return result;
    }

    std::pair<uint32_t, uint32_t> addVertices(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices)
    {
        size_t numVertices = mesh->vertexBuffers().empty() ? 0u : mesh->vertexBuffers().begin()->second->numVertices();
        size_t addedVerticesCount = vertices.empty() ? 0u : static_cast<uint32_t>(vertices.begin()->second.size());

        for (const auto &[attrib, buffer] : mesh->vertexBuffers())
        {
            buffer->setNumVertices(numVertices + addedVerticesCount);

            if (auto bufferIt = vertices.find(attrib); bufferIt != vertices.end())
            {
                if (bufferIt->second.size() != addedVerticesCount)
                    LOG_CRITICAL << "Buffer have different size";

                if (auto transformIt = transforms.find(attrib); transformIt != transforms.end())
                {
                    const auto &transform = transformIt->second;
                    for (size_t i = 0u; i < addedVerticesCount; ++i)
                    {
                        const auto &v = bufferIt->second[i];
                        setVertexToBuffer(buffer, numVertices + i, glm::vec4(transform * glm::vec3(v), v.w));
                    }
                }
                else
                {
                    for (size_t i = 0u; i < addedVerticesCount; ++i)
                        setVertexToBuffer(buffer, numVertices + i, bufferIt->second[i]);
                }
            }
            else
            {
                const auto &defaultValue = defaultValues[castFromVertexAttribute(attrib)];
                for (size_t i = 0u; i < addedVerticesCount; ++i)
                    setVertexToBuffer(buffer, numVertices + i, defaultValue);
            }
        }

        return std::make_pair(numVertices, addedVerticesCount);
    }

    std::shared_ptr<Mesh> mesh;
    std::array<glm::vec4, numElementsVertexAttribute()> defaultValues;
    std::unordered_map<VertexAttribute, Transform> transforms;
};

AbstractPainter::AbstractPainter(const std::shared_ptr<Mesh>& mesh)
    : m_(std::make_unique<AbstractPainterPrivate>(mesh))
{
    setMesh(mesh);

    setDefaultValue(VertexAttribute::Position, glm::vec4(0.f, 0.f, 0.f, 1.f));
    setDefaultValue(VertexAttribute::Normal, glm::vec4(0.f, 0.f, 1.f, 0.f));
    setDefaultValue(VertexAttribute::TexCoords, glm::vec4(0.f, 0.f, 0.f, 1.f));
    setDefaultValue(VertexAttribute::Tangent, glm::vec4(1.f, 0.f, 0.f, 1.f));
    setDefaultValue(VertexAttribute::Color, glm::vec4(1.f, 1.f, 1.f, 1.f));

    setVertexTransform(Transform());
    setTexCoordsTransform(Transform());
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

const Transform &AbstractPainter::vertexTransform() const
{
    return m_->transforms[VertexAttribute::Position];
}

const Transform &AbstractPainter::normalTransform() const
{
    return m_->transforms[VertexAttribute::Normal];
}

void AbstractPainter::setVertexTransform(const Transform &vertexTransform)
{
    m_->transforms[VertexAttribute::Position] = vertexTransform;
    m_->transforms[VertexAttribute::Normal] = Transform::makeRotation(vertexTransform.rotation);
}

const Transform &AbstractPainter::texCoordsTransform() const
{
    return m_->transforms[VertexAttribute::TexCoords];
}

void AbstractPainter::setTexCoordsTransform(const Transform &texCoordsTransform)
{
    m_->transforms[VertexAttribute::TexCoords] = texCoordsTransform;
}

void AbstractPainter::drawArrays(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices, PrimitiveType primitiveType)
{
    if (!m_->mesh)
        LOG_CRITICAL << "Mesh can't be nullptr";

    auto firstCount = m_->addVertices(vertices);
    m_->mesh->attachPrimitiveSet(std::make_shared<DrawArrays>(primitiveType, firstCount.first, firstCount.second));
}

void AbstractPainter::drawElements(const std::unordered_map<VertexAttribute, const std::vector<glm::vec4>&> &vertices,
                                   PrimitiveType primitiveType,
                                   const std::vector<uint32_t> &indices,
                                   DrawElementsIndexType drawElemetsIndexType)
{
    if (!m_->mesh)
        LOG_CRITICAL << "Mesh can't be nullptr";

    auto firstCount = m_->addVertices(vertices);

    auto drawElemetsBuffer = std::make_shared<DrawElementsBuffer>(primitiveType,
                                                                  static_cast<uint32_t>(indices.size()),
                                                                  drawElemetsIndexType,
                                                                  firstCount.first);
    for (uint32_t i = 0u; i < static_cast<uint32_t>(indices.size()); ++i)
        setIndexToBuffer(drawElemetsBuffer, i, indices[i]);

    m_->mesh->attachPrimitiveSet(drawElemetsBuffer);
}

BoundingBox AbstractPainter::calculateBoundingBox() const
{
    return m_->calculateBoundingBox();
}

MeshPainter::MeshPainter(const std::shared_ptr<Mesh>& mesh)
    : AbstractPainter(mesh)
{
}

MeshPainter::~MeshPainter()
{
}

MeshPainter &MeshPainter::drawTriangle()
{
    drawElements({{VertexAttribute::Position, s_triangleVertices},
                  {VertexAttribute::Normal, s_triangleNormals},
                  {VertexAttribute::TexCoords, s_triangleTexCoords}},
                 PrimitiveType::Triangles,
                 s_triangleIndices,
                 toDrawElementsIndexType<decltype(s_triangleIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawPlane()
{
    drawElements({{VertexAttribute::Position, s_planeVertices},
                  {VertexAttribute::Normal, s_planeNormals},
                  {VertexAttribute::TexCoords, s_planeTexCoords}},
                 PrimitiveType::Triangles,
                 s_planeIndices,
                 toDrawElementsIndexType<decltype(s_planeIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawTetrahedron()
{
    drawElements({{VertexAttribute::Position, s_tetrahedronVertices},
                  {VertexAttribute::Normal, s_tetrahedronNormals},
                  {VertexAttribute::TexCoords, s_tetrahedronTexCoords}},
                 PrimitiveType::Triangles,
                 s_tetrahedronIndices,
                 toDrawElementsIndexType<decltype(s_tetrahedronIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawCube(const glm::vec3 &size)
{
    const glm::vec4 v4Size(size, 1.f);
    std::vector<glm::vec4> vertices(s_cubeVertices.size());
    for (size_t i = 0u; i < s_cubeVertices.size(); ++i)
        vertices[i] = s_cubeVertices[i] * v4Size;

    drawElements({{VertexAttribute::Position, vertices},
                  {VertexAttribute::Normal, s_cubeNormals},
                  {VertexAttribute::TexCoords, s_cubeTexCoords}},
                 PrimitiveType::Triangles,
                 s_cubeIndices,
                 toDrawElementsIndexType<decltype(s_cubeIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter& MeshPainter::drawFrustum(const glm::mat4x4& projectionMatrix)
{
    const auto projectionMatrixInverted = glm::inverse(projectionMatrix);

    std::vector<glm::vec4> vertices(s_cubeVertices.size());
    for (size_t i = 0u; i < s_cubeVertices.size(); ++i)
    {
        vertices[i] = projectionMatrixInverted * glm::vec4(2.f * glm::vec3(s_cubeVertices[i]), 1.f);
        vertices[i] /= vertices[i].w;
    }

    drawElements({ {VertexAttribute::Position, vertices},
                  {VertexAttribute::Normal, s_cubeNormals},
                  {VertexAttribute::TexCoords, s_cubeTexCoords} },
        PrimitiveType::Triangles,
        s_cubeIndices,
        toDrawElementsIndexType<decltype(s_cubeIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawSuzanne()
{
    drawElements({{VertexAttribute::Position, s_suzanneVertices},
                  {VertexAttribute::Normal, s_suzanneNormals},
                  {VertexAttribute::TexCoords, s_suzanneTexCoords}},
                 PrimitiveType::Triangles,
                 s_suzanneIndices,
                 toDrawElementsIndexType<decltype(s_suzanneIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawBunny()
{
    drawElements({{VertexAttribute::Position, s_bunnyVertices},
                  {VertexAttribute::Normal, s_bunnyNormals},
                  {VertexAttribute::TexCoords, s_bunnyTexCoords}},
                 PrimitiveType::Triangles,
                 s_bunnyIndices,
                 toDrawElementsIndexType<decltype(s_bunnyIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawTeapot()
{
    drawElements({{VertexAttribute::Position, s_teapotVertices},
                  {VertexAttribute::Normal, s_teapotNormals},
                  {VertexAttribute::TexCoords, s_teapotTexCoords}},
                 PrimitiveType::Triangles,
                 s_teapotIndices,
                 toDrawElementsIndexType<decltype(s_teapotIndices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawSphere(uint32_t segs)
{
    segs = glm::max(segs, 3u);

    std::vector<glm::vec4> vertices((segs+1u) * (segs+1u));
    std::vector<glm::vec4> normals((segs+1u) * (segs+1u));
    std::vector<glm::vec4> texCoords((segs+1u) * (segs+1u));
    std::vector<uint32_t> indices(6u * segs * segs);

    for (uint32_t a = 0u; a <= segs; ++a)
    {
        const float fA = static_cast<float>(a) / static_cast<float>(segs);
        const float angleA = glm::pi<float>() * (fA - .5f);
        const float sinA = glm::sin(angleA);
        const float cosA = glm::cos(angleA);

        for (uint32_t b = 0u; b <= segs; ++b)
        {
            const float fB = static_cast<float>(b) / static_cast<float>(segs);
            const float angleB = glm::two_pi<float>() * fB;
            const float sinB = glm::sin(angleB);
            const float cosB = glm::cos(angleB);

            const glm::vec3 p(cosA * sinB, sinA, cosA * cosB);

            vertices[a * (segs+1u) + b] = glm::vec4(p, 1.0f);
            normals[a * (segs+1u) + b] = glm::vec4(p, 0.0f);
            texCoords[a * (segs+1u) + b] = glm::vec4(fA, fB, 0.f, 1.f);
        }
    }

    for (uint32_t a = 0u; a < segs; ++a)
        for (uint32_t b = 0u; b < segs; ++b)
        {
            indices[6u * (a*segs + b) + 0u] = a * (segs+1u) + b;
            indices[6u * (a*segs + b) + 1u] = a * (segs+1u) + (b + 1u);
            indices[6u * (a*segs + b) + 2u] = (a+1u) * (segs+1u) + b;
            indices[6u * (a*segs + b) + 3u] = a * (segs+1u) + (b + 1u);
            indices[6u * (a*segs + b) + 4u] = (a+1u) * (segs+1u) + (b + 1u);
            indices[6u * (a*segs + b) + 5u] = (a+1u) * (segs+1u) + b;
        }

    drawElements({{VertexAttribute::Position, vertices},
                  {VertexAttribute::Normal, normals},
                  {VertexAttribute::TexCoords, texCoords}},
                 PrimitiveType::Triangles,
                 indices,
                 toDrawElementsIndexType<decltype(indices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawCone(float height, float halfAngle, uint32_t segs)
{
    segs = glm::max(segs, 3u);
    halfAngle = glm::tan(halfAngle);

    std::vector<glm::vec4> vertices((segs+1u) * 4u);
    std::vector<glm::vec4> normals((segs+1u) * 4u);
    std::vector<glm::vec4> texCoords((segs+1u) * 4u);
    std::vector<uint32_t> indices(6u *  segs);

    for (uint32_t a = 0u; a <= segs; ++a)
    {
        const auto fA = static_cast<float>(a) / static_cast<float>(segs);
        const auto angleA = glm::two_pi<float>() * fA;
        const auto sinA = glm::sin(angleA);
        const auto cosA = glm::cos(angleA);
        const auto p = glm::vec3(cosA * halfAngle, sinA * halfAngle, -1.0f) * height;
        const auto n = glm::normalize(glm::vec3(cosA, sinA, 1.0f));

        vertices[4u * a + 0u] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        vertices[4u * a + 1u] = glm::vec4(p, 1.0f);
        vertices[4u * a + 2u] = glm::vec4(p, 1.0f);
        vertices[4u * a + 3u] = glm::vec4(0.0f, 0.0f, -height, 1.0f);

        normals[4u * a + 0u] = glm::vec4(n, 0.0f);
        normals[4u * a + 1u] = glm::vec4(n, 0.0f);
        normals[4u * a + 2u] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        normals[4u * a + 3u] = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

        texCoords[4u * a + 0u] = glm::vec4(fA, 1.0f, 0.0f, 1.0f);
        texCoords[4u * a + 1u] = glm::vec4(fA, 0.0f, 0.0f, 1.0f);
        texCoords[4u * a + 2u] = glm::vec4(fA, 0.0f, 0.0f, 1.0f);
        texCoords[4u * a + 3u] = glm::vec4(fA, 1.0f, 0.0f, 1.0f);
    }

    for (uint32_t a = 0u; a < segs; ++a)
    {
        indices[6u * a + 0u] = (4u * (a)) + 0u;
        indices[6u * a + 1u] = (4u * (a)) + 1u;
        indices[6u * a + 2u] = (4u * (a+1u)) + 1u;
        indices[6u * a + 3u] = (4u * (a)) + 2u;
        indices[6u * a + 4u] = (4u * (a)) + 3u;
        indices[6u * a + 5u] = (4u * (a+1u)) + 2u;
    }

    drawElements({{VertexAttribute::Position, vertices},
                  {VertexAttribute::Normal, normals},
                  {VertexAttribute::TexCoords, texCoords}},
                 PrimitiveType::Triangles,
                 indices,
                 toDrawElementsIndexType<decltype(indices)::value_type>());
    if (m_->mesh->vertexBuffers().count(VertexAttribute::Tangent))
        m_->calculateTangents();
    return *this;
}

MeshPainter &MeshPainter::drawCamera()
{
    drawElements({{VertexAttribute::Position, s_cameraVertices}},
                 PrimitiveType::Lines,
                 s_cameraIndices,
                 toDrawElementsIndexType<decltype(s_cameraIndices)::value_type>());
    return *this;
}

MeshPainter &MeshPainter::drawBoundingBox()
{
    drawElements({{VertexAttribute::Position, s_boundingBoxVertices}},
                 PrimitiveType::Lines,
                 s_boundingBoxIndices,
                 toDrawElementsIndexType<decltype(s_boundingBoxIndices)::value_type>());
    return *this;
}

MeshPainter &MeshPainter::drawScreenQuad()
{
    drawArrays({{VertexAttribute::Position, s_screenQuadVertices}},
                 PrimitiveType::TriangleStrip);
    return *this;
}

}
}
