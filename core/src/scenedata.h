#ifndef CORE_SCENEDATA_H
#define CORE_SCENEDATA_H

#include <deque>
#include <map>

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/idgenerator.h>

#include <core/forwarddecl.h>
#include <core/stateset.h>

#include "descriptions.h"

namespace simplex
{
namespace core
{

template<typename T>
class DataStore
{
public:
    using value_type = T;

    DataStore() : m_vectorBuffer(graphics::VectorBuffer<value_type>::create()) {}
    ~DataStore() {}

    size_t allocate(size_t count, const value_type* data)
    {
        auto result = std::numeric_limits<size_t>::max();

        if (!count)
            return result;

        if (auto it = findFreeBlock(count); it != m_freeBlocks.end())
        {
            result = it->first + it->second - count;
            m_vectorBuffer->set(result, data, count);

            it->second -= count;
            if (it->second == 0u)
                m_freeBlocks.erase(it);
        }
        else
        {
            result = m_vectorBuffer->size();
            m_vectorBuffer->insert(result, data, count);
        }

        return result;
    }

    size_t allocate(std::initializer_list<value_type> l) { return allocate(l.size(), l.begin()); }

    void free(size_t index, size_t count)
    {
        if (!count)
            return;

        if (index >= m_vectorBuffer->size())
        {
            LOG_CRITICAL << "Index is out of range";
        }
        if (index + count > m_vectorBuffer->size())
        {
            LOG_CRITICAL << "Count is out of range";
        }

        auto insertionResult = m_freeBlocks.insert({ index, count });
        if (!insertionResult.second)
            LOG_CRITICAL << "Free blocks already have the memory with equal offset";

        auto& iter = insertionResult.first;

        auto nextIter = std::next(insertionResult.first, 1u);
        if (nextIter != m_freeBlocks.end())
        {
            if (const auto end = iter->first + iter->second; end > nextIter->first)
            {
                LOG_CRITICAL << "The memory is not allocated to be freed";
            }
            else if (end == nextIter->first)
            {
                iter->second += nextIter->second;
                m_freeBlocks.erase(nextIter);
            }
        }

        if (iter != m_freeBlocks.begin())
        {
            auto prevIter = std::prev(insertionResult.first, 1u);
            if (const auto prevEnd = prevIter->first + prevIter->second; prevEnd > iter->first)
            {
                LOG_CRITICAL << "The memory is not allocated to be freed";
            }
            else if (prevEnd == iter->first)
            {
                prevIter->second += iter->second;
                m_freeBlocks.erase(iter);
            }
        }

        if (auto lastIter = std::prev(m_freeBlocks.end(), 1u); lastIter->first + lastIter->second == m_vectorBuffer->size())
        {
            m_vectorBuffer->resize(m_vectorBuffer->size() - lastIter->second);
            m_freeBlocks.erase(lastIter);
        }
    }

    std::shared_ptr<const graphics::IDynamicBuffer> buffer() const { return m_vectorBuffer->buffer(); }

    static std::shared_ptr<DataStore<value_type>> create() { return std::make_shared<DataStore<value_type>>(); }

private:
    std::shared_ptr<graphics::VectorBuffer<value_type>> m_vectorBuffer;
    std::map<size_t, size_t> m_freeBlocks;

    std::map<size_t, size_t>::iterator findFreeBlock(size_t count)
    {
        auto result = m_freeBlocks.end();
        auto sizeDiff = std::numeric_limits<size_t>::max();

        for (auto it = m_freeBlocks.begin(); (it != m_freeBlocks.end()) && sizeDiff; ++it)
        {
            if (it->second < count)
                continue;

            if (const auto diff = it->second - count; diff < sizeDiff)
            {
                result = it;
                sizeDiff = diff;
            }
        }

        return result;
    }
};

using VerticesDataDescription = float;
using ElementsDataDescription = uint32_t;
using SkeletonsDataDescription = float;
using BonesTransfromsDataDescription = TransformDescription;

using VerticesDataBuffer = std::shared_ptr<DataStore<VerticesDataDescription>>;
using ElementsDataBuffer = std::shared_ptr<DataStore<ElementsDataDescription>>;
using SkeletonsDataBuffer = std::shared_ptr<DataStore<SkeletonsDataDescription>>;
using BonesTransformsDataBuffer = std::shared_ptr<DataStore<BonesTransfromsDataDescription>>;

using MeshesBuffer = std::shared_ptr<graphics::VectorBuffer<MeshDescription>>;
using MaterialMapsBuffer = std::shared_ptr<graphics::VectorBuffer<MaterialMapDescription>>;
using MaterialsBuffer = std::shared_ptr<graphics::VectorBuffer<MaterialDescription>>;
using DrawablesBuffer = std::shared_ptr<graphics::VectorBuffer<DrawableDescription>>;
using BackgroundBuffer = std::shared_ptr<graphics::StructBuffer<BackgroundDescription>>;
using LightsBuffer = std::shared_ptr<graphics::VectorBuffer<LightDescription>>;
using SkeletonsBuffer = std::shared_ptr<graphics::VectorBuffer<SkeletonDescription>>;

using DrawDataBuffer = std::shared_ptr<graphics::VectorBuffer<DrawDataDescription>>;
using SkeletalAnimatedDataBuffer = std::shared_ptr<graphics::VectorBuffer<SkeletalAnimatedDataDescription>>;

class SceneData;

class ResourceHandler
{
public:
    ResourceHandler(const std::weak_ptr<SceneData>&);
    virtual ~ResourceHandler();

    std::weak_ptr<SceneData>& sceneData();

protected:
    std::weak_ptr<SceneData> m_sceneData;
};

class MeshHandler : public ResourceHandler
{
public:
    MeshHandler(const std::weak_ptr<SceneData>&, const std::weak_ptr<const Mesh>&, utils::IDsGenerator::value_type);
    MeshHandler(const std::weak_ptr<SceneData>&);
    ~MeshHandler() override;

    std::weak_ptr<const Mesh> &mesh();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<const Mesh> m_mesh;
    utils::IDsGenerator::value_type m_ID;
};

class MaterialMapHandler : public ResourceHandler
{
public:
    MaterialMapHandler(const std::weak_ptr<SceneData>&, const std::weak_ptr<const MaterialMap>&, utils::IDsGenerator::value_type);
    MaterialMapHandler(const std::weak_ptr<SceneData>&);
    ~MaterialMapHandler() override;

    std::weak_ptr<const MaterialMap> &materialMap();
    utils::IDsGenerator::value_type &ID();

private:
    std::weak_ptr<const MaterialMap> m_materialMap;
    utils::IDsGenerator::value_type m_ID;
};

class MaterialHandler : public ResourceHandler
{
public:
    MaterialHandler(const std::weak_ptr<SceneData>&, const std::weak_ptr<const Material>&, utils::IDsGenerator::value_type);
    MaterialHandler(const std::weak_ptr<SceneData>&);
    ~MaterialHandler() override;

    std::weak_ptr<const Material>& material();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<const Material> m_material;
    utils::IDsGenerator::value_type m_ID;
};

class DrawableHandler : public ResourceHandler
{
public:
    DrawableHandler(const std::weak_ptr<SceneData>&, const std::weak_ptr<const Drawable>&, utils::IDsGenerator::value_type);
    DrawableHandler(const std::weak_ptr<SceneData>&);
    ~DrawableHandler() override;

    std::weak_ptr<const Drawable> &drawable();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<const Drawable> m_drawable;
    utils::IDsGenerator::value_type m_ID;
};

class DrawDataHandler : public ResourceHandler
{
public:
    DrawDataHandler(const std::weak_ptr<SceneData>&, size_t);
    DrawDataHandler(const std::weak_ptr<SceneData>&);
    ~DrawDataHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class BackgroundHandler : public ResourceHandler
{
public:
    BackgroundHandler(const std::weak_ptr<SceneData>&, const std::weak_ptr<const Background>&);
    BackgroundHandler(const std::weak_ptr<SceneData>&);
    ~BackgroundHandler() override;

    std::weak_ptr<const Background> &background();

private:
    std::weak_ptr<const Background> m_background;
};

class LightHandler : public ResourceHandler
{
public:
    LightHandler(const std::weak_ptr<SceneData>&, size_t);
    ~LightHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class SkeletonHandler : public ResourceHandler
{
public:
    SkeletonHandler(const std::weak_ptr<SceneData>&, const std::weak_ptr<const Skeleton>&, utils::IDsGenerator::value_type);
    SkeletonHandler(const std::weak_ptr<SceneData>&);
    ~SkeletonHandler() override;

    std::weak_ptr<const Skeleton>& skeleton();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<const Skeleton> m_skeleton;
    utils::IDsGenerator::value_type m_ID;
};

class SkeletalAnimatedDataHandler : public ResourceHandler
{
public:
    SkeletalAnimatedDataHandler(const std::weak_ptr<SceneData>&, size_t);
    SkeletalAnimatedDataHandler(const std::weak_ptr<SceneData>&);
    ~SkeletalAnimatedDataHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class SceneData : public StateSet, public std::enable_shared_from_this<SceneData>
{
public:
    SceneData();
    ~SceneData();

    struct AddVerticesDataResult
    {
        uint32_t verticesDataOffset = utils::IDsGenerator::last();
        uint32_t verticesDataSize = 0u;
        uint32_t positionComponentsCount = 0u;
        uint32_t normalComponentsCount = 0u;
        uint32_t texCoordsComponentsCount = 0u;
        bool hasTangent = false;
        uint32_t bonesCount = 0u;
        uint32_t colorComponentsCount = 0u;
    };
    AddVerticesDataResult addVerticesData(const std::unordered_map<utils::VertexAttribute, std::shared_ptr<utils::VertexBuffer>>&);
    void removeVerticeshData(uint32_t, uint32_t);

    struct AddElementsDataResult
    {
        uint32_t elementsDataOffset = utils::IDsGenerator::last();
        uint32_t elementsDataSize = 0u;
    };
    AddElementsDataResult addElementsData(const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>>&);
    void removeElementsData(uint32_t, uint32_t);

    struct AddSkeletonDataResult
    {
        uint32_t skeletonsDataOffset = utils::IDsGenerator::last();
        uint32_t skeletonsDataSize = 0u;
    };
    AddSkeletonDataResult addSkeletonData(
        const std::vector<Bone>&,
        uint32_t rootBoneID,
        const std::map<std::string, std::shared_ptr<Animation>>&);
    void removeSkeletonData(uint32_t, uint32_t);

    struct AddBonesTransformsDataResult
    {
        uint32_t bonesTransformsDataOffset = utils::IDsGenerator::last();
        uint32_t bonesTransformsDataSize = 0u;
    };
    AddBonesTransformsDataResult addBonesTransformsData(const std::vector<Bone>&);
    void removeBonesTransformsData(uint32_t, uint32_t);

    struct AddTextureHandleResult
    {
        graphics::TextureHandle handle = utils::IDsGeneratorT<graphics::TextureHandle>::last();
    };
    AddTextureHandleResult addTextureHandle(uint32_t, const graphics::PConstTexture&, const std::shared_ptr<graphics::RendererBase>&);
    void removeTextureHandle(uint32_t);

    std::shared_ptr<MeshHandler> addMesh(const std::shared_ptr<const Mesh>&);
    void removeMesh(const std::shared_ptr<const Mesh>&);
    void onMeshChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const utils::Mesh>&, const utils::BoundingBox&);

    std::shared_ptr<MaterialMapHandler> addMaterialMap(const std::shared_ptr<const MaterialMap>&);
    void removeMaterialMap(const std::shared_ptr<const MaterialMap>&);
    void onMaterialMapChanged(utils::IDsGenerator::value_type, const std::filesystem::path&, const std::shared_ptr<const utils::Image>&);

    std::shared_ptr<MaterialHandler> addMaterial(const std::shared_ptr<const Material>&);
    void removeMaterial(const std::shared_ptr<const Material>&);
    void onMaterialChanged(
        utils::IDsGenerator::value_type,
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
        bool isDoubleSided);

    std::shared_ptr<DrawableHandler> addDrawable(const std::shared_ptr<const Drawable>&);
    void removeDrawable(const std::shared_ptr<const Drawable>&);
    void onDrawableChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const Mesh>&, const std::shared_ptr<const Material>&);

    void setBackground(const std::shared_ptr<const Background>&);
    void removeBackground(const std::shared_ptr<const Background>&);
    void onBackgroundChanged(const std::shared_ptr<const MaterialMap>&, const glm::quat&, const glm::vec3&, float);

    std::shared_ptr<LightHandler> addPointLight(const utils::Transform&, const glm::vec3&, const glm::vec2&);
    void onPointLightChanged(
        utils::IDsGenerator::value_type,
        const utils::Transform&,
        const glm::vec3&,
        const glm::vec2&);

    std::shared_ptr<LightHandler> addSpotLight(
        const utils::Transform&,
        const glm::vec3&,
        const glm::vec2&,
        const glm::vec2&);
    void onSpotLightChanged(
        utils::IDsGenerator::value_type,
        const utils::Transform&,
        const glm::vec3&,
        const glm::vec2&,
        const glm::vec2&);

    std::shared_ptr<LightHandler> addDirectionalLight(const utils::Transform&, const glm::vec3&);
    void onDirectionalLightChanged(
        utils::IDsGenerator::value_type,
        const utils::Transform&,
        const glm::vec3&);

    std::shared_ptr<LightHandler> addImageBasedLight(
        const utils::Transform&,
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        float);
    void onImageBasedLightChanged(
        utils::IDsGenerator::value_type,
        const utils::Transform&,
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        float);

    void removeLight(utils::IDsGenerator::value_type);

    std::shared_ptr<SkeletonHandler> addSkeleton(const std::shared_ptr<const Skeleton>&);
    void removeSkeleton(const std::shared_ptr<const Skeleton>&);
    void onSkeletonChanged(
        utils::IDsGenerator::value_type,
        const std::vector<Bone>&,
        uint32_t,
        const std::map<std::string, std::shared_ptr<Animation>>&);

    std::shared_ptr<DrawDataHandler> addDrawData(const std::shared_ptr<const Drawable>&, const utils::Transform&);
    void removeDrawData(utils::IDsGenerator::value_type);
    void onDrawDataChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const Drawable>&, const utils::Transform&);

    std::shared_ptr<SkeletalAnimatedDataHandler> addSkeletalAnimatedData(const std::shared_ptr<Skeleton>&, const std::string&);
    void removeSkeletalAnimatedData(utils::IDsGenerator::value_type);
    void onSkeletalAnimatedDataChanged(utils::IDsGenerator::value_type, const std::shared_ptr<Skeleton>&, const std::string&);

    size_t drawDataCount() const;
    size_t skeletalAnimatedDataCount() const;
    size_t lightsCount() const;

    graphics::PDrawArraysIndirectCommandsConstBuffer screenQuadCommandsBuffer() const;

private:
    std::shared_ptr<LightHandler> addLight();
    void onLightChanged(utils::IDsGenerator::value_type, const LightDescription&);

    static bool isMaterialTransparent(
        const glm::vec4& baseColor,
        const std::shared_ptr<const MaterialMap>& baseColorMap,
        const std::shared_ptr<const MaterialMap>& opacityMap,
        float alphaCutoff);

    VerticesDataBuffer m_verticesDataBuffer;
    ElementsDataBuffer m_elementsDataBuffer;
    SkeletonsDataBuffer m_skeletonsDataBuffer;
    BonesTransformsDataBuffer m_bonesTransformsDataBuffer;
    std::deque<graphics::PTextureHandle> m_textureHandles;

    MeshesBuffer m_meshesBuffer;
    MaterialMapsBuffer m_materialMapsBuffer;
    MaterialsBuffer m_materialsBuffer;
    DrawablesBuffer m_drawablesBuffer;
    BackgroundBuffer m_backgroundBuffer;
    LightsBuffer m_lightsBuffer;
    SkeletonsBuffer m_skeletonsBuffer;

    DrawDataBuffer m_drawDataBuffer;
    SkeletalAnimatedDataBuffer m_skeletalAnimatedDataBuffer;

    graphics::PDrawArraysIndirectCommandsBuffer m_screenQuadCommandsBuffer;

    utils::IDsGenerator m_meshIDsGenerator;
    utils::IDsGenerator m_materialMapIDsGenerator;
    utils::IDsGenerator m_materialIDsGenerator;
    utils::IDsGenerator m_drawableIDsGenerator;
    utils::IDsGenerator m_lightIDsGenerator;
    utils::IDsGenerator m_skeletonIDsGenerator;

    utils::IDsGenerator m_drawDataIDsGenerator;
    utils::IDsGenerator m_skeletalAnimatedDataIDsGenerator;

    std::shared_ptr<Mesh> m_screenQuadMesh;
    std::shared_ptr<Mesh> m_pointLightAreaMesh;
    std::shared_ptr<Mesh> m_spotLightAreaMesh;
    std::shared_ptr<Mesh> m_directionalLightAreaMesh;
};

}
}

#endif // CORE_SCENEDATA_H
