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

using VertexDataDescription = float;
using ElementsDescription = uint32_t;

struct MeshDescription
{
    uint32_t vertexDataOffset;
    uint32_t numVertexData;
    uint32_t elementsOffset; // draw arrays is used if 0xFFFFFFFFu 
    uint32_t numElements;
    uint32_t flags;
    //  0.. 1 - position components count [0..3]
    //  2.. 3 - normal components count [0..3]
    //  4.. 5 - texture coords component count [0..3]
    //  6.. 8 - bones count [0..7]
    //  9.. 9 - tangent space flag [0 - no tangents, 1 - tangents + binormals flags]
    // 10..12 - color components count [0 - no colors, 1 - grayscale, 2 - grayscale,alpha, 3 - RGB, 4 - RGBA, 5..7 - not used]
    // 13..31 - free (19 bits)
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
    float roughness;
    float metalness;
    glm::vec4 baseColor;
    glm::vec4 emissionAndFlags;
    //  0.. 7 - occlusionMapStrength
    //  8..15 - normalMapScale
    // 16..17 - occlusion map swizzle
    // 18..19 - roughness map swizzle
    // 20..21 - metalness map swizzle
    // 22..22 - is lighted
    // 23..23 - is shadowed
    // 24..24 - is transparent
    // 25..31 - free (7 bits)
};

struct DrawableDescription
{
    uint32_t meshOffset;
    uint32_t materialOffset;
};

struct DrawDataBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct DrawDataDescription
{
    glm::mat4x4 modelMatrix;
    glm::mat3x4 normalMatrixAndDrawableOffset;
    // [0][0]..[2][2] - normal matrix
    // [0][3] - drawable offset
};

struct BackgroundsBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct BackgroundDescription
{
    uint32_t meshOffset;
    uint32_t environmentMapOffset;
    glm::vec4 environmentColorAndBlurPower;
};

struct DrawIndirectCommandsBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct DrawArraysIndirectCommand
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t first;
    uint32_t baseInstance;
};
struct DrawElementsIndirectCommand
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  baseVertex;
    uint32_t baseInstance;
};

using VertexDataBuffer = std::shared_ptr<graphics::DynamicBufferT<VertexDataDescription>>;
using ElementsBuffer = std::shared_ptr<graphics::DynamicBufferT<ElementsDescription>>;
using MeshesBuffer = std::shared_ptr<graphics::DynamicBufferT<MeshDescription>>;
using MaterialMapsBuffer = std::shared_ptr<graphics::DynamicBufferT<MaterialMapDescription>>;
using MaterialsBuffer = std::shared_ptr<graphics::DynamicBufferT<MaterialDescription>>;
using DrawablesBuffer = std::shared_ptr<graphics::DynamicBufferT<DrawableDescription>>;
using DrawDataBuffer = std::shared_ptr<graphics::DynamicBufferT<DrawDataDescription, DrawDataBufferReservedData>>;
using BackgroundsBuffer = std::shared_ptr<graphics::DynamicBufferT<BackgroundDescription, BackgroundsBufferReservedData>>;
using DrawIndirectArraysCommandsBuffer = std::shared_ptr<graphics::DynamicBufferT<DrawArraysIndirectCommand, DrawIndirectCommandsBufferReservedData>>;
using DrawIndirectElementsCommandsBuffer = std::shared_ptr<graphics::DynamicBufferT<DrawElementsIndirectCommand, DrawIndirectCommandsBufferReservedData>>;

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
    graphics::PTextureHandle m_materialMapHandle;
    utils::IDsGenerator::value_type m_ID;
};

using MaterialMaps = std::shared_ptr<std::unordered_map<std::shared_ptr<const MaterialMap>, std::weak_ptr<MaterialMapHandler>>>;

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
    DrawDataHandler(const std::weak_ptr<SceneData>& sceneData, size_t ID);
    ~DrawDataHandler();

    size_t ID() const;

private:
    std::weak_ptr<SceneData> m_sceneData;
    size_t m_ID;
};

class BackgroundHandler
{
public:
    BackgroundHandler(const std::weak_ptr<SceneData>& sceneData, const std::weak_ptr<const Background>& background, size_t ID);
    ~BackgroundHandler();

    std::weak_ptr<const Background> &background();
    size_t ID() const;

private:
    std::weak_ptr<SceneData> m_sceneData;
    std::weak_ptr<const Background> m_background;
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
    void removeDrawData(utils::IDsGenerator::value_type);
    void onDrawDataChanged(const std::shared_ptr<const Drawable>&, const glm::mat4x4&, utils::IDsGenerator::value_type);

    void addBackground(const std::shared_ptr<const Background>&);
    void removeBackground(size_t);
    void onBackgroundChanged(const std::shared_ptr<const Background>&, size_t);

    VertexDataBuffer& vertexDataBuffer();
    ElementsBuffer& elementsBuffer();
    MeshesBuffer& meshesBuffer();
    MaterialMapsBuffer& materialMapsBuffer();
    MaterialsBuffer& materialsBuffer();
    DrawablesBuffer& drawablesBuffer();
    DrawDataBuffer& drawDataBuffer();
    BackgroundsBuffer& backgroundsBuffer();
    DrawIndirectArraysCommandsBuffer& opaqueDrawDataCommandsBuffer();
    DrawIndirectArraysCommandsBuffer& transparentDrawDataCommandsBuffer();
    DrawIndirectArraysCommandsBuffer& backgroundsCommandsBuffer();

    //tmp
    const std::unordered_map<utils::IDsGenerator::value_type, graphics::PTextureHandle>& materialMapsHandles()
    {
        return m_materialMapsHandles;
    }

private:
    VertexDataBuffer m_vertexDataBuffer;
    ElementsBuffer m_elementsBuffer;
    MeshesBuffer m_meshesBuffer;
    MaterialMapsBuffer m_materialMapsBuffer;
    MaterialsBuffer m_materialsBuffer;
    DrawablesBuffer m_drawablesBuffer;
    DrawDataBuffer m_drawDataBuffer;
    BackgroundsBuffer m_backgroundsBuffer;
    DrawIndirectArraysCommandsBuffer m_opaqueDrawDataCommandsBuffer;
    DrawIndirectArraysCommandsBuffer m_transparentDrawDataCommandsBuffer;
    DrawIndirectArraysCommandsBuffer m_backgroundsCommandsBuffer;

    utils::IDsGenerator m_meshIDsGenerator;
    std::unordered_map<std::shared_ptr<const Mesh>, std::weak_ptr<MeshHandler>> m_meshes;

    utils::IDsGenerator m_materialMapIDsGenerator;
    MaterialMaps m_materialMaps;
    std::unordered_map<utils::IDsGenerator::value_type, graphics::PTextureHandle> m_materialMapsHandles;

    utils::IDsGenerator m_materialIDsGenerator;
    std::unordered_map<std::shared_ptr<const Material>, std::weak_ptr<MaterialHandler>> m_materials;

    utils::IDsGenerator m_drawableIDsGenerator;
    std::unordered_map<std::shared_ptr<const Drawable>, std::weak_ptr<DrawableHandler>> m_drawables;

    utils::IDsGenerator m_drawDataIDsGenerator;
    std::deque<std::weak_ptr<DrawDataHandler>> m_drawDataHandlers;

    utils::IDsGenerator m_backgroundIDsGenerator;
    std::deque<std::weak_ptr<BackgroundHandler>> m_backgroundsHandlers;
};

}
}

#endif // CORE_SCENEDATA_H
