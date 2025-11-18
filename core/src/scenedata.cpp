#include <utils/meshpainter.h>
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

ResourceHandler::ResourceHandler(const std::weak_ptr<SceneData>& sceneData)
    : m_sceneData(sceneData)
{
}

ResourceHandler::~ResourceHandler() = default;

std::weak_ptr<SceneData>& ResourceHandler::sceneData()
{
    return m_sceneData;
}

MeshHandler::MeshHandler(
    const std::weak_ptr<SceneData>& sceneData,
    const std::weak_ptr<const Mesh>& mesh,
    utils::IDsGenerator::value_type ID)
    : ResourceHandler(sceneData)
    , m_mesh(mesh)
    , m_ID(ID)
{}

MeshHandler::~MeshHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto mesh = m_mesh.lock())
            sceneData->removeMesh(mesh);
}

std::weak_ptr<const Mesh>& MeshHandler::mesh()
{
    return m_mesh;
}

utils::IDsGenerator::value_type MeshHandler::ID() const
{
    return m_ID;
}

MaterialMapHandler::MaterialMapHandler(
    const std::weak_ptr<SceneData>& sceneData,
    const std::weak_ptr<const MaterialMap>& materialMap,
    utils::IDsGenerator::value_type ID)
    : ResourceHandler(sceneData)
    , m_materialMap(materialMap)
    , m_ID(ID)
{}

MaterialMapHandler::~MaterialMapHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto materialMap = m_materialMap.lock())
            sceneData->removeMaterialMap(materialMap);
}

std::weak_ptr<const MaterialMap>& MaterialMapHandler::materialMap()
{
    return m_materialMap;
}

utils::IDsGenerator::value_type& MaterialMapHandler::ID()
{
    return m_ID;
}

MaterialHandler::MaterialHandler(
    const std::weak_ptr<SceneData>& sceneData,
    const std::weak_ptr<const Material>& material,
    utils::IDsGenerator::value_type ID)
    : ResourceHandler(sceneData)
    , m_material(material)
    , m_ID(ID)
{}

MaterialHandler::~MaterialHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto material = m_material.lock())
            sceneData->removeMaterial(material);
}

std::weak_ptr<const Material>& MaterialHandler::material()
{
    return m_material;
}

utils::IDsGenerator::value_type MaterialHandler::ID() const
{
    return m_ID;
}

DrawableHandler::DrawableHandler(
    const std::weak_ptr<SceneData>& sceneData,
    const std::weak_ptr<const Drawable>& drawable,
    utils::IDsGenerator::value_type ID)
    : ResourceHandler(sceneData)
    , m_drawable(drawable)
    , m_ID(ID)
{
}

DrawableHandler::~DrawableHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto drawable = m_drawable.lock())
            sceneData->removeDrawable(drawable);
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
    : ResourceHandler(sceneData)
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

BackgroundHandler::BackgroundHandler(
    const std::weak_ptr<SceneData>& sceneData,
    const std::weak_ptr<const Background>& background)
    : ResourceHandler(sceneData)
    , m_background(background)
{
}

BackgroundHandler::~BackgroundHandler()
{
    if (auto sceneData = m_sceneData.lock())
        sceneData->removeBackground();
}

std::weak_ptr<const Background>& BackgroundHandler::background()
{
    return m_background;
}

LightHandler::LightHandler(const std::weak_ptr<SceneData>& sceneData, size_t ID)
    : ResourceHandler(sceneData)
    , m_ID(ID)
{
}

LightHandler::~LightHandler()
{
    if (auto sceneData = m_sceneData.lock())
        sceneData->removeLight(m_ID);
}

size_t LightHandler::ID() const
{
    return m_ID;
}

SceneData::~SceneData() = default;

std::shared_ptr<MeshHandler> SceneData::addMesh(const std::shared_ptr<const Mesh>& mesh)
{
    std::shared_ptr<MeshHandler> result;

    if (!mesh)
    {
        // do nothing
    }
    else
    {
        auto& meshPrivate = mesh->m();

        for (const auto& meshHandler : meshPrivate.handlers())
        {
            if (meshHandler->sceneData().lock() == shared_from_this())
            {
                result = meshHandler;
                break;
            }
        }

        if (!result)
        {
            bool isEmpty = mesh->mesh() == nullptr;

            auto meshID = isEmpty ? utils::IDsGenerator::last() : m_meshIDsGenerator.generate();
            result = std::make_shared<MeshHandler>(weak_from_this(), mesh, meshID);
            meshPrivate.handlers().insert(result);

            onMeshChanged(meshID, mesh->mesh(), mesh->boundingBox());
        }
    }

    return result;
}

void SceneData::removeMesh(const std::shared_ptr<const Mesh>& mesh)
{
    auto& meshHandlers = mesh->m().handlers();

    for (auto it = meshHandlers.begin(); it != meshHandlers.end(); ++it)
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            removeMeshData((*it)->ID());
            m_meshIDsGenerator.clear((*it)->ID());
            break;
        }
}

void SceneData::onMeshChanged(
    utils::IDsGenerator::value_type ID,
    const std::shared_ptr<const utils::Mesh>& mesh,
    const utils::BoundingBox& bb)
{
    removeMeshData(ID);
    addMeshData(ID, mesh, bb);
}

void SceneData::addMeshData(
    utils::IDsGenerator::value_type ID,
    const std::shared_ptr<const utils::Mesh>& mesh,
    const utils::BoundingBox& bb)
{
    if (!mesh)
        return;

    const auto& vertexBuffers = mesh->vertexBuffers();

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
    m_vertexDataBuffer->insert(vertexDataOffset, vertexData.data(), vertexData.size());

    static constexpr auto s_indexType = utils::toDrawElementsIndexType<ElementsDescription>();
    static_assert(s_indexType != utils::DrawElementsIndexType::Count);

    static constexpr auto s_primitiveType = utils::PrimitiveType::Triangles;

    uint32_t elementsOffset = m_elementsBuffer->size();
    uint32_t numElements = 0u;

    for (const auto& primitiveSet : mesh->primitiveSets())
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
            m_elementsBuffer->size(), reinterpret_cast<ElementsDescription*>(buffer->data()), buffer->numIndices());

        numElements += buffer->numIndices();
    }

    if (m_meshesBuffer->size() <= ID)
        m_meshesBuffer->resize(ID + 1u);

    m_meshesBuffer->set(ID, MeshDescription::make(
        bb,
        numVertexData,
        numElements,
        vertexDataOffset,
        elementsOffset,
        numPositionComponents,
        numNormalComponents,
        numTexCoordsComponents,
        numBones,
        hasTangent,
        numColorComponents));
}

void SceneData::removeMeshData(utils::IDsGenerator::value_type ID)
{
    if (m_meshesBuffer->size() <= ID)
        return;

    auto description = m_meshesBuffer->get(ID);
    const auto& vertexDataOffset = description.vertexDataOffset;
    auto numVertexData = MeshDescription::numVertexData(description);
    const auto& elementsOffset = description.elementsOffset;
    auto numElements = MeshDescription::numElements(description);

    for (size_t i = 0u; i < m_meshesBuffer->size(); ++i)
    {
        auto meshDescription = m_meshesBuffer->get(i);

        if (meshDescription.vertexDataOffset >= vertexDataOffset)
            meshDescription.vertexDataOffset -= numVertexData;

        if (meshDescription.elementsOffset >= elementsOffset)
            meshDescription.elementsOffset -= numElements;

        m_meshesBuffer->set(i, meshDescription);
    }

    m_vertexDataBuffer->erase(vertexDataOffset, numVertexData);
    m_elementsBuffer->erase(elementsOffset, numElements);

    m_meshesBuffer->erase(ID, 1u);
}

std::shared_ptr<MaterialMapHandler> SceneData::addMaterialMap(const std::shared_ptr<const MaterialMap>& materialMap)
{
    std::shared_ptr<MaterialMapHandler> result;

    if (!materialMap)
    {
        // do nothing
    }
    else if (materialMap->isEmpty())
    {
        // do nothing
    }
    else
    {
        auto& materialMapPrivate = materialMap->m();

        for (const auto& materialMapHandler : materialMapPrivate.handlers())
        {
            if (materialMapHandler->sceneData().lock() == shared_from_this())
            {
                result = materialMapHandler;
                break;
            }
        }

        if (!result)
        {
            bool isEmpty = materialMap->isEmpty();
            auto materialMapID = isEmpty ? utils::IDsGenerator::last() : m_materialMapIDsGenerator.generate();

            result = std::make_shared<MaterialMapHandler>(weak_from_this(), materialMap, materialMapID);
            materialMapPrivate.handlers().insert(result);

            onMaterialMapChanged(materialMapID, materialMap->filesystemPath(), materialMap->image());
        }
    }

    return result;
}

void SceneData::removeMaterialMap(const std::shared_ptr<const MaterialMap>& materialMap)
{
    auto& materialMapHandlers = materialMap->m().handlers();

    for (auto it = materialMapHandlers.begin(); it != materialMapHandlers.end(); ++it)
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            m_materialMapIDsGenerator.clear((*it)->ID());
            m_textureHandles[(*it)->ID()].reset();
            materialMapHandlers.erase(it);
            break;
        }
}

void SceneData::onMaterialMapChanged(
    utils::IDsGenerator::value_type ID,
    const std::filesystem::path& path,
    const std::shared_ptr<const utils::Image>& image)
{
    if (ID == utils::IDsGenerator::last())
        return;

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
    if (!path.empty())
        texture = texturesManager->loadOrGetTexture(path);
    else if (image)
        texture = texturesManager->loadOrGetTexture(image);

    if (!texture)
    {
        LOG_CRITICAL << "Failed to load texture";
        return;
    }

    graphics::PTextureHandle textureHandle = graphicsRenderer->createTextureHandle(texture);
    textureHandle->makeResident();

    if (m_materialMapsBuffer->size() <= ID) m_materialMapsBuffer->resize(ID + 1u);
    m_materialMapsBuffer->set(ID, MaterialMapDescription::make(textureHandle->handle()));

    if (m_textureHandles.size() <= ID) m_textureHandles.resize(ID + 1u);
    m_textureHandles[ID] = textureHandle;
}

std::shared_ptr<MaterialHandler> SceneData::addMaterial(const std::shared_ptr<const Material>& material)
{
    std::shared_ptr<MaterialHandler> result;

    if (!material)
    {
        // do nothing
    }
    else
    {
        auto& materialPrivate = material->m();

        for (const auto& materialHandler : materialPrivate.handlers())
        {
            if (materialHandler->sceneData().lock() == shared_from_this())
            {
                result = materialHandler;
                break;
            }
        }

        if (!result)
        {
            result = std::make_shared<MaterialHandler>(weak_from_this(), material, m_materialIDsGenerator.generate());
            materialPrivate.handlers().insert(result);

            onMaterialChanged(
                result->ID(),
                material->baseColor(),
                material->emission(),
                material->roughness(),
                material->metalness(),
                material->alphaCutoff(),
                material->materialMap(MaterialMapTarget::BaseColor),
                material->materialMap(MaterialMapTarget::Emission),
                material->materialMap(MaterialMapTarget::Occlusion),
                material->materialMap(MaterialMapTarget::Roughness),
                material->materialMap(MaterialMapTarget::Metalness),
                material->materialMap(MaterialMapTarget::Normal),
                material->occlusionMapStrength(),
                material->normalMapScale(),
                material->ORMSwizzleMask(),
                material->isLighted(),
                material->isShadowed(),
                material->isDoubleSided());
        }
    }

    return result;
}

void SceneData::removeMaterial(const std::shared_ptr<const Material>& material)
{
    auto& materialHandlers = material->m().handlers();

    for (auto it = materialHandlers.begin(); it != materialHandlers.end(); ++it)
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            m_materialIDsGenerator.clear((*it)->ID());
            materialHandlers.erase(it);
            break;
        }
}

void SceneData::onMaterialChanged(
    utils::IDsGenerator::value_type ID,
    const glm::vec4& baseColor,
    const glm::vec3& emission,
    float roughness,
    float metalness,
    float alphaCutoff,
    const std::shared_ptr<const MaterialMap>& baseColorMap,
    const std::shared_ptr<const MaterialMap>& emissionMap,
    const std::shared_ptr<const MaterialMap>& occlusionMap,
    const std::shared_ptr<const MaterialMap>& roughnessMap,
    const std::shared_ptr<const MaterialMap>& metalnessMap,
    const std::shared_ptr<const MaterialMap>& normalMap,
    float occlusionMapStrength,
    float normalMapScale,
    const glm::u32vec3& ORMSwizzleMask,
    bool isLighted,
    bool isShadowed,
    bool isDoubleSided)
{
    if (m_materialsBuffer->size() <= ID)
        m_materialsBuffer->resize(ID + 1u);

    m_materialsBuffer->set(ID, MaterialDescription::make(
        baseColor,
        glm::vec4(emission, 1.f),
        baseColorMap ? addMaterialMap(baseColorMap)->ID() : utils::IDsGenerator::last(),
        emissionMap ? addMaterialMap(emissionMap)->ID() : utils::IDsGenerator::last(),
        occlusionMap ? addMaterialMap(occlusionMap)->ID() : utils::IDsGenerator::last(),
        roughnessMap ? addMaterialMap(roughnessMap)->ID() : utils::IDsGenerator::last(),
        metalnessMap ? addMaterialMap(metalnessMap)->ID() : utils::IDsGenerator::last(),
        normalMap ? addMaterialMap(normalMap)->ID() : utils::IDsGenerator::last(),
        roughness,
        metalness,
        occlusionMapStrength,
        normalMapScale,
        ORMSwizzleMask,
        isLighted,
        isShadowed,
        isMaterialTransparent(baseColor, baseColorMap),
        isDoubleSided,
        alphaCutoff));
}

std::shared_ptr<DrawableHandler> SceneData::addDrawable(const std::shared_ptr<const Drawable>& drawable)
{
    std::shared_ptr<DrawableHandler> result;

    if (!drawable)
    {
        // do nothing
    }
    else
    {
        auto& drawablePrivate = drawable->m();

        for (const auto& drawableHandler : drawablePrivate.handlers())
        {
            if (drawableHandler->sceneData().lock() == shared_from_this())
            {
                result = drawableHandler;
                break;
            }
        }

        if (!result)
        {
            result = std::make_shared<DrawableHandler>(weak_from_this(), drawable, m_drawableIDsGenerator.generate());
            drawablePrivate.handlers().insert(result);

            onDrawableChanged(result->ID(), drawable->mesh(), drawable->material());
        }
    }

    return result;
}

void SceneData::removeDrawable(const std::shared_ptr<const Drawable>& drawable)
{
    auto& drawableHandlers = drawable->m().handlers();

    for (auto it = drawableHandlers.begin(); it != drawableHandlers.end(); ++it)
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            m_drawableIDsGenerator.clear((*it)->ID());
            drawableHandlers.erase(it);
            break;
        }
}

void SceneData::onDrawableChanged(
    utils::IDsGenerator::value_type ID,
    const std::shared_ptr<const Mesh>& mesh,
    const std::shared_ptr<const Material>& material)
{
    if (m_drawablesBuffer->size() <= ID)
        m_drawablesBuffer->resize(ID + 1u);

    m_drawablesBuffer->set(ID, DrawableDescription::make(
        addMesh(mesh)->ID(),
        addMaterial(material)->ID()));
}

std::shared_ptr<DrawDataHandler> SceneData::addDrawData(
    const std::shared_ptr<const Drawable>& drawable,
    const utils::Transform& transform)
{
    if (!drawable)
    {
        LOG_CRITICAL << "Failed to add draw data";
        return nullptr;
    }

    const auto ID = m_drawDataIDsGenerator.generate();
    auto result = std::make_shared<DrawDataHandler>(weak_from_this(), ID);

    onDrawDataChanged(ID, drawable, transform);

    return result;
}

void SceneData::removeDrawData(utils::IDsGenerator::value_type ID)
{
    m_drawDataBuffer->set(ID, DrawDataDescription::makeEmpty());
    m_drawDataIDsGenerator.clear(ID);
}

void SceneData::onDrawDataChanged(
    utils::IDsGenerator::value_type ID,
    const std::shared_ptr<const Drawable>& drawable,
    const utils::Transform& transform)
{
    if (!drawable)
    {
        LOG_CRITICAL << "Drawable can't be nullptr";
        return;
    }

    if (m_drawDataBuffer->size() <= ID)
    {
        m_drawDataBuffer->resize(ID + 1u);
        m_drawDataBuffer->setReservedData({ static_cast<uint32_t>(m_drawDataBuffer->size()), {0u, 0u, 0u} });
    }

    m_drawDataBuffer->set(ID, DrawDataDescription::make(transform, addDrawable(drawable)->ID()));
}

void SceneData::setBackground(const std::shared_ptr<const Background>& background)
{
    removeBackground();

    if (!background)
    {
        LOG_CRITICAL << "Failed to add background";
        return;
    }

    auto handler = std::make_shared<BackgroundHandler>(weak_from_this(), background);
    background->m().handler() = handler;

    m_backgroundHandler = handler;

    onBackgroundChanged(background->environmentMap(), background->environmentColor(), background->blurPower());
}

void SceneData::removeBackground()
{
    if (auto handler = m_backgroundHandler.lock())
        if (auto background = handler->background().lock())
            background->m().handler().reset();

    m_backgroundHandler.reset();
    m_backgroundBuffer->set(BackgroundDescription::makeEmpty());
}

void SceneData::onBackgroundChanged(
    const std::shared_ptr<const MaterialMap>& environmentMap,
    const glm::vec3& environmentColor,
    float blurPower)
{
    auto mipmapLevel = 0.f;
    auto materialMapHandler = addMaterialMap(environmentMap);

    if (auto environmentMapID = materialMapHandler->ID(); environmentMapID != utils::IDsGenerator::last())
        if (auto& textureHandle = m_textureHandles[environmentMapID])
            if (auto texture = textureHandle->texture())
                mipmapLevel = glm::clamp(blurPower, 0.f, 1.f) * glm::max(0u, (texture->numMipmapLevels() - 1u));

    m_backgroundBuffer->set(BackgroundDescription::make(environmentColor, mipmapLevel, materialMapHandler->ID()));
}

std::shared_ptr<LightHandler> SceneData::addPointLight(
    const utils::Transform& transform,
    const glm::vec3& color,
    const glm::vec2& radiuses)
{
    auto result = addLight();
    onPointLightChanged(result->ID(), transform, color, radiuses);
    return result;
}

void SceneData::onPointLightChanged(
    utils::IDsGenerator::value_type ID,
    const utils::Transform& transform,
    const glm::vec3& color,
    const glm::vec2& radiuses)
{
    onLightChanged(ID, LightDescription::makePoint(transform, color, radiuses));
}

std::shared_ptr<LightHandler> SceneData::addSpotLight(
    const utils::Transform& transform,
    const glm::vec3& color,
    const glm::vec2& radiuses,
    const glm::vec2& halfAngles)
{
    auto result = addLight();
    onSpotLightChanged(result->ID(), transform, color, radiuses, halfAngles);
    return result;
}

void SceneData::onSpotLightChanged(
    utils::IDsGenerator::value_type ID,
    const utils::Transform& transform,
    const glm::vec3& color,
    const glm::vec2& radiuses,
    const glm::vec2& halfAngles)
{
    onLightChanged(ID, LightDescription::makeSpot(transform, color, radiuses, halfAngles));
}

std::shared_ptr<LightHandler> SceneData::addDirectionalLight(const utils::Transform& transform, const glm::vec3& color)
{
    auto result = addLight();
    onDirectionalLightChanged(result->ID(), transform, color);
    return result;
}

void SceneData::onDirectionalLightChanged(
    utils::IDsGenerator::value_type ID,
    const utils::Transform& transform,
    const glm::vec3& color)
{
    onLightChanged(ID, LightDescription::makeDirectional(transform, color));
}

std::shared_ptr<LightHandler> SceneData::addImageBasedLight(
    const utils::Transform& transform,
    const std::shared_ptr<const MaterialMap>& BRDFLutMap,
    const std::shared_ptr<const MaterialMap>& diffuseMap,
    const std::shared_ptr<const MaterialMap>& specularMap,
    float contribution)
{
    auto result = addLight();
    onImageBasedLightChanged(result->ID(), transform, BRDFLutMap, diffuseMap, specularMap, contribution);
    return result;
}

void SceneData::onImageBasedLightChanged(utils::IDsGenerator::value_type ID,
    const utils::Transform& transform,
    const std::shared_ptr<const MaterialMap>& BRDFLutMap,
    const std::shared_ptr<const MaterialMap>& diffuseMap,
    const std::shared_ptr<const MaterialMap>& specularMap,
    float contribution)
{
    onLightChanged(ID, LightDescription::makeImageBased(
        transform,
        addMaterialMap(BRDFLutMap)->ID(),
        addMaterialMap(diffuseMap)->ID(),
        addMaterialMap(specularMap)->ID(),
        contribution));
}

void SceneData::removeLight(utils::IDsGenerator::value_type ID)
{
    m_lightsBuffer->set(ID, LightDescription::makeEmpty());
    m_lightIDsGenerator.clear(ID);
}

VertexDataBuffer& SceneData::vertexDataBuffer()
{
    return m_vertexDataBuffer;
}

const VertexDataBuffer& SceneData::vertexDataBuffer() const
{
    return const_cast<SceneData*>(this)->vertexDataBuffer();
}

ElementsBuffer& SceneData::elementsBuffer()
{
    return m_elementsBuffer;
}

const ElementsBuffer& SceneData::elementsBuffer() const
{
    return const_cast<SceneData*>(this)->elementsBuffer();
}

MeshesBuffer& SceneData::meshesBuffer()
{
    return m_meshesBuffer;
}

const MeshesBuffer& SceneData::meshesBuffer() const
{
    return const_cast<SceneData*>(this)->meshesBuffer();
}

MaterialMapsBuffer& SceneData::materialMapsBuffer()
{
    return m_materialMapsBuffer;
}

const MaterialMapsBuffer& SceneData::materialMapsBuffer() const
{
    return const_cast<SceneData*>(this)->materialMapsBuffer();
}

MaterialsBuffer& SceneData::materialsBuffer()
{
    return m_materialsBuffer;
}

const MaterialsBuffer& SceneData::materialsBuffer() const
{
    return const_cast<SceneData*>(this)->materialsBuffer();
}

DrawablesBuffer& SceneData::drawablesBuffer()
{
    return m_drawablesBuffer;
}

const DrawablesBuffer& SceneData::drawablesBuffer() const
{
    return const_cast<SceneData*>(this)->drawablesBuffer();
}

DrawDataBuffer& SceneData::drawDataBuffer()
{
    return m_drawDataBuffer;
}

const DrawDataBuffer& SceneData::drawDataBuffer() const
{
    return const_cast<SceneData*>(this)->drawDataBuffer();
}

BackgroundBuffer& SceneData::backgroundBuffer()
{
    return m_backgroundBuffer;
}

const BackgroundBuffer& SceneData::backgroundBuffer() const
{
    return const_cast<SceneData*>(this)->backgroundBuffer();
}

LightsBuffer& SceneData::lightsBuffer()
{
    return m_lightsBuffer;
}

const LightsBuffer& SceneData::lightsBuffer() const
{
    return const_cast<SceneData*>(this)->lightsBuffer();
}

graphics::PDrawArraysIndirectCommandsConstBuffer SceneData::screenQuadCommandsBuffer() const
{
    return m_screenQuadCommandsBuffer;
}

std::shared_ptr<SceneData> SceneData::create()
{
    auto result = std::shared_ptr<SceneData>(new SceneData);

    // create screen quad mesh
    result->m_screenQuadMesh = std::make_shared<Mesh>(utils::MeshPainter(utils::Mesh::createEmptyMesh({
        {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}
        })).drawScreenQuad().mesh(), utils::BoundingBox());
    auto screenQuadMeshHandler = result->addMesh(result->m_screenQuadMesh);

    // create screen quad commands buffer
    auto screenQuadMeshDescription = result->meshesBuffer()->get(screenQuadMeshHandler->ID());
    result->m_screenQuadCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create(
        { 1u, 1u, {0u, 0u} }, { {
                MeshDescription::numElements(screenQuadMeshDescription),
                1u,
                screenQuadMeshDescription.elementsOffset,
                screenQuadMeshHandler->ID() }
        });

    return result;
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
    m_backgroundBuffer = BackgroundBuffer::element_type::create();
    m_lightsBuffer = LightsBuffer::element_type::create();

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::VertexDataBuffer) = graphics::BufferRange::create(m_vertexDataBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ElementsBuffer) = graphics::BufferRange::create(m_elementsBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::MeshesBuffer) = graphics::BufferRange::create(m_meshesBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::MaterialMapsBuffer) = graphics::BufferRange::create(m_materialMapsBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::MaterialsBuffer) = graphics::BufferRange::create(m_materialsBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::DrawablesBuffer) = graphics::BufferRange::create(m_drawablesBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::DrawDataBuffer) = graphics::BufferRange::create(m_drawDataBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::BackgroundBuffer) = graphics::BufferRange::create(m_backgroundBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::LightsBuffer) = graphics::BufferRange::create(m_lightsBuffer->buffer());
}

std::shared_ptr<LightHandler> SceneData::addLight()
{
    const auto ID = m_lightIDsGenerator.generate();
    return std::make_shared<LightHandler>(weak_from_this(), ID);
}

void SceneData::onLightChanged(utils::IDsGenerator::value_type ID, const LightDescription& lightDescription)
{
    if (m_lightsBuffer->size() <= ID)
    {
        m_lightsBuffer->resize(ID + 1u);

        auto reservedData = m_lightsBuffer->reservedData();
        reservedData.count = static_cast<uint32_t>(m_lightsBuffer->size());
        m_lightsBuffer->setReservedData(reservedData);
    }

    m_lightsBuffer->set(ID, lightDescription);
}

bool SceneData::isMaterialTransparent(const glm::vec4& baseColor, const std::shared_ptr<const MaterialMap>& baseColorMap)
{
    if (baseColor.a < 1.f - utils::epsilon<float>()) return true;

    if (baseColorMap)
    {
        std::shared_ptr<const utils::Image> baseColorImage;
        if (auto path = baseColorMap->filesystemPath(); !path.empty())
        {
            baseColorImage = utils::ImageManager::instance().loadOrGetDescription(path);
        }
        else if (auto image = baseColorMap->image(); image)
        {
            baseColorImage = image;
        }

        if (baseColorImage && (baseColorImage->numComponents() == 4u))
            return true;
    }

    // other future conditions like opacity map

    return false;
}

}
}
