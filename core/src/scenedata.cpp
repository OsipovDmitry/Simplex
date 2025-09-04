#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/imagemanager.h>

#include <core/scene.h>
#include <core/drawable.h>
#include <core/material.h>
#include <core/mesh.h>
#include <core/graphicsrendererbase.h>
#include <core/igraphicswidget.h>
#include <core/graphicsengine.h>
#include <core/texturesmanager.h>
#include <core/background.h>

#include "scenedata.h"
#include "sceneprivate.h"
#include "drawableprivate.h"
#include "materialprivate.h"
#include "meshprivate.h"
#include "screenquad.h"
#include "backgroundprivate.h"

namespace simplex
{
namespace core
{

static const utils::RangeT<uint32_t> BonesCountRange{1u, 7u};
using BonesIDsUnderlyingType = uint32_t;

using VertexAttributeFormat = std::pair<utils::RangeT<uint32_t>, utils::VertexComponentType>;
static const VertexAttributeFormat PositionFormat{ {1u, 3u}, utils::toVertexComponentType<VertexDataDescription>()};
static const VertexAttributeFormat NormalFormat{ {1u, 3u}, utils::toVertexComponentType<VertexDataDescription>() };
static const VertexAttributeFormat TexCoordsFormat{ {1u, 3u}, utils::toVertexComponentType<VertexDataDescription>() };
static const VertexAttributeFormat BonesIDsFormat{ BonesCountRange, utils::toVertexComponentType<BonesIDsUnderlyingType>() };
static const VertexAttributeFormat BonesWeightsFormat{ BonesCountRange, utils::toVertexComponentType<VertexDataDescription>() };
static const VertexAttributeFormat TangentFormat{ {4u, 4u}, utils::toVertexComponentType<VertexDataDescription>() };
static const VertexAttributeFormat ColorFormat{ {1u, 4u}, utils::toVertexComponentType<VertexDataDescription>() };

static inline float materialOcclusionMapStrength(uint32_t flags) { return static_cast<float>((flags >> 0u) & 0xFF) / 255.f; }
static inline float materialNormalMapScale(uint32_t flags) { return static_cast<float>((flags >> 8u) & 0xFF) / 255.f; }
static inline uint32_t materialOcclusionSwizzle(uint32_t flags) { return static_cast<uint32_t>((flags >> 16u) & 0x3); }
static inline uint32_t materialRoughnessSwizzle(uint32_t flags) { return static_cast<uint32_t>((flags >> 18u) & 0x3); }
static inline uint32_t materialMetalnessSwizzle(uint32_t flags) { return static_cast<uint32_t>((flags >> 20u) & 0x3); }
static inline bool isMaterialLighted(uint32_t flags) { return (flags >> 22u) & 0x1; }
static inline bool isMaterialShadowed(uint32_t flags) { return (flags >> 23u) & 0x1; }
static inline bool isMaterialTransparent(uint32_t flags) { return (flags >> 24u) & 0x1; }
static inline uint32_t makeMaterialFlags(
    float occlusionMapStrength,
    float normalMapScale,
    const glm::u32vec3& ORMSwizzleMask,
    bool isLighted,
    bool isShadowed,
    bool isTransparent)
{
    uint32_t result = 0u;
    result |= (static_cast<uint32_t>(glm::clamp(occlusionMapStrength, 0.f, 1.f) * 255.f) & 0xFF) << 0u;
    result |= (static_cast<uint32_t>(glm::clamp(normalMapScale, 0.f, 1.f) * 255.f) & 0xFF) << 8u;
    result |= (ORMSwizzleMask.r & 0x3u) << 16u;
    result |= (ORMSwizzleMask.g & 0x3u) << 18u;
    result |= (ORMSwizzleMask.b & 0x3u) << 20u;
    result |= ((isLighted ? 1u : 0u) & 0x1u) << 22u;
    result |= ((isShadowed ? 1u : 0u) & 0x1u) << 23u;
    result |= ((isTransparent ? 1u : 0u) & 0x1u) << 24u;
    return result;
}

static inline uint32_t makeMeshFlags(
    uint32_t numPositionComponents,
    uint32_t numNormalComponents,
    uint32_t numTexCoordsComponents,
    uint32_t numBones,
    bool hasTangent,
    uint32_t numColorComponents)
{
    uint32_t result = 0u;
    result |= (numPositionComponents & 0x3u) << 0u;
    result |= (numNormalComponents & 0x3u) << 2u;
    result |= (numTexCoordsComponents & 0x3u) << 4u;
    result |= (numBones & 0x7u) << 6u;
    result |= ((hasTangent ? 1u : 0u) & 0x1u) << 9u;
    result |= (numColorComponents & 0x7u) << 10u;
    return result;
}

static inline bool isMaterialTransparent(const std::shared_ptr<const Material>& material)
{
    if (material->baseColor().a < 1.f - utils::epsilon<float>()) return true;

    if (auto baseColorMap = material->materialMap(MaterialMapTarget::BaseColor))
    {
        std::shared_ptr<const utils::Image> baseColorImage;
        if (auto path = baseColorMap->asFilesystemPath(); !path.empty())
        {
            baseColorImage = utils::ImageManager::instance().loadOrGetDescription(path);
        }
        else if (auto image = baseColorMap->asImage(); image)
        {
            baseColorImage = image;
        }

        if (baseColorImage && (baseColorImage->numComponents() == 4u))
            return true;
    }

    // other future conditions like opacity map

    return false;
}

static inline bool isMeshTransparent(const std::shared_ptr<const Mesh>& mesh)
{
    if (auto utilsMesh = mesh->mesh())
    {
        const auto& vertexBuffers = utilsMesh->vertexBuffers();
        if (auto it = vertexBuffers.find(utils::VertexAttribute::Color); it != vertexBuffers.end())
            if (it->second->numComponents() == 4u)
                return true;
    }

    return false;
}

static glm::mat3x4 makeDrawDataNormalMatrixAndDrawableOffset(const glm::mat4x4& modelMatrix, utils::IDsGenerator::value_type ID)
{
    glm::mat3x4 result = glm::mat3x4(glm::inverseTranspose(modelMatrix));
    result[0u][3u] = glm::uintBitsToFloat(ID);
    return result;
}

static glm::vec4 makeBackgroundEnvironmentColorAndBlurPower(const glm::vec3& environmentColor, float blurPower)
{
    return glm::vec4(environmentColor, blurPower);
}

MeshHandler::MeshHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type id)
    : m_sceneData(sceneData)
    , m_mesh(mesh)
    , m_ID(id)
{}

MeshHandler::~MeshHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto mesh = m_mesh.lock())
            sceneData->removeMesh(mesh, m_ID);
}

std::weak_ptr<const Mesh>& MeshHandler::mesh()
{
    return m_mesh;
}

utils::IDsGenerator::value_type MeshHandler::ID() const
{
    return m_ID;
}

MaterialMapHandler::MaterialMapHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const MaterialMap>& materialMap, utils::IDsGenerator::value_type id)
    : m_sceneData(sceneData)
    , m_materialMap(materialMap)
    , m_ID(id)
{}

MaterialMapHandler::~MaterialMapHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto materialMap = m_materialMap.lock())
            sceneData->removeMaterialMap(materialMap, m_ID);
}

std::weak_ptr<const MaterialMap>& MaterialMapHandler::materialMap()
{
    return m_materialMap;
}

utils::IDsGenerator::value_type& MaterialMapHandler::ID()
{
    return m_ID;
}

MaterialHandler::MaterialHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Material>& material, utils::IDsGenerator::value_type id)
    : m_sceneData(sceneData)
    , m_material(material)
    , m_ID(id)
{}

MaterialHandler::~MaterialHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto material = m_material.lock())
            sceneData->removeMaterial(material, m_ID);
}

std::weak_ptr<const Material>& MaterialHandler::material()
{
    return m_material;
}

utils::IDsGenerator::value_type MaterialHandler::ID() const
{
    return m_ID;
}

DrawableHandler::DrawableHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Drawable>& drawable, utils::IDsGenerator::value_type id)
    : m_sceneData(sceneData)
    , m_drawable(drawable)
    , m_ID(id)
{
}

DrawableHandler::~DrawableHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto drawable = m_drawable.lock())
        sceneData->removeDrawable(drawable, m_ID);
}

std::weak_ptr<const Drawable>& DrawableHandler::drawable()
{
    return m_drawable;
}

utils::IDsGenerator::value_type DrawableHandler::ID() const
{
    return m_ID;
}

DrawDataHandler::DrawDataHandler(const std::weak_ptr<SceneData>& sceneData, size_t ID)
    : m_sceneData(sceneData)
    , m_ID(ID)
{
}

DrawDataHandler::~DrawDataHandler()
{
    if (auto sceneData = m_sceneData.lock())
        sceneData->removeDrawData(m_ID);
}

size_t DrawDataHandler::ID() const
{
    return m_ID;
}

BackgroundHandler::BackgroundHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Background>& background, size_t ID)
    : m_sceneData(sceneData)
    , m_background(background)
    , m_ID(ID)
{
}

BackgroundHandler::~BackgroundHandler()
{
    if (auto sceneData = m_sceneData.lock())
        sceneData->removeBackground(m_ID);
}

std::weak_ptr<const Background>& BackgroundHandler::background()
{
    return m_background;
}

size_t BackgroundHandler::ID() const
{
    return m_ID;
}

SceneData::SceneData()
{
    m_vertexDataBuffer = VertexDataBuffer::element_type::create();
    m_elementsBuffer = ElementsBuffer::element_type::create();
    m_meshesBuffer = MeshesBuffer::element_type::create();
    m_materialMapsBuffer = MaterialMapsBuffer::element_type::create();
    m_materialsBuffer = MaterialsBuffer::element_type::create();
    m_drawablesBuffer = DrawablesBuffer::element_type::create();
    m_drawDataBuffer = DrawDataBuffer::element_type::create();
    m_backgroundsBuffer = BackgroundsBuffer::element_type::create();
    m_opaqueDrawDataCommandsBuffer = DrawIndirectArraysCommandsBuffer::element_type::create();
    m_transparentDrawDataCommandsBuffer = DrawIndirectArraysCommandsBuffer::element_type::create();
    m_backgroundsCommandsBuffer = DrawIndirectArraysCommandsBuffer::element_type::create();

    m_materialMaps = std::make_shared<MaterialMaps::element_type>();
}

SceneData::~SceneData() = default;

utils::IDsGenerator::value_type SceneData::addMesh(const std::shared_ptr<const Mesh>& mesh)
{
    auto result = utils::IDsGenerator::last();

    if (!mesh)
    {
        // do nothing
    }
    else if (auto it = m_meshes.find(mesh); it != m_meshes.end())
    {
        if (it->second.expired())
            LOG_CRITICAL << "Scene has expired mesh";

        result = it->second.lock()->ID();
    }
    else
    {
        result = m_meshIDsGenerator.generate();

        auto handler = std::make_shared<MeshHandler>(weak_from_this(), mesh, result);
        mesh->m().handlers().insert({ shared_from_this(), handler });
        m_meshes.insert({ mesh, handler });

        addMeshData(mesh, result);
    }

    return result;
}

void SceneData::removeMesh(const std::shared_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type ID)
{
    if (auto it = m_meshes.find(mesh); it == m_meshes.end())
    {
        LOG_CRITICAL << "No found mesh in scene to remove";
        return;
    }
    else
    {
        removeMeshData(mesh, ID);
        m_meshes.erase(mesh);
        m_meshIDsGenerator.clear(ID);
    }
}

void SceneData::onMeshChanged(const std::shared_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type ID)
{
    if (!mesh)
    {
        LOG_CRITICAL << "Mesh can't be nullptr";
        return;
    }

    if (auto it = m_meshes.find(mesh); it == m_meshes.end())
    {
        LOG_CRITICAL << "No found mesh in scene to change";
        return;
    }

    removeMeshData(mesh, ID);
    addMeshData(mesh, ID);
}

void SceneData::addMeshData(const std::shared_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type ID)
{
    auto utilsMesh = mesh->mesh();
    if (!utilsMesh)
    {
        LOG_CRITICAL << "Mesh can't be nullptr";
        return;
    }

    const auto& vertexBuffers = utilsMesh->vertexBuffers();

    const size_t numVertices = vertexBuffers.empty() ? 0u : vertexBuffers.begin()->second->numVertices();
    size_t vertexStride = 0u; // in floats
    for (auto const& [attrib, buffer] : vertexBuffers)
    {
        if (buffer->numVertices() != numVertices)
        {
            LOG_CRITICAL << "Buffers have different size";
            return;
        }

        vertexStride += buffer->numComponents();
    }

    const uint32_t numVertexData = numVertices * vertexStride;
    std::vector<VertexDataDescription> vertexData(numVertexData, static_cast<VertexDataDescription>(0));

    size_t relativeOffset = 0u;
    uint32_t numPositionComponents = 0u;
    uint32_t numNormalComponents = 0u;
    uint32_t numTexCoordsComponents = 0u;
    bool hasTangent = false;
    uint32_t numBones = 0u;
    uint32_t numColorComponents = 0u;

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Position); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!PositionFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != PositionFormat.second))
        {
            buffer = buffer->converted(PositionFormat.first.farValue(), PositionFormat.second);
        }

        numPositionComponents = buffer->numComponents();
        for (size_t v = 0u; v < numVertices; ++v)
        {
            const auto* vertex = reinterpret_cast<const VertexDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < numPositionComponents; ++c)
                vertexData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += numPositionComponents;
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Normal); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!NormalFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != NormalFormat.second))
        {
            buffer = buffer->converted(NormalFormat.first.farValue(), NormalFormat.second);
        }

        numNormalComponents = buffer->numComponents();
        for (size_t v = 0u; v < numVertices; ++v)
        {
            const auto* vertex = reinterpret_cast<const VertexDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < numNormalComponents; ++c)
                vertexData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += numNormalComponents;
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::TexCoords); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!TexCoordsFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != TexCoordsFormat.second))
        {
            buffer = buffer->converted(TexCoordsFormat.first.farValue(), TexCoordsFormat.second);
        }

        numTexCoordsComponents = buffer->numComponents();
        for (size_t v = 0u; v < numVertices; ++v)
        {
            const auto* vertex = reinterpret_cast<const VertexDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < numTexCoordsComponents; ++c)
                vertexData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += numTexCoordsComponents;
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Tangent);
        (it != vertexBuffers.end()) && (numNormalComponents > 0u))
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (TangentFormat.first.isInside(buffer->numComponents())) // only 4-components tangent is suitable
        {
            if (buffer->componentType() != TangentFormat.second)
            {
                buffer = buffer->converted(buffer->numComponents(), TangentFormat.second);
            }

            const size_t numComponents = buffer->numComponents();
            hasTangent = numComponents != 0u;
            for (size_t v = 0u; v < numVertices; ++v)
            {
                const auto* vertex = reinterpret_cast<const VertexDataDescription*>(buffer->vertex(v));
                for (size_t c = 0u; c < numComponents; ++c)
                    vertexData[vertexStride * v + relativeOffset + c] = vertex[c];
            }

            relativeOffset += numComponents;
        }
    }

    if (vertexBuffers.count(utils::VertexAttribute::BonesIDs) &&
        vertexBuffers.count(utils::VertexAttribute::BonesWeights))
    {
        std::shared_ptr<utils::VertexBuffer> bonesIDsBuffer = vertexBuffers.at(utils::VertexAttribute::BonesIDs);
        std::shared_ptr<utils::VertexBuffer> bonesWeightsBuffer = vertexBuffers.at(utils::VertexAttribute::BonesWeights);

        numBones = bonesIDsBuffer->numComponents();
        if (numBones != bonesWeightsBuffer->numComponents())
        {
            LOG_CRITICAL << "Bones IDs and weights buffers must have equal number of components";
            return;
        }

        numBones = glm::min(numBones, BonesCountRange.farValue());

        if (!BonesIDsFormat.first.isInside(bonesIDsBuffer->numComponents()) ||
            (bonesIDsBuffer->componentType() != BonesIDsFormat.second))
        {
            bonesIDsBuffer = bonesIDsBuffer->converted(numBones, BonesIDsFormat.second);
        }

        if (!BonesWeightsFormat.first.isInside(bonesWeightsBuffer->numComponents()) ||
            (bonesWeightsBuffer->componentType() != BonesWeightsFormat.second))
        {
            bonesWeightsBuffer =
                bonesWeightsBuffer->converted(numBones, BonesWeightsFormat.second);
        }

        for (size_t v = 0u; v < numVertices; ++v)
        {
            const auto* IDs = reinterpret_cast<const BonesIDsUnderlyingType*>(bonesIDsBuffer->vertex(v));
            const auto* weights = reinterpret_cast<const VertexDataDescription*>(bonesWeightsBuffer->vertex(v));
            for (uint32_t c = 0u; c < numBones; ++c)
            {
                vertexData[vertexStride * v + relativeOffset + 2u * c + 0u] = glm::uintBitsToFloat(IDs[c]);
                vertexData[vertexStride * v + relativeOffset + 2u * c + 1u] = weights[c];
            }
        }

        relativeOffset += 2u * numBones;
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Color); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!ColorFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != ColorFormat.second))
        {
            buffer = buffer->converted(ColorFormat.first.farValue(), ColorFormat.second);
        }

        numColorComponents = buffer->numComponents();
        for (size_t v = 0u; v < numVertices; ++v)
        {
            const auto* vertex = reinterpret_cast<const VertexDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < numColorComponents; ++c)
                vertexData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += numColorComponents;
    }

    uint32_t vertexDataOffset = m_vertexDataBuffer->size();
    m_vertexDataBuffer->insert(vertexDataOffset, vertexData.size(), vertexData.data());

    static constexpr auto s_indexType = utils::toDrawElementsIndexType<ElementsDescription>();
    static_assert(s_indexType != utils::DrawElementsIndexType::Count);

    static constexpr auto s_primitiveType = utils::PrimitiveType::Triangles;

    uint32_t elementsOffset = m_elementsBuffer->size();
    uint32_t numElements = 0u;

    for (const auto& primitiveSet : utilsMesh->primitiveSets())
    {
        std::shared_ptr<utils::DrawElementsBuffer> buffer;
        if (auto drawArrays = primitiveSet->asDrawArrays())
        {
            buffer = drawArrays->convertToElements(s_indexType);
        }
        else if (auto drawElements = primitiveSet->asDrawElements())
        {
            buffer = drawElements->asDrawElementsBuffer();
        }

        if (!buffer)
        {
            LOG_WARNING << "Failed to cast to draw elemtns buffer";
            continue;
        }

        if (buffer->indexType() != s_indexType)
            buffer = buffer->convertedToIndexType(s_indexType);

        if (buffer->primitiveType() != s_primitiveType)
            buffer = buffer->convertedToTriangles();

        if (buffer->baseVertex())
            buffer = buffer->appliedBaseVertex();

        m_elementsBuffer->insert(
            m_elementsBuffer->size(), buffer->numIndices(), reinterpret_cast<ElementsDescription*>(buffer->data()));

        numElements += buffer->numIndices();
    }

    if (m_meshesBuffer->size() <= ID)
        m_meshesBuffer->resize(ID + 1u);

    m_meshesBuffer->set(ID, {
        vertexDataOffset,
        numVertexData,
        elementsOffset,
        numElements,
        makeMeshFlags(
            numPositionComponents,
            numNormalComponents,
            numTexCoordsComponents,
            numBones,
            hasTangent,
            numColorComponents)
        });
}

void SceneData::removeMeshData(const std::shared_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type ID)
{
    auto description = m_meshesBuffer->get(ID);

    for (size_t i = 0u; i < m_meshesBuffer->size(); ++i)
    {
        auto meshDescription = m_meshesBuffer->get(i);

        if (meshDescription.vertexDataOffset >= description.vertexDataOffset)
            meshDescription.vertexDataOffset -= description.numVertexData;

        if (meshDescription.elementsOffset >= description.elementsOffset)
            meshDescription.elementsOffset -= description.numElements;

        m_meshesBuffer->set(i, meshDescription);
    }

    m_vertexDataBuffer->erase(description.vertexDataOffset, description.numVertexData);
    m_elementsBuffer->erase(description.elementsOffset, description.numElements);

    m_meshesBuffer->erase(ID, 1u);
}

utils::IDsGenerator::value_type SceneData::addMaterialMap(const std::shared_ptr<const MaterialMap>& materialMap)
{
    auto result = utils::IDsGenerator::last();

    if (!materialMap)
    {
        // do nothing
    }
    else if (materialMap->isEmpty())
    {
        // do nothing
    }
    else if (auto it = m_materialMaps->find(materialMap); it != m_materialMaps->end())
    {
        if (it->second.expired())
            LOG_CRITICAL << "Scene has expired material map";

        result = it->second.lock()->ID();
    }
    else
    {
        auto& materialMapPrivate = materialMap->m();

        result = m_materialMapIDsGenerator.generate();

        auto handler = std::make_shared<MaterialMapHandler>(weak_from_this(), materialMap, result);
        materialMapPrivate.handlers().insert({ shared_from_this(), handler });
        m_materialMaps->insert({ materialMap, handler });

        onMaterialMapChanged(materialMap, result);
    }

    return result;
}

void SceneData::removeMaterialMap(const std::shared_ptr<const MaterialMap>& materialMap, utils::IDsGenerator::value_type ID)
{
    if (auto it = m_materialMaps->find(materialMap); it == m_materialMaps->end())
    {
        LOG_CRITICAL << "No found material map in scene to remove";
        return;
    }
    else
    {
        m_materialMapsHandles.erase(ID);
        m_materialMaps->erase(materialMap);
        m_materialMapIDsGenerator.clear(ID);
    }
}

void SceneData::onMaterialMapChanged(const std::shared_ptr<const MaterialMap>& materialMap, utils::IDsGenerator::value_type ID)
{
    if (!materialMap)
    {
        LOG_CRITICAL << "Material map can't be nullptr";
        return;
    }

    if (auto it = m_materialMaps->find(materialMap); it == m_materialMaps->end())
    {
        LOG_CRITICAL << "No found material map in scene to change";
        return;
    }

    auto graphicsRenderer = graphics::RendererBase::current();
    if (!graphicsRenderer)
    {
        LOG_CRITICAL << "No current graphics renderer";
        return;
    }

    auto graphicsWidget = graphicsRenderer->widget();
    if (!graphicsWidget)
    {
        LOG_CRITICAL << "Graphics widget can't be nullptr";
        return;
    }

    auto graphicsEngine = graphicsWidget->graphicsEngine();
    if (!graphicsEngine)
    {
        LOG_CRITICAL << "Graphics engine can't be nullptr";
        return;
    }

    auto texturesManager = graphicsEngine->texturesManager();
    if (!texturesManager)
    {
        LOG_CRITICAL << "Textures manager can't be nullptr";
        return;
    }

    graphics::PTexture texture;
    if (auto path = materialMap->asFilesystemPath(); !path.empty())
    {
        texture = texturesManager->loadOrGetTexture(path);
    }
    else if (auto image = materialMap->asImage(); image)
    {
        texture = texturesManager->loadOrGetTexture(image);
    }

    if (!texture)
    {
        LOG_CRITICAL << "Failed to load texture";
        return;
    }

    auto textureHandle = graphicsRenderer->createTextureHandle(texture);
    m_materialMapsHandles.insert({ ID, textureHandle });

    if (m_materialMapsBuffer->size() <= ID)
        m_materialMapsBuffer->resize(ID + 1u);

    m_materialMapsBuffer->set(ID, { textureHandle->handle() });
}

utils::IDsGenerator::value_type SceneData::addMaterial(const std::shared_ptr<const Material>& material)
{
    auto result = utils::IDsGenerator::last();

    if (!material)
    {
        // do nothing
    }
    else if (auto it = m_materials.find(material); it != m_materials.end())
    {
        if (it->second.expired())
            LOG_CRITICAL << "Scene has expired material";

        result = it->second.lock()->ID();
    }
    else
    {
        auto& materialPrivate = material->m();

        result = m_materialIDsGenerator.generate();

        auto handler = std::make_shared<MaterialHandler>(weak_from_this(), material, result);
        materialPrivate.handlers().insert({ shared_from_this(), handler });
        m_materials.insert({ material, handler });

        onMaterialChanged(material, result);
    }

    return result;
}

void SceneData::removeMaterial(const std::shared_ptr<const Material>& material, utils::IDsGenerator::value_type ID)
{
    if (auto it = m_materials.find(material); it == m_materials.end())
    {
        LOG_CRITICAL << "No found material in scene to remove";
        return;
    }
    else
    {
        m_materials.erase(material);
        m_materialIDsGenerator.clear(ID);
    }
}

void SceneData::onMaterialChanged(const std::shared_ptr<const Material>& material, utils::IDsGenerator::value_type ID)
{
    if (!material)
    {
        LOG_CRITICAL << "Material can't be nullptr";
        return;
    }

    if (auto it = m_materials.find(material); it == m_materials.end())
    {
        LOG_CRITICAL << "No found material in scene to change";
        return;
    }

    if (m_materialsBuffer->size() <= ID)
        m_materialsBuffer->resize(ID + 1u);

    auto& materialPrivate = material->m();
    m_materialsBuffer->set(ID, {
        addMaterialMap(materialPrivate.maps()[castFromMaterialMapTarget(MaterialMapTarget::BaseColor)]),
        addMaterialMap(materialPrivate.maps()[castFromMaterialMapTarget(MaterialMapTarget::Emission)]),
        addMaterialMap(materialPrivate.maps()[castFromMaterialMapTarget(MaterialMapTarget::Occlusion)]),
        addMaterialMap(materialPrivate.maps()[castFromMaterialMapTarget(MaterialMapTarget::Roughness)]),
        addMaterialMap(materialPrivate.maps()[castFromMaterialMapTarget(MaterialMapTarget::Metalness)]),
        addMaterialMap(materialPrivate.maps()[castFromMaterialMapTarget(MaterialMapTarget::Normal)]),
        materialPrivate.roughness(),
        materialPrivate.metalness(),
        materialPrivate.baseColor(),
        glm::vec4(
            materialPrivate.emission(),
            glm::uintBitsToFloat(makeMaterialFlags(
                materialPrivate.occlusionMapStrength(),
                materialPrivate.normalMapScale(),
                materialPrivate.ORMSwizzleMask(),
                materialPrivate.isLighted(),
                materialPrivate.isShadowed(),
                isMaterialTransparent(material))))
        });
}

utils::IDsGenerator::value_type SceneData::addDrawable(const std::shared_ptr<const Drawable>& drawable)
{
    auto result = utils::IDsGenerator::last();

    if (!drawable)
    {
        // do nothing
    }
    else if (auto it = m_drawables.find(drawable); it != m_drawables.end())
    {
        if (it->second.expired())
            LOG_CRITICAL << "Scene has expired drawable";

        result = it->second.lock()->ID();
    }
    else
    {
        auto& drawablePrivate = drawable->m();

        result = m_drawableIDsGenerator.generate();

        auto handler = std::make_shared<DrawableHandler>(weak_from_this(), drawable, result);
        drawablePrivate.handlers().insert({ shared_from_this(), handler });
        m_drawables.insert({ drawable, handler });

        onDrawableChanged(drawable, result);
    }

    return result;
}

void SceneData::removeDrawable(const std::shared_ptr<const Drawable>& drawable, utils::IDsGenerator::value_type ID)
{
    if (auto it = m_drawables.find(drawable); it == m_drawables.end())
    {
        LOG_CRITICAL << "No found drawable in scene to remove";
        return;
    }
    else
    {
        m_drawables.erase(drawable);
        m_drawableIDsGenerator.clear(ID);
    }
}

void SceneData::onDrawableChanged(const std::shared_ptr<const Drawable>& drawable, utils::IDsGenerator::value_type ID)
{
    if (!drawable)
    {
        LOG_CRITICAL << "Drawable can't be nullptr";
        return;
    }

    if (auto it = m_drawables.find(drawable); it == m_drawables.end())
    {
        LOG_CRITICAL << "No found drawable in scene to change";
        return;
    }

    if (m_drawablesBuffer->size() <= ID)
        m_drawablesBuffer->resize(ID + 1u);

    auto& drawablePrivate = drawable->m();
    m_drawablesBuffer->set(ID, {
        addMesh(drawablePrivate.mesh()),
        addMaterial(drawablePrivate.material())
        });
}

std::shared_ptr<DrawDataHandler> SceneData::addDrawData(const std::shared_ptr<const Drawable>& drawable, const glm::mat4x4& modelMatrix)
{
    if (!drawable)
    {
        LOG_CRITICAL << "Failed to add draw data";
        return nullptr;
    }

    const auto ID = m_drawDataIDsGenerator.generate();

    auto result = std::make_shared<DrawDataHandler>(weak_from_this(), ID);

    if (m_drawDataHandlers.size() <= ID)
        m_drawDataHandlers.resize(ID + 1u);

    m_drawDataHandlers[ID] = result;

    onDrawDataChanged(drawable, modelMatrix, ID);

    return result;
}

void SceneData::removeDrawData(utils::IDsGenerator::value_type ID)
{
    if (ID >= m_drawDataHandlers.size())
    {
        LOG_CRITICAL << "No found draw data in scene to remove";
        return;
    }

    m_drawDataHandlers[ID].reset();
    m_drawDataBuffer->set(ID, {
        glm::mat4x4(1.f),
        makeDrawDataNormalMatrixAndDrawableOffset(glm::mat4x4(1.f), utils::IDsGenerator::last())
        });
    m_drawDataIDsGenerator.clear(ID);
}

void SceneData::onDrawDataChanged(const std::shared_ptr<const Drawable>& drawable, const glm::mat4x4& modelMatrix, utils::IDsGenerator::value_type ID)
{
    if (!drawable)
    {
        LOG_CRITICAL << "Drawable can't be nullptr";
        return;
    }

    if (ID >= m_drawDataHandlers.size())
    {
        LOG_CRITICAL << "No found draw data in scene to change";
        return;
    }

    if (m_drawDataBuffer->size() <= ID)
    {
        m_drawDataBuffer->resize(ID + 1u);
        m_drawDataBuffer->setReservedData({ static_cast<uint32_t>(m_drawDataBuffer->size()), {0u, 0u, 0u} });
    }

    if (m_opaqueDrawDataCommandsBuffer->size() <= ID)
    {
        m_opaqueDrawDataCommandsBuffer->resize(ID + 1u);
        m_opaqueDrawDataCommandsBuffer->setReservedData({ static_cast<uint32_t>(m_opaqueDrawDataCommandsBuffer->size()), {0u, 0u, 0u} });
    }

    if (m_transparentDrawDataCommandsBuffer->size() <= ID)
    {
        m_transparentDrawDataCommandsBuffer->resize(ID + 1u);
        m_transparentDrawDataCommandsBuffer->setReservedData({ static_cast<uint32_t>(m_transparentDrawDataCommandsBuffer->size()), {0u, 0u, 0u} });
    }

    m_drawDataBuffer->set(ID, {
        modelMatrix,
        makeDrawDataNormalMatrixAndDrawableOffset(modelMatrix, addDrawable(drawable))
        });
}

void SceneData::addBackground(const std::shared_ptr<const Background>& background)
{
    if (!background)
    {
        LOG_CRITICAL << "Failed to add background";
        return;
    }

    const auto ID = m_backgroundIDsGenerator.generate();

    auto handler = std::make_shared<BackgroundHandler>(weak_from_this(), background, ID);
    background->m().handler() = { shared_from_this(), handler };

    if (m_backgroundsHandlers.size() <= ID)
        m_backgroundsHandlers.resize(ID + 1u);

    m_backgroundsHandlers [ID] = handler;

    onBackgroundChanged(background, ID);
}

void SceneData::removeBackground(size_t ID)
{
    if (ID >= m_backgroundsHandlers.size())
    {
        LOG_CRITICAL << "No found background in scene to remove";
        return;
    }

    m_backgroundsHandlers[ID].reset();
    m_backgroundsBuffer->set(ID, {
        utils::IDsGenerator::last(),
        utils::IDsGenerator::last(),
        makeBackgroundEnvironmentColorAndBlurPower(glm::vec3(0.f), 0.f)
        });
    m_backgroundIDsGenerator.clear(ID);
}

void SceneData::onBackgroundChanged(const std::shared_ptr<const Background>& background, size_t ID)
{
    if (!background)
    {
        LOG_CRITICAL << "Background can't be nullptr";
        return;
    }

    if (ID >= m_backgroundsHandlers.size())
    {
        LOG_CRITICAL << "No found background in scene to change";
        return;
    }

    if (m_backgroundsBuffer->size() <= ID)
    {
        m_backgroundsBuffer->resize(ID + 1u);
        m_backgroundsBuffer->setReservedData({ static_cast<uint32_t>(m_backgroundsBuffer->size()), {0u, 0u, 0u} });
    }

    if (m_backgroundsCommandsBuffer->size() <= ID)
    {
        m_backgroundsCommandsBuffer->resize(ID + 1u);
        m_backgroundsCommandsBuffer->setReservedData({ static_cast<uint32_t>(m_backgroundsCommandsBuffer->size()), {0u, 0u, 0u} });
    }

    m_backgroundsBuffer->set(ID, {
        addMesh(ScreenQuad::instance()),
        addMaterialMap(background->environmentMap()),
        makeBackgroundEnvironmentColorAndBlurPower( background->environmentColor(), background->blurPower())
        });
}

VertexDataBuffer& SceneData::vertexDataBuffer()
{
    return m_vertexDataBuffer;
}

ElementsBuffer& SceneData::elementsBuffer()
{
    return m_elementsBuffer;
}

MeshesBuffer& SceneData::meshesBuffer()
{
    return m_meshesBuffer;
}

MaterialMapsBuffer& SceneData::materialMapsBuffer()
{
    return m_materialMapsBuffer;
}

MaterialsBuffer& SceneData::materialsBuffer()
{
    return m_materialsBuffer;
}

DrawablesBuffer& SceneData::drawablesBuffer()
{
    return m_drawablesBuffer;
}

DrawDataBuffer& SceneData::drawDataBuffer()
{
    return m_drawDataBuffer;
}

BackgroundsBuffer& SceneData::backgroundsBuffer()
{
    return m_backgroundsBuffer;
}

DrawIndirectArraysCommandsBuffer& SceneData::opaqueDrawDataCommandsBuffer()
{
    return m_opaqueDrawDataCommandsBuffer;
}

DrawIndirectArraysCommandsBuffer& SceneData::transparentDrawDataCommandsBuffer()
{
    return m_transparentDrawDataCommandsBuffer;;
}

DrawIndirectArraysCommandsBuffer& SceneData::backgroundsCommandsBuffer()
{
    return m_backgroundsCommandsBuffer;
}

}
}
