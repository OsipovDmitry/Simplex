#include <utils/meshpainter.h>
#include <utils/imagemanager.h>

#include <core/background.h>
#include <core/drawable.h>
#include <core/graphicsengine.h>
#include <core/graphicsrendererbase.h>
#include <core/igraphicswidget.h>
#include <core/material.h>
#include <core/mesh.h>
#include <core/scene.h>
#include <core/skeletalanimation.h>
#include <core/texturesmanager.h>

#include "backgroundprivate.h"
#include "drawableprivate.h"
#include "materialprivate.h"
#include "meshprivate.h"
#include "scenedata.h"
#include "sceneprivate.h"
#include "skeletalanimationprivate.h"

namespace simplex
{
namespace core
{

static const utils::RangeT<uint32_t> BonesCountRange{1u, 7u};
using BonesIDsUnderlyingType = uint32_t;

using VertexAttributeFormat = std::pair<utils::RangeT<uint32_t>, utils::VertexComponentType>;
static const VertexAttributeFormat PositionFormat{ {1u, 3u}, utils::toVertexComponentType<VerticesDataDescription>()};
static const VertexAttributeFormat NormalFormat{ {1u, 3u}, utils::toVertexComponentType<VerticesDataDescription>() };
static const VertexAttributeFormat TexCoordsFormat{ {1u, 3u}, utils::toVertexComponentType<VerticesDataDescription>() };
static const VertexAttributeFormat BonesIDsFormat{ BonesCountRange, utils::toVertexComponentType<BonesIDsUnderlyingType>() };
static const VertexAttributeFormat BonesWeightsFormat{ BonesCountRange, utils::toVertexComponentType<VerticesDataDescription>() };
static const VertexAttributeFormat TangentFormat{ {4u, 4u}, utils::toVertexComponentType<VerticesDataDescription>() };
static const VertexAttributeFormat ColorFormat{ {1u, 4u}, utils::toVertexComponentType<VerticesDataDescription>() };

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

MeshHandler::MeshHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
    , m_mesh()
    , m_ID(utils::IDsGenerator::last())
{
}

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

MaterialMapHandler::MaterialMapHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
    , m_ID(utils::IDsGenerator::last())
{
}

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

MaterialHandler::MaterialHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
    , m_ID(utils::IDsGenerator::last())
{
}

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

DrawableHandler::DrawableHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
    , m_ID(utils::IDsGenerator::last())
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

DrawDataHandler::DrawDataHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
    , m_ID(utils::IDsGenerator::last())
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

BackgroundHandler::BackgroundHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
{
}

BackgroundHandler::~BackgroundHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto background = m_background.lock())
            sceneData->removeBackground(background);
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

SkeletonHandler::SkeletonHandler(
    const std::weak_ptr<SceneData>& sceneData,
    const std::weak_ptr<const Skeleton>& skeleton,
    utils::IDsGenerator::value_type ID)
    : ResourceHandler(sceneData)
    , m_skeleton(skeleton)
    , m_ID(ID)
{

}

SkeletonHandler::SkeletonHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
    , m_ID(utils::IDsGenerator::last())
{
}

SkeletonHandler::~SkeletonHandler()
{
    if (auto sceneData = m_sceneData.lock())
        if (auto skeleton = m_skeleton.lock())
            sceneData->removeSkeleton(skeleton);
}

std::weak_ptr<const Skeleton>& SkeletonHandler::skeleton()
{
    return m_skeleton;
}

utils::IDsGenerator::value_type SkeletonHandler::ID() const
{
    return m_ID;
}

SkeletalAnimatedDataHandler::SkeletalAnimatedDataHandler(const std::weak_ptr<SceneData>& sceneData, size_t ID)
    : ResourceHandler(sceneData)
    , m_ID(ID)
{
}

SkeletalAnimatedDataHandler::SkeletalAnimatedDataHandler(const std::weak_ptr<SceneData>& sceneData)
    : ResourceHandler(sceneData)
    , m_ID(utils::IDsGenerator::last())
{
}

SkeletalAnimatedDataHandler::~SkeletalAnimatedDataHandler()
{
    if (auto sceneData = m_sceneData.lock())
        ;// sceneData->removeDrawData(m_ID);
}

size_t SkeletalAnimatedDataHandler::ID() const
{
    return m_ID;
}

SceneData::SceneData()
{
    m_verticesDataBuffer = VerticesDataBuffer::element_type::create();
    m_elementsDataBuffer = ElementsDataBuffer::element_type::create();
    m_skeletonsDataBuffer = SkeletonsDataBuffer::element_type::create();
    m_bonesTransformsDataBuffer = BonesTransformsDataBuffer::element_type::create();

    m_meshesBuffer = MeshesBuffer::element_type::create();
    m_materialMapsBuffer = MaterialMapsBuffer::element_type::create();
    m_materialsBuffer = MaterialsBuffer::element_type::create();
    m_drawablesBuffer = DrawablesBuffer::element_type::create();
    m_backgroundBuffer = BackgroundBuffer::element_type::create();
    m_lightsBuffer = LightsBuffer::element_type::create();
    m_skeletonsBuffer = SkeletonsBuffer::element_type::create();

    m_drawDataBuffer = DrawDataBuffer::element_type::create();
    m_skeletalAnimatedDataBuffer = SkeletalAnimatedDataBuffer::element_type::create();

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::VerticesDataBuffer) = graphics::BufferRange::create(m_verticesDataBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::ElementsDataBuffer) = graphics::BufferRange::create(m_elementsDataBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SkeletonsDataBuffer) = graphics::BufferRange::create(m_skeletonsDataBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::BonesTransformsDataBuffer) = graphics::BufferRange::create(m_bonesTransformsDataBuffer->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::MeshesBuffer) = graphics::BufferRange::create(m_meshesBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::MaterialMapsBuffer) = graphics::BufferRange::create(m_materialMapsBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::MaterialsBuffer) = graphics::BufferRange::create(m_materialsBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::DrawablesBuffer) = graphics::BufferRange::create(m_drawablesBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::BackgroundBuffer) = graphics::BufferRange::create(m_backgroundBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::LightsBuffer) = graphics::BufferRange::create(m_lightsBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SkeletonsBuffer) = graphics::BufferRange::create(m_skeletonsBuffer->buffer());

    getOrCreateShaderStorageBlock(ShaderStorageBlockID::DrawDataBuffer) = graphics::BufferRange::create(m_drawDataBuffer->buffer());
    getOrCreateShaderStorageBlock(ShaderStorageBlockID::SkeletalAnimatedDataBuffer) = graphics::BufferRange::create(m_skeletalAnimatedDataBuffer->buffer());

    // create screen quad mesh
    m_screenQuadMesh = std::make_shared<Mesh>(utils::MeshPainter(utils::Mesh::createEmptyMesh({
        {utils::VertexAttribute::Position, {2u, utils::VertexComponentType::Single}}
        })).drawScreenQuad().mesh(), utils::BoundingBox());
    auto screenQuadMeshHandler = addMesh(m_screenQuadMesh);

    // create screen quad commands buffer
    auto screenQuadMeshDescription = m_meshesBuffer->get(screenQuadMeshHandler->ID());
    m_screenQuadCommandsBuffer = graphics::PDrawArraysIndirectCommandsBuffer::element_type::create({ {
        MeshDescription::elementsDataSize(screenQuadMeshDescription),
        1u,
        screenQuadMeshDescription.elementsDataOffset,
        screenQuadMeshHandler->ID() }
        });
}

SceneData::~SceneData() = default;

SceneData::AddVerticesDataResult SceneData::addVerticesData(
    const std::unordered_map<utils::VertexAttribute, std::shared_ptr<utils::VertexBuffer>>& verticesBuffers)
{
    const size_t verticesCount = verticesBuffers.empty() ? 0u : verticesBuffers.begin()->second->numVertices();
    size_t vertexStride = 0u; // in floats
    for (auto const& [attrib, buffer] : verticesBuffers)
    {
        if (buffer->numVertices() != verticesCount)
        {
            LOG_ERROR << "Buffers have different sizes";
            return {};
        }

        vertexStride += buffer->numComponents();
    }

    const size_t verticesDataSize = verticesCount * vertexStride;
    if (!verticesDataSize)
        return {};

    std::vector<VerticesDataDescription> verticesData(verticesDataSize);

    size_t relativeOffset = 0u;
    uint32_t positionComponentsCount = 0u;
    uint32_t normalComponentsCount = 0u;
    uint32_t texCoordsComponentsCount = 0u;
    bool hasTangent = false;
    uint32_t bonesCount = 0u;
    uint32_t colorComponentsCount = 0u;

    if (auto it = verticesBuffers.find(utils::VertexAttribute::Position); it != verticesBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!PositionFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != PositionFormat.second))
        {
            buffer = buffer->converted(PositionFormat.first.farValue(), PositionFormat.second);
        }

        positionComponentsCount = buffer->numComponents();
        for (size_t v = 0u; v < verticesCount; ++v)
        {
            const auto* vertex = reinterpret_cast<const VerticesDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < positionComponentsCount; ++c)
                verticesData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += positionComponentsCount;
    }

    if (auto it = verticesBuffers.find(utils::VertexAttribute::Normal); it != verticesBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!NormalFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != NormalFormat.second))
        {
            buffer = buffer->converted(NormalFormat.first.farValue(), NormalFormat.second);
        }

        normalComponentsCount = buffer->numComponents();
        for (size_t v = 0u; v < verticesCount; ++v)
        {
            const auto* vertex = reinterpret_cast<const VerticesDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < normalComponentsCount; ++c)
                verticesData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += normalComponentsCount;
    }

    if (auto it = verticesBuffers.find(utils::VertexAttribute::TexCoords); it != verticesBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!TexCoordsFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != TexCoordsFormat.second))
        {
            buffer = buffer->converted(TexCoordsFormat.first.farValue(), TexCoordsFormat.second);
        }

        texCoordsComponentsCount = buffer->numComponents();
        for (size_t v = 0u; v < verticesCount; ++v)
        {
            const auto* vertex = reinterpret_cast<const VerticesDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < texCoordsComponentsCount; ++c)
                verticesData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += texCoordsComponentsCount;
    }

    if (auto it = verticesBuffers.find(utils::VertexAttribute::Tangent);
        (it != verticesBuffers.end()) && (normalComponentsCount > 0u))
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (TangentFormat.first.isInside(buffer->numComponents())) // only 4-components tangent is suitable
        {
            if (buffer->componentType() != TangentFormat.second)
            {
                buffer = buffer->converted(buffer->numComponents(), TangentFormat.second);
            }

            const size_t componentsCount = buffer->numComponents();
            hasTangent = componentsCount != 0u;
            for (size_t v = 0u; v < verticesCount; ++v)
            {
                const auto* vertex = reinterpret_cast<const VerticesDataDescription*>(buffer->vertex(v));
                for (size_t c = 0u; c < componentsCount; ++c)
                    verticesData[vertexStride * v + relativeOffset + c] = vertex[c];
            }

            relativeOffset += componentsCount;
        }
    }

    if (verticesBuffers.count(utils::VertexAttribute::BonesIDs) &&
        verticesBuffers.count(utils::VertexAttribute::BonesWeights))
    {
        std::shared_ptr<utils::VertexBuffer> bonesIDsBuffer = verticesBuffers.at(utils::VertexAttribute::BonesIDs);
        std::shared_ptr<utils::VertexBuffer> bonesWeightsBuffer = verticesBuffers.at(utils::VertexAttribute::BonesWeights);

        bonesCount = bonesIDsBuffer->numComponents();
        if (bonesCount != bonesWeightsBuffer->numComponents())
        {
            LOG_CRITICAL << "Bones IDs and weights buffers must have equal number of components";
            return {};
        }

        bonesCount = glm::min(bonesCount, BonesCountRange.farValue());

        if (!BonesIDsFormat.first.isInside(bonesIDsBuffer->numComponents()) ||
            (bonesIDsBuffer->componentType() != BonesIDsFormat.second))
        {
            bonesIDsBuffer = bonesIDsBuffer->converted(bonesCount, BonesIDsFormat.second);
        }

        if (!BonesWeightsFormat.first.isInside(bonesWeightsBuffer->numComponents()) ||
            (bonesWeightsBuffer->componentType() != BonesWeightsFormat.second))
        {
            bonesWeightsBuffer =
                bonesWeightsBuffer->converted(bonesCount, BonesWeightsFormat.second);
        }

        for (size_t v = 0u; v < verticesCount; ++v)
        {
            const auto* IDs = reinterpret_cast<const BonesIDsUnderlyingType*>(bonesIDsBuffer->vertex(v));
            const auto* weights = reinterpret_cast<const VerticesDataDescription*>(bonesWeightsBuffer->vertex(v));
            for (uint32_t c = 0u; c < bonesCount; ++c)
            {
                verticesData[vertexStride * v + relativeOffset + 2u * c + 0u] = glm::uintBitsToFloat(IDs[c]);
                verticesData[vertexStride * v + relativeOffset + 2u * c + 1u] = weights[c];
            }
        }

        relativeOffset += static_cast<size_t>(bonesCount) * 2u;
    }

    if (auto it = verticesBuffers.find(utils::VertexAttribute::Color); it != verticesBuffers.end())
    {
        std::shared_ptr<utils::VertexBuffer> buffer = it->second;
        if (!ColorFormat.first.isInside(buffer->numComponents()) ||
            (buffer->componentType() != ColorFormat.second))
        {
            buffer = buffer->converted(ColorFormat.first.farValue(), ColorFormat.second);
        }

        colorComponentsCount = buffer->numComponents();
        for (size_t v = 0u; v < verticesCount; ++v)
        {
            const auto* vertex = reinterpret_cast<const VerticesDataDescription*>(buffer->vertex(v));
            for (uint32_t c = 0u; c < colorComponentsCount; ++c)
                verticesData[vertexStride * v + relativeOffset + c] = vertex[c];
        }

        relativeOffset += colorComponentsCount;
    }

    const auto verticesDataOffset = m_verticesDataBuffer->allocate(verticesData.size(), verticesData.data());

    return {
        static_cast<uint32_t>(verticesDataOffset),
        static_cast<uint32_t>(verticesDataSize),
        positionComponentsCount,
        normalComponentsCount,
        texCoordsComponentsCount,
        hasTangent,
        bonesCount,
        colorComponentsCount };
}

void SceneData::removeVerticeshData(uint32_t verticesDataOffset, uint32_t verticesDataSize)
{
    if ((verticesDataOffset == utils::IDsGenerator::last()) || (!verticesDataSize))
        return;
    
    m_verticesDataBuffer->free(verticesDataOffset, verticesDataSize);
}

SceneData::AddElementsDataResult SceneData::addElementsData(const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>>& primitiveSets)
{
    static constexpr auto s_indexType = utils::toDrawElementsIndexType<ElementsDataDescription>();
    static_assert(s_indexType != utils::DrawElementsIndexType::Count);

    static constexpr auto s_primitiveType = utils::PrimitiveType::Triangles;

    size_t elementsDataSize = 0u;
    std::vector<std::shared_ptr<utils::DrawElementsBuffer>> drawElementsBuffers;
    drawElementsBuffers.reserve(primitiveSets.size());

    for (const auto& primitiveSet : primitiveSets)
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
            LOG_ERROR << "Failed to cast to draw elemtns buffer";
            continue;
        }

        if (buffer->indexType() != s_indexType)
            buffer = buffer->convertedToIndexType(s_indexType);

        if (buffer->primitiveType() != s_primitiveType)
            buffer = buffer->convertedToTriangles();

        if (buffer->baseVertex())
            buffer = buffer->appliedBaseVertex();

        elementsDataSize += buffer->numIndices();
        drawElementsBuffers.push_back(buffer);
    }

    if (!elementsDataSize)
        return {};

    std::vector<ElementsDataDescription> elementsData;
    elementsData.reserve(elementsDataSize);
    for (const auto& drawElemetsBuffer : drawElementsBuffers)
    {
        const auto* data = reinterpret_cast<const ElementsDataDescription*>(drawElemetsBuffer->data());
        elementsData.insert(elementsData.end(), data, data + drawElemetsBuffer->numIndices());
    }

    const auto elementsDataOffset = m_elementsDataBuffer->allocate(elementsDataSize, elementsData.data());
    return { static_cast<uint32_t>(elementsDataOffset), static_cast<uint32_t>(elementsDataSize) };
}

void SceneData::removeElementsData(uint32_t elementsDataOffset, uint32_t elementsDataSize)
{
    if ((elementsDataOffset == utils::IDsGenerator::last()) || (!elementsDataSize))
        return;
    
    m_elementsDataBuffer->free(elementsDataOffset, elementsDataSize);
}

SceneData::AddSkeletonDataResult SceneData::addSkeletonData(
    const std::vector<Bone>& bones,
    uint32_t rootBoneID,
    const std::map<std::string, std::shared_ptr<Animation>>& animations)
{
    static const size_t s_bonesCountIndex = 0;
    static const size_t s_rootBoneIDIndex = s_bonesCountIndex + 1u;
    static const size_t s_animationsCountIndex = s_rootBoneIDIndex + 1u;

    const size_t bonesOffsetsIndices = s_animationsCountIndex + 1u;
    const size_t animationsOffsetsIndices = bonesOffsetsIndices + bones.size();

    const size_t dataOffsetIndex = animationsOffsetsIndices + animations.size();

    size_t skeletonDataSize = 0u; // in floats

    skeletonDataSize += 1u; // bones count
    skeletonDataSize += 1u; // root bone ID
    skeletonDataSize += 1u; // animations count
    skeletonDataSize += bones.size(); // bones offsets
    skeletonDataSize += animations.size(); // animations offsets

    for (const auto& bone : bones)
    {
        skeletonDataSize += 8u; // offsetTransform
        skeletonDataSize += 8u; // transform
        skeletonDataSize += 1u; // parentID
        skeletonDataSize += 1u; // children count
        skeletonDataSize += bone.childrenIDs.size(); // childrenIDs
    }

    for (const auto& [_, animation] : animations)
    {
        skeletonDataSize += 1u; // duration
        skeletonDataSize += 1u; // ticksPerSecond
        skeletonDataSize += 1u; // channels count
        skeletonDataSize += animation->channels().size(); // channels offsets
        for (const auto& animationChannel : animation->channels())
        {
            skeletonDataSize += 1u; // boneID
            skeletonDataSize += 1u; // scale keys count
            skeletonDataSize += 1u; // rotation keys count
            skeletonDataSize += 1u; // translation keys count
            skeletonDataSize += animationChannel.scaleKeys.size() * 2u; // scale keys (time + scale[1])
            skeletonDataSize += animationChannel.rotationKeys.size() * 5u; // rotation keys (time + rotation[4])
            skeletonDataSize += animationChannel.translationKeys.size() * 4u; // translation keys (time + translation[3])
        }
    }

    // 3u because of the header of the description (bones count, root bone ID and animations count that in fact are empty)
    if (skeletonDataSize == 3u)
        return {};

    std::vector<SkeletonsDataDescription> skeletonData(skeletonDataSize);

    skeletonData[s_bonesCountIndex] = glm::uintBitsToFloat(static_cast<uint32_t>(bones.size()));
    skeletonData[s_rootBoneIDIndex] = glm::uintBitsToFloat(static_cast<uint32_t>(rootBoneID));
    skeletonData[s_animationsCountIndex] = glm::uintBitsToFloat(static_cast<uint32_t>(animations.size()));

    uint32_t offset = static_cast<uint32_t>(dataOffsetIndex);

    size_t boneIndex = 0u;
    for (const auto& bone : bones)
    {
        skeletonData[bonesOffsetsIndices + boneIndex++] = glm::uintBitsToFloat(offset);

        const auto offsetTransformDescription = TransformDescription::make(bone.offsetTransform);
        std::memcpy(skeletonData.data() + offset, &offsetTransformDescription, sizeof(TransformDescription));
        offset += 8u; // offsetTransform

        const auto modelTransformDescription = TransformDescription::make(bone.transform);
        std::memcpy(skeletonData.data() + offset, &modelTransformDescription, sizeof(TransformDescription));
        offset += 8u; // transform

        skeletonData[offset++] = glm::uintBitsToFloat(bone.parentID); // parentID

        skeletonData[offset++] = glm::uintBitsToFloat(static_cast<uint32_t>(bone.childrenIDs.size())); // children count

        for (const auto& childID : bone.childrenIDs)
            skeletonData[offset++] = glm::uintBitsToFloat(childID); // children IDs
    }

    size_t animationIndex = 0u;
    for (const auto& [_, animation] : animations)
    {
        skeletonData[animationsOffsetsIndices + animationIndex++] = glm::uintBitsToFloat(offset);

        skeletonData[offset++] = glm::uintBitsToFloat(animation->duration()); // duration

        skeletonData[offset++] = glm::uintBitsToFloat(animation->ticksPerSecond()); // ticksPerSecond

        skeletonData[offset++] = glm::uintBitsToFloat(static_cast<uint32_t>(animation->channels().size())); // channels count

        const size_t channelsOffsetIndex = offset;

        size_t channelIndex = 0u;
        for (const auto& animationChannel : animation->channels())
        {
            skeletonData[channelsOffsetIndex + channelIndex++] = glm::uintBitsToFloat(offset);

            skeletonData[offset++] = glm::uintBitsToFloat(animationChannel.boneID); // boneID

            // scale kyes count
            skeletonData[offset++] = glm::uintBitsToFloat(static_cast<uint32_t>(animationChannel.scaleKeys.size()));

            // rotation kyes count
            skeletonData[offset++] = glm::uintBitsToFloat(static_cast<uint32_t>(animationChannel.rotationKeys.size()));

            // translation kyes count
            skeletonData[offset++] = glm::uintBitsToFloat(static_cast<uint32_t>(animationChannel.translationKeys.size()));

            const auto& scaleKeys = animationChannel.scaleKeys;
            for (size_t i = 0u; i < scaleKeys.size(); ++i)
            {
                const auto& key = scaleKeys[i];
                skeletonData[offset++] = glm::uintBitsToFloat(key.first); // time
                skeletonData[offset++] = key.second; // scale
            }

            const auto& rotationKeys = animationChannel.rotationKeys;
            for (size_t i = 0u; i < rotationKeys.size(); ++i)
            {
                const auto& key = rotationKeys[i];
                skeletonData[offset++] = glm::uintBitsToFloat(key.first); // time
                skeletonData[offset++] = key.second.x; // r.x
                skeletonData[offset++] = key.second.y; // r.y
                skeletonData[offset++] = key.second.z; // r.z
                skeletonData[offset++] = key.second.w; // r.w
            }

            const auto& translationKeys = animationChannel.translationKeys;
            for (size_t i = 0u; i < translationKeys.size(); ++i)
            {
                const auto& key = translationKeys[i];
                skeletonData[offset++] = glm::uintBitsToFloat(key.first); // time
                skeletonData[offset++] = key.second.x; // t.x
                skeletonData[offset++] = key.second.y; // t.y
                skeletonData[offset++] = key.second.z; // t.z
            }
        }
    }

    const auto skeletonDataOffset = m_skeletonsDataBuffer->allocate(skeletonData.size(), skeletonData.data());
    return { static_cast<uint32_t>(skeletonDataOffset), static_cast<uint32_t>(skeletonData.size()) };
}

void SceneData::removeSkeletonData(uint32_t skeletonsDataOffset, uint32_t skeletonsDataSize)
{
    if ((skeletonsDataOffset == utils::IDsGenerator::last()) || (!skeletonsDataSize))
        return;
    
    m_skeletonsDataBuffer->free(skeletonsDataOffset, skeletonsDataSize);
}

SceneData::AddBonesTransformsDataResult SceneData::addBonesTransformsData(const std::vector<Bone>& bones)
{
    const auto bonesTransformsDataSize = bones.size();

    if (!bonesTransformsDataSize)
        return {};

    const auto bonesTransformsDataOffset = m_bonesTransformsDataBuffer->allocate(bonesTransformsDataSize, nullptr);
    return { static_cast<uint32_t>(bonesTransformsDataOffset), static_cast<uint32_t>(bonesTransformsDataSize) };
}

void SceneData::removeBonesTransformsData(uint32_t bonesTransformsDataOffset, uint32_t bonesTransformsDataSize)
{
    if ((bonesTransformsDataOffset == utils::IDsGenerator::last()) || (!bonesTransformsDataSize))
        return;
    
    m_bonesTransformsDataBuffer->free(bonesTransformsDataOffset, bonesTransformsDataSize);
}

SceneData::AddTextureHandleResult SceneData::addTextureHandle(
    uint32_t materialMapID,
    const graphics::PConstTexture& materialMapTexture,
    const std::shared_ptr<graphics::RendererBase>& graphicsRenderer)
{
    if ((materialMapID == utils::IDsGenerator::last()) || (!materialMapTexture) || (!graphicsRenderer))
        return {};

    auto textureHandle = graphicsRenderer->createTextureHandle(materialMapTexture);
    textureHandle->makeResident();

    m_textureHandles[materialMapID] = textureHandle;
    return { textureHandle->handle() };
}

void SceneData::removeTextureHandle(uint32_t materialMapID)
{
    m_textureHandles[materialMapID].reset();
}

std::shared_ptr<MeshHandler> SceneData::addMesh(const std::shared_ptr<const Mesh>& mesh)
{
    std::shared_ptr<MeshHandler> result;

    if (!mesh)
    {
        result = std::make_shared<MeshHandler>(weak_from_this());
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
            const auto meshID = m_meshIDsGenerator.generate();
            m_meshesBuffer->resize(static_cast<size_t>(meshID) + 1u);
            m_meshesBuffer->set(meshID, MeshDescription::makeEmpty());

            onMeshChanged(meshID, mesh->mesh(), mesh->boundingBox());

            result = std::make_shared<MeshHandler>(weak_from_this(), mesh, meshID);
            meshPrivate.handlers().insert(result);
        }
    }

    return result;
}

void SceneData::removeMesh(const std::shared_ptr<const Mesh>& mesh)
{
    auto& meshHandlers = mesh->m().handlers();

    for (auto it = meshHandlers.begin(); it != meshHandlers.end(); ++it)
    {
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            const auto meshID = (*it)->ID();
            onMeshChanged(meshID, nullptr, utils::BoundingBox::empty());
            m_meshIDsGenerator.clear(meshID);
            meshHandlers.erase(it);
            break;
        }
    }
}

void SceneData::onMeshChanged(
    utils::IDsGenerator::value_type ID,
    const std::shared_ptr<const utils::Mesh>& mesh,
    const utils::BoundingBox& bb)
{
    const auto meshDescription = m_meshesBuffer->get(ID);
    removeVerticeshData(meshDescription.verticesDataOffset, MeshDescription::verticesDataSize(meshDescription));
    removeElementsData(meshDescription.elementsDataOffset, MeshDescription::elementsDataSize(meshDescription));

    AddVerticesDataResult addVerticesDataResult;
    AddElementsDataResult addElementsDataResult;

    if (mesh)
    {
        addVerticesDataResult = addVerticesData(mesh->vertexBuffers());
        addElementsDataResult = addElementsData(mesh->primitiveSets());
    }

    m_meshesBuffer->set(ID, MeshDescription::make(
        bb,
        addVerticesDataResult.verticesDataSize,
        addElementsDataResult.elementsDataSize,
        addVerticesDataResult.verticesDataOffset,
        addElementsDataResult.elementsDataOffset,
        addVerticesDataResult.positionComponentsCount,
        addVerticesDataResult.normalComponentsCount,
        addVerticesDataResult.texCoordsComponentsCount,
        addVerticesDataResult.bonesCount,
        addVerticesDataResult.hasTangent,
        addVerticesDataResult.colorComponentsCount));
}

std::shared_ptr<MaterialMapHandler> SceneData::addMaterialMap(const std::shared_ptr<const MaterialMap>& materialMap)
{
    std::shared_ptr<MaterialMapHandler> result;

    if (!materialMap)
    {
        result = std::make_shared<MaterialMapHandler>(weak_from_this());
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
            const auto materialMapID = m_materialMapIDsGenerator.generate();
            m_materialMapsBuffer->resize(static_cast<size_t>(materialMapID) + 1u);
            m_materialMapsBuffer->set(materialMapID, MaterialMapDescription::makeEmpty());
            m_textureHandles.resize(static_cast<size_t>(materialMapID) + 1u);
            m_textureHandles[materialMapID] = nullptr;

            onMaterialMapChanged(materialMapID, materialMap->filesystemPath(), materialMap->image());

            result = std::make_shared<MaterialMapHandler>(weak_from_this(), materialMap, materialMapID);
            materialMapPrivate.handlers().insert(result);
        }
    }

    return result;
}

void SceneData::removeMaterialMap(const std::shared_ptr<const MaterialMap>& materialMap)
{
    auto& materialMapHandlers = materialMap->m().handlers();

    for (auto it = materialMapHandlers.begin(); it != materialMapHandlers.end(); ++it)
    {
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            const auto materialMapID = (*it)->ID();
            onMaterialMapChanged(materialMapID, std::filesystem::path(), nullptr);
            m_materialMapIDsGenerator.clear(materialMapID);
            materialMapHandlers.erase(it);
            break;
        }
    }
}

void SceneData::onMaterialMapChanged(
    utils::IDsGenerator::value_type ID,
    const std::filesystem::path& path,
    const std::shared_ptr<const utils::Image>& image)
{
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

    removeTextureHandle(ID);

    graphics::PTexture texture;
    if (!path.empty())
        texture = texturesManager->loadOrGetTexture(path);
    else if (image)
        texture = texturesManager->loadOrGetTexture(image);

    m_materialMapsBuffer->set(
        ID,
        MaterialMapDescription::make(addTextureHandle(ID, texture, graphicsRenderer).handle));
}

std::shared_ptr<MaterialHandler> SceneData::addMaterial(const std::shared_ptr<const Material>& material)
{
    std::shared_ptr<MaterialHandler> result;

    if (!material)
    {
        result = std::make_shared<MaterialHandler>(weak_from_this());
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
            const auto materialID = m_materialIDsGenerator.generate();
            m_materialsBuffer->resize(static_cast<size_t>(materialID) + 1u);
            m_materialsBuffer->set(materialID, MaterialDescription::makeEmpty());

            onMaterialChanged(
                materialID,
                material->baseColor(),
                material->emission(),
                material->roughness(),
                material->metalness(),
                material->alphaCutoff(),
                material->materialMap(MaterialMapTarget::BaseColor),
                material->materialMap(MaterialMapTarget::Opacity),
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
                material->isShadowCasted(),
                material->isDoubleSided());

            result = std::make_shared<MaterialHandler>(weak_from_this(), material, materialID);
            materialPrivate.handlers().insert(result);
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
            const auto materialID = (*it)->ID();
            onMaterialChanged(
                materialID,
                glm::vec4(),
                glm::vec3(),
                0.f,
                0.f,
                0.f,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                0.f,
                0.f,
                glm::u32vec3(),
                false,
                false,
                false,
                false);
            m_materialIDsGenerator.clear(materialID);
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
    const std::shared_ptr<const MaterialMap>& opacityMap,
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
    bool isShadowCasted,
    bool isDoubleSided)
{
    m_materialsBuffer->set(ID, MaterialDescription::make(
        baseColor,
        emission,
        addMaterialMap(baseColorMap)->ID(),
        addMaterialMap(opacityMap)->ID(),
        addMaterialMap(emissionMap)->ID(),
        addMaterialMap(occlusionMap)->ID(),
        addMaterialMap(roughnessMap)->ID(),
        addMaterialMap(metalnessMap)->ID(),
        addMaterialMap(normalMap)->ID(),
        roughness,
        metalness,
        occlusionMapStrength,
        normalMapScale,
        ORMSwizzleMask,
        isLighted,
        isShadowed,
        isShadowCasted,
        isDoubleSided,
        isMaterialTransparent(baseColor, baseColorMap, opacityMap, alphaCutoff),
        alphaCutoff));
}

std::shared_ptr<DrawableHandler> SceneData::addDrawable(const std::shared_ptr<const Drawable>& drawable)
{
    std::shared_ptr<DrawableHandler> result;

    if (!drawable)
    {
        result = std::make_shared<DrawableHandler>(weak_from_this());
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
            auto drawableID = m_drawableIDsGenerator.generate();
            m_drawablesBuffer->resize(static_cast<size_t>(drawableID) + 1u);
            m_drawablesBuffer->set(drawableID, DrawableDescription::makeEmpty());

            onDrawableChanged(drawableID, drawable->mesh(), drawable->material());

            result = std::make_shared<DrawableHandler>(weak_from_this(), drawable, drawableID);
            drawablePrivate.handlers().insert(result);
        }
    }

    return result;
}

void SceneData::removeDrawable(const std::shared_ptr<const Drawable>& drawable)
{
    auto& drawableHandlers = drawable->m().handlers();

    for (auto it = drawableHandlers.begin(); it != drawableHandlers.end(); ++it)
    {
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            const auto drawableID = (*it)->ID();
            onDrawableChanged(drawableID, nullptr, nullptr);
            m_drawableIDsGenerator.clear(drawableID);
            drawableHandlers.erase(it);
            break;

        }
    }
}

void SceneData::onDrawableChanged(
    utils::IDsGenerator::value_type ID,
    const std::shared_ptr<const Mesh>& mesh,
    const std::shared_ptr<const Material>& material)
{
    m_drawablesBuffer->set(ID, DrawableDescription::make(
        addMesh(mesh)->ID(),
        addMaterial(material)->ID()));
}

void SceneData::setBackground(const std::shared_ptr<const Background>& background)
{
    auto backgroundHandler = std::make_shared<BackgroundHandler>(weak_from_this());
    m_backgroundBuffer->set(BackgroundDescription::makeEmpty());

    if (background)
    {
        onBackgroundChanged(
            background->environmentMap(),
            background->rotation(),
            background->environmentColor(),
            background->blurPower());

        backgroundHandler = std::make_shared<BackgroundHandler>(weak_from_this(), background);
    }

    background->m().handler() = backgroundHandler;

}

void SceneData::removeBackground(const std::shared_ptr<const Background>& background)
{
    auto& backgroundHandler = background->m().handler();
    if (backgroundHandler && (backgroundHandler->sceneData().lock() == shared_from_this()))
    {
        onBackgroundChanged(nullptr, glm::quat(), glm::vec3(), 0.f);
        backgroundHandler.reset();
    }
}

void SceneData::onBackgroundChanged(
    const std::shared_ptr<const MaterialMap>& environmentMap,
    const glm::quat& rotation,
    const glm::vec3& environmentColor,
    float blurPower)
{
    auto mipmapLevel = 0.f;
    auto materialMapHandler = addMaterialMap(environmentMap);

    if (auto environmentMapID = materialMapHandler->ID(); environmentMapID != utils::IDsGenerator::last())
        if (auto& textureHandle = m_textureHandles[environmentMapID])
            if (auto texture = textureHandle->texture())
                mipmapLevel = glm::clamp(blurPower, 0.f, 1.f) * glm::max(0u, (texture->numMipmapLevels() - 1u));

    m_backgroundBuffer->set(BackgroundDescription::make(rotation, environmentColor, mipmapLevel, materialMapHandler->ID()));
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

void SceneData::removeLight(utils::IDsGenerator::value_type lightID)
{
    onLightChanged(lightID, LightDescription::makeEmpty());
    m_lightIDsGenerator.clear(lightID);
}

std::shared_ptr<SkeletonHandler> SceneData::addSkeleton(const std::shared_ptr<const Skeleton>& skeleton)
{
    std::shared_ptr<SkeletonHandler> result;

    if (!skeleton)
    {
        result = std::make_shared<SkeletonHandler>(weak_from_this());
    }
    else
    {
        auto& skeletonPrivate = skeleton->m();

        for (const auto& skeletonHandler : skeletonPrivate.handlers())
        {
            if (skeletonHandler->sceneData().lock() == shared_from_this())
            {
                result = skeletonHandler;
                break;
            }
        }

        if (!result)
        {
            const auto skeletonID = m_skeletonIDsGenerator.generate();
            m_skeletonsBuffer->resize(static_cast<size_t>(skeletonID) + 1u);
            m_skeletonsBuffer->set(skeletonID, SkeletonDescription::makeEmpty());

            onSkeletonChanged(skeletonID, skeleton->bones(), skeleton->rootBoneID(), skeleton->animations());

            result = std::make_shared<SkeletonHandler>(weak_from_this(), skeleton, skeletonID);
            skeletonPrivate.handlers().insert(result);
        }
    }

    return result;
}

void SceneData::removeSkeleton(const std::shared_ptr<const Skeleton>& skeleton)
{
    auto& skeletonHandlers = skeleton->m().handlers();

    for (auto it = skeletonHandlers.begin(); it != skeletonHandlers.end(); ++it)
    {
        if ((*it)->sceneData().lock() == shared_from_this())
        {
            const auto skeletonID = (*it)->ID();
            onSkeletonChanged(
                skeletonID,
                std::vector<Bone>(),
                utils::IDsGenerator::last(),
                std::map<std::string, std::shared_ptr<Animation>>());
            m_skeletonIDsGenerator.clear(skeletonID);
            break;
        }
    }
}

void SceneData::onSkeletonChanged(
    utils::IDsGenerator::value_type ID,
    const std::vector<Bone>& bones,
    uint32_t rootBoneID,
    const std::map<std::string, std::shared_ptr<Animation>>& animations)
{
    const auto skeletonDescription = m_skeletonsBuffer->get(ID);
    removeSkeletonData(skeletonDescription.dataOffset, skeletonDescription.dataSize);

    const auto addSkeletonDataResult = addSkeletonData(bones, rootBoneID, animations);
    m_skeletonsBuffer->set(
        ID,
        SkeletonDescription::make(addSkeletonDataResult.skeletonsDataOffset, addSkeletonDataResult.skeletonsDataSize));
}

std::shared_ptr<DrawDataHandler> SceneData::addDrawData(
    const std::shared_ptr<const Drawable>& drawable,
    const utils::Transform& transform)
{
    std::shared_ptr<DrawDataHandler> result;

    if (!drawable)
    {
        result = std::make_shared<DrawDataHandler>(weak_from_this());
    }
    else
    {
        const auto drawDataID = m_drawDataIDsGenerator.generate();
        m_drawDataBuffer->resize(static_cast<size_t>(drawDataID) + 1u);
        m_drawDataBuffer->set(drawDataID, DrawDataDescription::makeEmpty());

        onDrawDataChanged(drawDataID, drawable, transform);

        result = std::make_shared<DrawDataHandler>(weak_from_this(), drawDataID);
    }

    return result;
}

void SceneData::removeDrawData(utils::IDsGenerator::value_type ID)
{
    onDrawDataChanged(ID, nullptr, utils::Transform::makeIdentity());
    m_drawDataIDsGenerator.clear(ID);
}

void SceneData::onDrawDataChanged(
    utils::IDsGenerator::value_type ID,
    const std::shared_ptr<const Drawable>& drawable,
    const utils::Transform& transform)
{
    m_drawDataBuffer->set(ID, DrawDataDescription::make(transform, addDrawable(drawable)->ID()));
}

std::shared_ptr<SkeletalAnimatedDataHandler> SceneData::addSkeletalAnimatedData(
    const std::shared_ptr<Skeleton>& skeleton,
    const std::string& currentAnimation)
{
    std::shared_ptr<SkeletalAnimatedDataHandler> result;

    if (!skeleton)
    {
        result = std::make_shared<SkeletalAnimatedDataHandler>(weak_from_this());
    }
    else
    {
        const auto skeletalAnimatedDataID = m_skeletalAnimatedDataIDsGenerator.generate();
        m_skeletalAnimatedDataBuffer->resize(static_cast<size_t>(skeletalAnimatedDataID) + 1u);
        m_skeletalAnimatedDataBuffer->set(skeletalAnimatedDataID, SkeletalAnimatedDataDescription::makeEmpty());

        onSkeletalAnimatedDataChanged(skeletalAnimatedDataID, skeleton, currentAnimation);

        result = std::make_shared<SkeletalAnimatedDataHandler>(weak_from_this(), skeletalAnimatedDataID);
    }

    return result;
}

void SceneData::removeSkeletalAnimatedData(utils::IDsGenerator::value_type skeletalAnimatedDataID)
{
    onSkeletalAnimatedDataChanged(skeletalAnimatedDataID, nullptr, std::string());
    m_skeletalAnimatedDataIDsGenerator.clear(skeletalAnimatedDataID);
}

void SceneData::onSkeletalAnimatedDataChanged(
    utils::IDsGenerator::value_type skeletalAnimatedDataID,
    const std::shared_ptr<Skeleton>& skeleton,
    const std::string& currentAnimation)
{
    const auto skeletalAnimatedDataDescription = m_skeletalAnimatedDataBuffer->get(skeletalAnimatedDataID);
    removeBonesTransformsData(
        skeletalAnimatedDataDescription.bonesTransfromsDataOffset,
        skeletalAnimatedDataDescription.bonesTransfromsDataSize);

    AddBonesTransformsDataResult addBonesTransformsDataResult;
    auto currentAnimationID = utils::IDsGenerator::last();

    if (skeleton)
    {
        addBonesTransformsDataResult = addBonesTransformsData(skeleton->bones());

        const auto& animations = skeleton->animations();
        if (auto currentAnimationIter = animations.find(currentAnimation); currentAnimationIter != animations.end())
            currentAnimationID = std::distance(animations.begin(), currentAnimationIter);
    }

    m_skeletalAnimatedDataBuffer->set(
        skeletalAnimatedDataID,
        SkeletalAnimatedDataDescription::make(
            addSkeleton(skeleton)->ID(),
            currentAnimationID,
            addBonesTransformsDataResult.bonesTransformsDataOffset,
            addBonesTransformsDataResult.bonesTransformsDataSize));
}

size_t SceneData::drawDataCount() const
{
    return m_drawDataBuffer->size();
}

size_t SceneData::skeletalAnimatedDataCount() const
{
    return m_skeletalAnimatedDataBuffer->size();
}

size_t SceneData::lightsCount() const
{
    return m_lightsBuffer->size();
}

graphics::PDrawArraysIndirectCommandsConstBuffer SceneData::screenQuadCommandsBuffer() const
{
    return m_screenQuadCommandsBuffer;
}

std::shared_ptr<LightHandler> SceneData::addLight()
{
    const auto lightID = m_lightIDsGenerator.generate();
    m_lightsBuffer->resize(static_cast<size_t>(lightID) + 1u);
    m_lightsBuffer->set(lightID, LightDescription::makeEmpty());
    return std::make_shared<LightHandler>(weak_from_this(), lightID);
}

void SceneData::onLightChanged(utils::IDsGenerator::value_type ID, const LightDescription& lightDescription)
{
    m_lightsBuffer->set(ID, lightDescription);
}

bool SceneData::isMaterialTransparent(
    const glm::vec4& baseColor,
    const std::shared_ptr<const MaterialMap>& baseColorMap,
    const std::shared_ptr<const MaterialMap>& opacityMap,
    float alphaCutoff)
{
    if (alphaCutoff < utils::epsilon<float>())
    {
        if (baseColor.a < 1.f - utils::epsilon<float>())
            return true;

        if (opacityMap)
            return true;

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
    }

    return false;
}

}
}
