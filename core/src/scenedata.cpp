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

DrawDataHandler::DrawDataHandler(const std::weak_ptr<SceneData>& sceneData, size_t id)
    : m_sceneData(sceneData)
    , m_ID(id)
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

BackgroundHandler::BackgroundHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Background>& background)
    : m_sceneData(sceneData)
    , m_background(background)
{
}

BackgroundHandler::~BackgroundHandler()
{
    if (auto sceneData = m_sceneData.lock())
        sceneData->removeBackground();
}

const std::weak_ptr<const Background>& BackgroundHandler::background()
{
    return m_background;
}

SceneData::SceneData()
{
    m_positionsBuffer = graphics::DynamicBufferT<PositionDescription>::create();
    m_normalsBuffer = graphics::DynamicBufferT<NormalDescription>::create();
    m_texCoordsBuffer = graphics::DynamicBufferT<TexCoordsDescription>::create();
    m_bonesBuffer = graphics::DynamicBufferT<BonesDescription>::create();
    m_tangentsBuffer = graphics::DynamicBufferT<TangentDescription>::create();
    m_colorsBuffer = graphics::DynamicBufferT<ColorDescription>::create();
    m_indicesBuffer = graphics::DynamicBufferT<IndexDescription>::create();
    m_meshesBuffer = graphics::DynamicBufferT<MeshDescription>::create();
    m_materialMapsBuffer = graphics::DynamicBufferT<MaterialMapDescription>::create();
    m_materialsBuffer = graphics::DynamicBufferT<MaterialDescription>::create();
    m_drawablesBuffer = graphics::DynamicBufferT<DrawableDescription>::create();
    m_drawDataBuffer = graphics::DynamicBufferT<DrawDataDescription>::create();
    m_backgroundsBuffer = graphics::DynamicBufferT<BackgroundDescription>::create();

    m_drawDataCommandsBuffer = graphics::DynamicBufferT<DrawElementsIndirectCommand>::create();
    m_backgroundCommandsBuffer = graphics::DynamicBufferT<DrawElementsIndirectCommand>::create();
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
        auto& meshPrivate = mesh->m();

        result = m_meshIDsGenerator.generate();

        auto handler = std::make_shared<MeshHandler>(weak_from_this(), mesh, result);
        meshPrivate.handlers().insert({ shared_from_this(), handler });
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
    uint32_t positionOffset = 0xFFFFFFFF;
    uint32_t normalOffset = 0xFFFFFFFF;
    uint32_t texCoordsOffset = 0xFFFFFFFF;
    uint32_t bonesOffset = 0xFFFFFFFF;
    uint32_t tangentOffset = 0xFFFFFFFF;
    uint32_t colorOffset = 0xFFFFFFFF;
    uint32_t indexOffset = 0xFFFFFFFF;

    auto& vertexBuffers = mesh->mesh()->vertexBuffers();

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Position); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if ((buffer->numComponents() != PositionDescription::length()) ||
            (buffer->componentType() != utils::toVertexComponentType<PositionDescription::value_type>()))
        {
            buffer = buffer->converted(
                PositionDescription::length(),
                utils::toVertexComponentType<PositionDescription::value_type>());
        }

        positionOffset = m_positionsBuffer->size();
        m_positionsBuffer->insert(
            positionOffset, buffer->numVertices(), reinterpret_cast<PositionDescription*>(buffer->data()));
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Normal); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if ((buffer->numComponents() != NormalDescription::length()) ||
            (buffer->componentType() != utils::toVertexComponentType<NormalDescription::value_type>()))
        {
            buffer = buffer->converted(
                NormalDescription::length(),
                utils::toVertexComponentType<NormalDescription::value_type>());
        }

        normalOffset = m_normalsBuffer->size();
        m_normalsBuffer->insert(
            normalOffset, buffer->numVertices(), reinterpret_cast<NormalDescription*>(buffer->data()));
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::TexCoords); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if ((buffer->numComponents() != TexCoordsDescription::length()) ||
            (buffer->componentType() != utils::toVertexComponentType<TexCoordsDescription::value_type>()))
        {
            buffer = buffer->converted(
                TexCoordsDescription::length(),
                utils::toVertexComponentType<TexCoordsDescription::value_type>());
        }

        texCoordsOffset = m_texCoordsBuffer->size();
        m_texCoordsBuffer->insert(
            texCoordsOffset, buffer->numVertices(), reinterpret_cast<TexCoordsDescription*>(buffer->data()));
    }

    if (vertexBuffers.count(utils::VertexAttribute::BonesIDs) && vertexBuffers.count(utils::VertexAttribute::BonesWeights))
    {
        std::shared_ptr<utils::VertexBuffer> bonesIDsBuffer = vertexBuffers.at(utils::VertexAttribute::BonesIDs);
        std::shared_ptr<utils::VertexBuffer> bonesWeightsBuffer = vertexBuffers.at(utils::VertexAttribute::BonesWeights);

        if (bonesIDsBuffer->numVertices() != bonesWeightsBuffer->numVertices())
        {
            if ((bonesIDsBuffer->numComponents() != BonesDescription::IDsDescription::length()) ||
                (bonesIDsBuffer->componentType() != utils::toVertexComponentType<BonesDescription::IDsDescription::value_type>()))
            {
                bonesIDsBuffer = bonesIDsBuffer->converted(
                    BonesDescription::IDsDescription::length(),
                    utils::toVertexComponentType<BonesDescription::IDsDescription::value_type>());
            }

            if ((bonesWeightsBuffer->numComponents() != BonesDescription::WeightsDescription::length()) ||
                (bonesWeightsBuffer->componentType() != utils::toVertexComponentType<BonesDescription::WeightsDescription::value_type>()))
            {
                bonesWeightsBuffer = bonesWeightsBuffer->converted(
                    BonesDescription::WeightsDescription::length(),
                    utils::toVertexComponentType<BonesDescription::WeightsDescription::value_type>());
            }

            const auto numVertices = bonesIDsBuffer->numVertices();
            std::vector<BonesDescription> bonesBuffer(numVertices);
            for (size_t i = 0u; i < numVertices; ++i)
            {
                bonesBuffer[i].IDs = *reinterpret_cast<const BonesDescription::IDsDescription*>(bonesIDsBuffer->vertex(i));
                bonesBuffer[i].weights = *reinterpret_cast<const BonesDescription::WeightsDescription*>(bonesWeightsBuffer->vertex(i));
            }

            bonesOffset = m_bonesBuffer->size();
            m_bonesBuffer->insert(bonesOffset, bonesBuffer.size(), bonesBuffer.data());
        }
        else
        {
            LOG_ERROR << "Bones IDs and weights buffers have different size";
        }
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Tangent); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if ((buffer->numComponents() != TangentDescription::length()) ||
            (buffer->componentType() != utils::toVertexComponentType<TangentDescription::value_type>()))
        {
            buffer = buffer->converted(
                TangentDescription::length(),
                utils::toVertexComponentType<TangentDescription::value_type>());
        }

        tangentOffset = m_tangentsBuffer->size();
        m_tangentsBuffer->insert(
            tangentOffset, buffer->numVertices(), reinterpret_cast<TangentDescription*>(buffer->data()));
    }

    if (auto it = vertexBuffers.find(utils::VertexAttribute::Color); it != vertexBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if ((buffer->numComponents() != ColorDescription::length()) ||
            (buffer->componentType() != utils::toVertexComponentType<ColorDescription::value_type>()))
        {
            buffer = buffer->converted(
                ColorDescription::length(),
                utils::toVertexComponentType<ColorDescription::value_type>());
        }

        colorOffset = m_colorsBuffer->size();
        m_colorsBuffer->insert(
            colorOffset, buffer->numVertices(), reinterpret_cast<ColorDescription*>(buffer->data()));
    }

    static constexpr auto s_indexType = utils::toDrawElementsIndexType<IndexDescription>();
    static_assert(s_indexType != utils::DrawElementsIndexType::Count);

    static constexpr auto s_primitiveType = utils::PrimitiveType::Triangles;

    indexOffset = m_indicesBuffer->size();
    uint32_t numElements = 0u;

    for (const auto& primitiveSet : mesh->mesh()->primitiveSets())
    {
        std::shared_ptr<utils::DrawElementsBuffer> buffer;
        if (auto drawArrays = primitiveSet->asDrawArrays())
        {
            buffer = nullptr;
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

        numElements += buffer->numIndices();

        const auto offset = m_indicesBuffer->size();
        m_indicesBuffer->insert(
            offset, buffer->numIndices(), reinterpret_cast<IndexDescription*>(buffer->data()));
    }

    if (m_meshesBuffer->size() <= ID)
        m_meshesBuffer->resize(ID + 1u);

    m_meshesBuffer->set(ID, {
        positionOffset,
        normalOffset,
        texCoordsOffset,
        bonesOffset,
        tangentOffset,
        colorOffset,
        indexOffset,
        numElements
        });
}

void SceneData::removeMeshData(const std::shared_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type ID)
{
    auto meshDescription = m_meshesBuffer->get(ID);

    auto positionSize = meshDescription.positionOffset == 0xFFFFFFFF ? 0u :
        (m_meshesBuffer->size() > ID + 1u) ?
        m_meshesBuffer->get(ID + 1u).positionOffset - meshDescription.positionOffset :
        m_positionsBuffer->size() - meshDescription.positionOffset;

    auto normalSize = meshDescription.normalOffset == 0xFFFFFFFF ? 0u :
        (m_meshesBuffer->size() > ID + 1u) ?
        m_meshesBuffer->get(ID + 1u).normalOffset - meshDescription.normalOffset :
        m_normalsBuffer->size() - meshDescription.normalOffset;

    auto texCoordsSize = meshDescription.texCoordsOffset == 0xFFFFFFFF ? 0u :
        (m_meshesBuffer->size() > ID + 1u) ?
        m_meshesBuffer->get(ID + 1u).texCoordsOffset - meshDescription.texCoordsOffset :
        m_texCoordsBuffer->size() - meshDescription.texCoordsOffset;

    auto bonesSize = meshDescription.bonesOffset == 0xFFFFFFFF ? 0u :
        (m_meshesBuffer->size() > ID + 1u) ?
        m_meshesBuffer->get(ID + 1u).bonesOffset - meshDescription.bonesOffset :
        m_bonesBuffer->size() - meshDescription.bonesOffset;

    auto tangentSize = meshDescription.tangentOffset == 0xFFFFFFFF ? 0u :
        (m_meshesBuffer->size() > ID + 1u) ?
        m_meshesBuffer->get(ID + 1u).tangentOffset - meshDescription.tangentOffset :
        m_tangentsBuffer->size() - meshDescription.tangentOffset;

    auto colorSize = meshDescription.colorOffset == 0xFFFFFFFF ? 0u :
        (m_meshesBuffer->size() > ID + 1u) ?
        m_meshesBuffer->get(ID + 1u).colorOffset - meshDescription.colorOffset :
        m_colorsBuffer->size() - meshDescription.colorOffset;

    auto indexSize = meshDescription.indexOffset == 0xFFFFFFFF ? 0u :
        (m_meshesBuffer->size() > ID + 1u) ?
        m_meshesBuffer->get(ID + 1u).indexOffset - meshDescription.indexOffset :
        m_indicesBuffer->size() - meshDescription.indexOffset;

    for (size_t i = ID + 1u; i < m_meshesBuffer->size(); ++i)
    {
        auto meshIDescription = m_meshesBuffer->get(i);

        meshIDescription.positionOffset -= positionSize;
        meshIDescription.normalOffset -= normalSize;
        meshIDescription.texCoordsOffset -= texCoordsSize;
        meshIDescription.bonesOffset -= bonesSize;
        meshIDescription.tangentOffset -= tangentSize;
        meshIDescription.colorOffset -= colorSize;
        meshIDescription.indexOffset -= indexSize;

        m_meshesBuffer->set(i, meshIDescription);
    }

    m_positionsBuffer->erase(meshDescription.positionOffset, positionSize);
    m_normalsBuffer->erase(meshDescription.normalOffset, normalSize);
    m_texCoordsBuffer->erase(meshDescription.texCoordsOffset, texCoordsSize);
    m_bonesBuffer->erase(meshDescription.bonesOffset, bonesSize);
    m_tangentsBuffer->erase(meshDescription.tangentOffset, tangentSize);
    m_colorsBuffer->erase(meshDescription.colorOffset, colorSize);
    m_indicesBuffer->erase(meshDescription.indexOffset, indexSize);

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
    else if (auto it = m_materialMaps.find(materialMap); it != m_materialMaps.end())
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
        m_materialMaps.insert({ materialMap, handler });

        onMaterialMapChanged(materialMap, result);
    }

    return result;
}

void SceneData::removeMaterialMap(const std::shared_ptr<const MaterialMap>& materialMap, utils::IDsGenerator::value_type ID)
{
    if (auto it = m_materialMaps.find(materialMap); it == m_materialMaps.end())
    {
        LOG_CRITICAL << "No found material map in scene to remove";
        return;
    }
    else
    {
        m_materialMapsHandles.erase(ID);
        m_materialMaps.erase(materialMap);
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

    if (auto it = m_materialMaps.find(materialMap); it == m_materialMaps.end())
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
        materialPrivate.baseColor(),
        materialPrivate.emission(),
        materialPrivate.roughness(),
        materialPrivate.metalness(),
        makeMaterialFlags(
            materialPrivate.occlusionMapStrength(),
            materialPrivate.normalMapScale(),
            materialPrivate.ORMSwizzleMask(),
            materialPrivate.isLighted(),
            materialPrivate.isShadowed()
        )
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

    const auto ID = m_drawDataHandlers.size();

    auto result = std::make_shared<DrawDataHandler>(weak_from_this(), ID);
    m_drawDataHandlers.push_back(result);

    onDrawDataChanged(drawable, modelMatrix, ID);

    return result;
}

void SceneData::removeDrawData(size_t ID)
{
    if (ID >= m_drawDataHandlers.size())
    {
        LOG_CRITICAL << "No found draw data in scene to remove";
        return;
    }

    m_drawDataHandlers.erase(std::next(m_drawDataHandlers.begin(), ID));
    m_drawDataCommandsBuffer->erase(ID, 1u);
}

void SceneData::onDrawDataChanged(const std::shared_ptr<const Drawable>& drawable, const glm::mat4x4& modelMatrix, size_t ID)
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
        m_drawDataCommandsBuffer->resize(ID + 1u);
    }

    m_drawDataBuffer->set(ID, {
        modelMatrix,
        addDrawable(drawable)
        });

    //auto numIndices = 0u;
    //if ()


    //m_drawDataCommandsBuffer->set(ID, {
    //    m_meshesNumIndices[],
    //    });
}

void SceneData::addBackground(const std::shared_ptr<const Background>& background)
{
    if (!background)
    {
        LOG_CRITICAL << "Failed to add background";
        return;
    }

    if (!m_backgroundHandler.expired())
    {
        LOG_CRITICAL << "Background handler alreay exists";
        return;
    }

    auto handler = std::make_shared<BackgroundHandler>(weak_from_this(), background);
    background->m().handler() = {shared_from_this(), handler};
    m_backgroundHandler = handler;

    onBackgroundChanged(background);
}

void SceneData::removeBackground()
{
    m_backgroundHandler.reset();
}

void SceneData::onBackgroundChanged(const std::shared_ptr<const Background>& background)
{
    if (!background)
    {
        LOG_CRITICAL << "Background can't be nullptr";
        return;
    }

    if (m_backgroundHandler.expired())
    {
        LOG_CRITICAL << "No found background in scene to change";
        return;
    }

    if (!m_backgroundsBuffer->size())
        m_backgroundsBuffer->resize(1u);

    m_backgroundsBuffer->set(0u, {
        addMesh(ScreenQuad::instance()),
        addMaterialMap(background->environmentMap()),
        background->environmentColor(),
        background->blurPower()
        });
}

std::shared_ptr<graphics::DynamicBufferT<PositionDescription>>& SceneData::positionsBuffer()
{
    return m_positionsBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<NormalDescription>>& SceneData::normalsBuffer()
{
    return m_normalsBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<TexCoordsDescription>>& SceneData::texCoordsBuffer()
{
    return m_texCoordsBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<BonesDescription>>& SceneData::bonesBuffer()
{
    return m_bonesBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<TangentDescription>>& SceneData::tangentsBuffer()
{
    return m_tangentsBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<ColorDescription>>& SceneData::colorsBuffer()
{
    return m_colorsBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<IndexDescription>>& SceneData::indicesBuffer()
{
    return m_indicesBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<MeshDescription>>& SceneData::meshesBuffer()
{
    return m_meshesBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<MaterialMapDescription>>& SceneData::texturesBuffer()
{
    return m_materialMapsBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<MaterialDescription>>& SceneData::materialBuffer()
{
    return m_materialsBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<DrawableDescription>>& SceneData::drawablesBuffer()
{
    return m_drawablesBuffer;
}

std::shared_ptr<graphics::DynamicBufferT<DrawDataDescription>>& SceneData::drawDataBuffer()
{
    return m_drawDataBuffer;
}

}
}
