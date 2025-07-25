#ifndef CORE_SCENEDATA_H
#define CORE_SCENEDATA_H

#include <deque>

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/glm/common.hpp>
#include <utils/idgenerator.h>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

using PositionDescription = glm::vec3;
using NormalDescription = glm::vec3;
using TexCoordsDescription = glm::vec2;
struct BonesDescription
{
    using IDsDescription = glm::u32vec4;
    using WeightsDescription = glm::vec4;

    IDsDescription IDs;
    WeightsDescription weights;
};
using TangentDescription = glm::vec4;
using ColorDescription = glm::vec4;
using IndexDescription = uint32_t;

struct MeshDescription
{
    uint32_t positionOffset;
    uint32_t normalOffset;
    uint32_t texCoordsOffset;
    uint32_t bonesOffset;
    uint32_t tangentOffset;
    uint32_t colorOffset;
    uint32_t indexOffset;
    uint32_t numIndices;
};
struct MaterialMapDescription
{
    graphics::TextureHandle handle;
};

struct MaterialDescription
{
    uint32_t baseColorTextureOffset;
    uint32_t emissionTextureOffset;
    uint32_t occlusionTextureOffset;
    uint32_t roughnessTextureOffset;
    uint32_t metalnessTextureOffset;
    uint32_t normalTextureOffset;
    glm::vec4 baseColor;
    glm::vec3 emission;
    float roughness;
    float metalness;
    uint32_t flags;
    //  0.. 7 - occlusionMapStrength
    //  8..15 - normalMapScale
    // 16..17 - occlusion map swizzle
    // 18..19 - roughness map swizzle
    // 20..21 - metalness map swizzle
    // 22..22 - is lighted
    // 23..23 - is shadowed
    // 24..31 - free (8 bits)
};

inline float occlusionMapStrength(uint32_t flags) { return static_cast<float>((flags >> 0u) & 0xFF) / 255.f; }
inline float normalMapScale(uint32_t flags) { return static_cast<float>((flags >> 8u) & 0xFF) / 255.f; }
inline uint32_t occlusionSwizzle(uint32_t flags) { return static_cast<uint32_t>((flags >> 16u) & 0x3); }
inline uint32_t roughnessSwizzle(uint32_t flags) { return static_cast<uint32_t>((flags >> 18u) & 0x3); }
inline uint32_t metalnessSwizzle(uint32_t flags) { return static_cast<uint32_t>((flags >> 20u) & 0x3); }
inline bool isLighted(uint32_t flags) { return (flags >> 22u) & 0x1; }
inline bool isShadowed(uint32_t flags) { return (flags >> 23u) & 0x1; }
inline uint32_t makeMaterialFlags(
    float occlusionMapStrength,
    float normalMapScale,
    const glm::u32vec3& ORMSwizzleMask,
    bool isLighted,
    bool isShadowed)
{
    uint32_t result = 0u;
    result |= (static_cast<uint32_t>(glm::clamp(occlusionMapStrength, 0.f, 1.f) * 255.f) & 0xFF) << 0u;
    result |= (static_cast<uint32_t>(glm::clamp(normalMapScale, 0.f, 1.f) * 255.f) & 0xFF) << 8u;
    result |= (ORMSwizzleMask.r & 0x3) << 16u;
    result |= (ORMSwizzleMask.g & 0x3) << 18u;
    result |= (ORMSwizzleMask.b & 0x3) << 20u;
    result |= ((isLighted ? 1u : 0u) & 0x1) << 22u;
    result |= ((isShadowed ? 1u : 0u) & 0x1) << 23u;
    return result;
}

struct DrawableDescription
{
    uint32_t meshOffset;
    uint32_t materialOffset;
};

struct DrawDataDescription
{
    glm::mat4x4 modelMatrix;
    uint32_t drawableOffset;
};

class SceneData;

class MeshHandler
{
public:
    MeshHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type id);
    ~MeshHandler();

    std::weak_ptr<const Mesh> &mesh();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<SceneData> m_sceneData;
    std::weak_ptr<const Mesh> m_mesh;
    utils::IDsGenerator::value_type m_ID;
};

class MaterialMapHandler
{
public:
    MaterialMapHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const MaterialMap>& materialMap, utils::IDsGenerator::value_type id);
    ~MaterialMapHandler();

    std::weak_ptr<const MaterialMap> &materialMap();
    utils::IDsGenerator::value_type &ID();

private:
    std::weak_ptr<SceneData> m_sceneData;
    std::weak_ptr<const MaterialMap> m_materialMap;
    utils::IDsGenerator::value_type m_ID;
};

class MaterialHandler
{
public:
    MaterialHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Material>& material, utils::IDsGenerator::value_type id);
    ~MaterialHandler();

    std::weak_ptr<const Material>& material();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<SceneData> m_sceneData;
    std::weak_ptr<const Material> m_material;
    utils::IDsGenerator::value_type m_ID;
};

class DrawableHandler
{
public:
    DrawableHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Drawable>& drawable, utils::IDsGenerator::value_type id);
    ~DrawableHandler();

    std::weak_ptr<const Drawable> &drawable();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<SceneData> m_sceneData;
    std::weak_ptr<const Drawable> m_drawable;
    utils::IDsGenerator::value_type m_ID;
};

class DrawDataHandler
{
public:
    DrawDataHandler(const std::weak_ptr<SceneData>& sceneData, size_t id);
    ~DrawDataHandler();

    size_t ID() const;

private:
    std::weak_ptr<SceneData> m_sceneData;
    size_t m_ID;
};

class SceneData : public std::enable_shared_from_this<SceneData>
{
public:
    SceneData();
    ~SceneData();

    utils::IDsGenerator::value_type addMesh(const std::shared_ptr<const Mesh>&);
    void removeMesh(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);
    void onMeshChanged(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);
    void addMeshData(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);
    void removeMeshData(const std::shared_ptr<const Mesh>&, utils::IDsGenerator::value_type);

    utils::IDsGenerator::value_type addMaterialMap(const std::shared_ptr<const MaterialMap>&);
    void removeMaterialMap(const std::shared_ptr<const MaterialMap>&, utils::IDsGenerator::value_type);
    void onMaterialMapChanged(const std::shared_ptr<const MaterialMap>&, utils::IDsGenerator::value_type);

    utils::IDsGenerator::value_type addMaterial(const std::shared_ptr<const Material>&);
    void removeMaterial(const std::shared_ptr<const Material>&, utils::IDsGenerator::value_type);
    void onMaterialChanged(const std::shared_ptr<const Material>&, utils::IDsGenerator::value_type);

    utils::IDsGenerator::value_type addDrawable(const std::shared_ptr<const Drawable>&);
    void removeDrawable(const std::shared_ptr<const Drawable>&, utils::IDsGenerator::value_type);
    void onDrawableChanged(const std::shared_ptr<const Drawable>&, utils::IDsGenerator::value_type);

    std::shared_ptr<DrawDataHandler> addDrawData(const std::shared_ptr<const Drawable>&, const glm::mat4x4&);
    void removeDrawData(size_t);
    void onDrawDataChanged(const std::shared_ptr<const Drawable>&, const glm::mat4x4&, size_t);

    std::shared_ptr<graphics::DynamicBufferT<PositionDescription>>& positionsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<NormalDescription>>& normalsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<TexCoordsDescription>>& texCoordsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<BonesDescription>>& bonesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<TangentDescription>>& tangentsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<ColorDescription>>& colorsBuffer();
    std::shared_ptr<graphics::DynamicBufferT<IndexDescription>>& indicesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<MeshDescription>>& meshesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<MaterialMapDescription>>& texturesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<MaterialDescription>>& materialBuffer();
    std::shared_ptr<graphics::DynamicBufferT<DrawableDescription>>& drawablesBuffer();
    std::shared_ptr<graphics::DynamicBufferT<DrawDataDescription>>& drawDataBuffer();

private:
    std::shared_ptr<graphics::DynamicBufferT<PositionDescription>> m_positionsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<NormalDescription>> m_normalsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<TexCoordsDescription>> m_texCoordsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<BonesDescription>> m_bonesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<TangentDescription>> m_tangentsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<ColorDescription>> m_colorsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<IndexDescription>> m_indicesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<MeshDescription>> m_meshesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<MaterialMapDescription>> m_materialMapsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<MaterialDescription>> m_materialsBuffer;
    std::shared_ptr<graphics::DynamicBufferT<DrawableDescription>> m_drawablesBuffer;
    std::shared_ptr<graphics::DynamicBufferT<DrawDataDescription>> m_drawDataBuffer;

    utils::IDsGenerator m_meshIDsGenerator;
    std::unordered_map<std::shared_ptr<const Mesh>, std::weak_ptr<MeshHandler>> m_meshes;

    utils::IDsGenerator m_materialMapIDsGenerator;
    std::unordered_map<std::shared_ptr<const MaterialMap>, std::weak_ptr<MaterialMapHandler>> m_materialMaps;
    std::unordered_map<utils::IDsGenerator::value_type, graphics::PTextureHandle> m_materialMapsHandles;

    utils::IDsGenerator m_materialIDsGenerator;
    std::unordered_map<std::shared_ptr<const Material>, std::weak_ptr<MaterialHandler>> m_materials;

    utils::IDsGenerator m_drawableIDsGenerator;
    std::unordered_map<std::shared_ptr<const Drawable>, std::weak_ptr<DrawableHandler>> m_drawables;

    std::deque<std::weak_ptr<DrawDataHandler>> m_drawData;
};

}
}

#endif // CORE_SCENEDATA_H
