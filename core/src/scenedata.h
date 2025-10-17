#ifndef CORE_SCENEDATA_H
#define CORE_SCENEDATA_H

#include <deque>

#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/glm/common.hpp>
#include <utils/idgenerator.h>

#include <core/forwarddecl.h>
#include <core/stateset.h>

namespace simplex
{
namespace core
{

using VertexDataDescription = float;
using ElementsDescription = uint32_t;

struct MeshDescription
{
    glm::vec4 boundingBoxMinPointAndNumVertexData;
    glm::vec4 boundingBoxMaxPointAndNumElements;
    uint32_t vertexDataOffset;
    uint32_t elementsOffset; // draw arrays is used if 0xFFFFFFFFu
    uint32_t flags;
    //  0.. 1 - position components count [0..3]
    //  2.. 3 - normal components count [0..3]
    //  4.. 5 - texture coords component count [0..3]
    //  6.. 8 - bones count [0..7]
    //  9.. 9 - tangent space flag [0 - no tangents, 1 - tangents + binormals flags]
    // 10..12 - color components count [0 - no colors, 1 - grayscale, 2 - grayscale,alpha, 3 - RGB, 4 - RGBA, 5..7 - not used]
    // 13..31 - free (19 bits)
    uint32_t padding[1u];

    static uint32_t makeFlags(
        uint32_t numPositionComponents,
        uint32_t numNormalComponents,
        uint32_t numTexCoordsComponents,
        uint32_t numBones,
        bool hasTangent,
        uint32_t numColorComponents);
    static uint32_t numVertexData(const MeshDescription&);
    static uint32_t numElements(const MeshDescription&);
};

struct MaterialMapDescription
{
    graphics::TextureHandle handle;
};

struct MaterialDescription
{
    glm::vec4 baseColor;
    glm::vec4 emission;
    uint32_t baseColorTextureID;
    uint32_t emissionTextureID;
    uint32_t occlusionTextureID;
    uint32_t roughnessTextureID;
    uint32_t metalnessTextureID;
    uint32_t normalTextureID;
    uint32_t flags0; // 0.. 7 - roughness, 8..15 - metalness, 16..23 - occlusionMapStrength, 24..31 - normalMapScale
    uint32_t flags1;
    //  0.. 1 - occlusion map swizzle
    //  2.. 3 - roughness map swizzle
    //  4.. 5 - metalness map swizzle
    //  6.. 6 - is lighted
    //  7.. 7 - is shadowed
    //  8.. 8 - is transparent
    //  9.. 9 - is double sided
    // 10..17 - alpha cutoff
    // 18..31 - free (14 bits)

    static uint32_t makeFlags0(
        float roughness,
        float metalness,
        float occlusionMapStrength,
        float normalMapScale);
    static inline uint32_t makeFlags1(
        const glm::u32vec3& ORMSwizzleMask,
        bool isLighted,
        bool isShadowed,
        bool isTransparent,
        bool isDoubleSided,
        float alphaCutoff);
    static bool isTransparent(const glm::vec4& baseColor, const std::shared_ptr<const MaterialMap>& baseColorMap);
};

struct DrawableDescription
{
    uint32_t meshID;
    uint32_t materialID;
};

struct DrawDataBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct DrawDataDescription
{
    glm::mat4x4 modelMatrix;
    glm::mat3x4 normalMatrixAndDrawableID;
    // [0][0]..[2][2] - normal matrix
    // [0][3] - drawable ID

    static glm::mat3x4 makeNormalMatrixAndDrawableID(const glm::mat4x4&, utils::IDsGenerator::value_type);
};

struct BackgroundDescription
{
    glm::vec4 environmentColorAndBlurPower;
    uint32_t environmentMapID;

    static glm::vec4 makeEnvironmentColorAndBlurPower(const glm::vec3&, float);
};

struct LightBufferReservedData { uint32_t count; uint32_t areaMeshID; uint32_t padding[2u]; };

struct PointLightDescription
{
    glm::mat4x4 modelMatrix;
    glm::vec4 areaScaleAndInnerRadius;
    glm::vec4 colorAndOuterRadius;

    static glm::vec4 makeAreaScaleAndInnerRadius(const glm::vec3&, float);
    static glm::vec4 makeColorAndOuterRadius(const glm::vec3&, float);
};

struct SpotLightDescription
{
    glm::mat4x4 modelMatrix;
    glm::vec4 areaScaleAndInnerRadius;
    glm::vec4 colorAndOuterRadius;
    glm::vec2 cosHalfAngles;

    static glm::vec4 makeAreaScaleAndInnerRadius(const glm::vec3&, float);
    static glm::vec4 makeColorAndOuterRadius(const glm::vec3&, float);
};

struct DirectionalLightDescription
{
    glm::vec4 direction;
    glm::vec4 color;

    static glm::vec4 makeDirection(const glm::mat4x4&);
    static glm::vec4 makeColor(const glm::vec3&);
};

struct ImageBasedLightDescription
{
    uint32_t BRDFLutTextureID;
    uint32_t diffuseTextureID;
    uint32_t specularTextureID;
    float contribution;
};

using VertexDataBuffer = std::shared_ptr<graphics::VectorBuffer<VertexDataDescription>>;
using ElementsBuffer = std::shared_ptr<graphics::VectorBuffer<ElementsDescription>>;
using MeshesBuffer = std::shared_ptr<graphics::VectorBuffer<MeshDescription>>;
using MaterialMapsBuffer = std::shared_ptr<graphics::VectorBuffer<MaterialMapDescription>>;
using MaterialsBuffer = std::shared_ptr<graphics::VectorBuffer<MaterialDescription>>;
using DrawablesBuffer = std::shared_ptr<graphics::VectorBuffer<DrawableDescription>>;
using DrawDataBuffer = std::shared_ptr<graphics::VectorBuffer<DrawDataDescription, DrawDataBufferReservedData>>;
using BackgroundBuffer = std::shared_ptr<graphics::StructBuffer<BackgroundDescription>>;
using PointLightsBuffer = std::shared_ptr<graphics::VectorBuffer<PointLightDescription, LightBufferReservedData>>;
using SpotLightsBuffer = std::shared_ptr<graphics::VectorBuffer<SpotLightDescription, LightBufferReservedData>>;
using DirectionalLightsBuffer = std::shared_ptr<graphics::VectorBuffer<DirectionalLightDescription, LightBufferReservedData>>;
using ImageBasedLightsBuffer = std::shared_ptr<graphics::VectorBuffer<ImageBasedLightDescription, LightBufferReservedData>>;

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
    ~DrawDataHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class BackgroundHandler : public ResourceHandler
{
public:
    BackgroundHandler(const std::weak_ptr<SceneData>&, const std::weak_ptr<const Background>&);
    ~BackgroundHandler() override;

    std::weak_ptr<const Background> &background();

private:
    std::weak_ptr<const Background> m_background;
};

class PointLightHandler : public ResourceHandler
{
public:
    PointLightHandler(const std::weak_ptr<SceneData>&, size_t);
    ~PointLightHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class SpotLightHandler : public ResourceHandler
{
public:
    SpotLightHandler(const std::weak_ptr<SceneData>&, size_t);
    ~SpotLightHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class DirectionalLightHandler : public ResourceHandler
{
public:
    DirectionalLightHandler(const std::weak_ptr<SceneData>&, size_t);
    ~DirectionalLightHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class ImageBasedLightHandler : public ResourceHandler
{
public:
    ImageBasedLightHandler(const std::weak_ptr<SceneData>&, size_t);
    ~ImageBasedLightHandler() override;

    size_t ID() const;

private:
    size_t m_ID;
};

class SceneData : public StateSet, public std::enable_shared_from_this<SceneData>
{
public:
    ~SceneData();

    std::shared_ptr<MeshHandler> addMesh(const std::shared_ptr<const Mesh>&);
    void removeMesh(const std::shared_ptr<const Mesh>&);
    void onMeshChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const utils::Mesh>&, const utils::BoundingBox&);
    void addMeshData(utils::IDsGenerator::value_type, const std::shared_ptr<const utils::Mesh>&, const utils::BoundingBox&);
    void removeMeshData(utils::IDsGenerator::value_type);

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
        bool isDoubleSided);

    std::shared_ptr<DrawableHandler> addDrawable(const std::shared_ptr<const Drawable>&);
    void removeDrawable(const std::shared_ptr<const Drawable>&);
    void onDrawableChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const Mesh>&, const std::shared_ptr<const Material>&);

    std::shared_ptr<DrawDataHandler> addDrawData(const std::shared_ptr<const Drawable>&, const glm::mat4x4&);
    void removeDrawData(utils::IDsGenerator::value_type);
    void onDrawDataChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const Drawable>&, const glm::mat4x4&);

    void setBackground(const std::shared_ptr<const Background>&);
    void removeBackground();
    void onBackgroundChanged(const std::shared_ptr<const MaterialMap>&, const glm::vec3&, float);

    std::shared_ptr<PointLightHandler> addPointLight(const glm::mat4x4&, const glm::vec3&, const glm::vec3&, const glm::vec2&);
    void removePointLight(utils::IDsGenerator::value_type);
    void onPointLightChanged(
        utils::IDsGenerator::value_type,
        const glm::mat4x4&,
        const glm::vec3&,
        const glm::vec3&,
        const glm::vec2&);

    std::shared_ptr<SpotLightHandler> addSpotLight(const glm::mat4x4&, const glm::vec3&, const glm::vec3&, const glm::vec2&, const glm::vec2&);
    void removeSpotLight(utils::IDsGenerator::value_type);
    void onSpotLightChanged(
        utils::IDsGenerator::value_type,
        const glm::mat4x4&,
        const glm::vec3&,
        const glm::vec3&,
        const glm::vec2&,
        const glm::vec2&);

    std::shared_ptr<DirectionalLightHandler> addDirectionalLight(const glm::mat4x4&, const glm::vec3&);
    void removeDirectionalLight(utils::IDsGenerator::value_type);
    void onDirectionalLightChanged(
        utils::IDsGenerator::value_type,
        const glm::mat4x4&,
        const glm::vec3&);

    std::shared_ptr<ImageBasedLightHandler> addImageBasedLight(
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        float);
    void removeImageBasedLight(utils::IDsGenerator::value_type);
    void onImageBasedLightChanged(
        utils::IDsGenerator::value_type,
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        const std::shared_ptr<const MaterialMap>&,
        float);

    VertexDataBuffer& vertexDataBuffer();
    const VertexDataBuffer& vertexDataBuffer() const;
    ElementsBuffer& elementsBuffer();
    const ElementsBuffer& elementsBuffer() const;
    MeshesBuffer& meshesBuffer();
    const MeshesBuffer& meshesBuffer() const;
    MaterialMapsBuffer& materialMapsBuffer();
    const MaterialMapsBuffer& materialMapsBuffer() const;
    MaterialsBuffer& materialsBuffer();
    const MaterialsBuffer& materialsBuffer() const;
    DrawablesBuffer& drawablesBuffer();
    const DrawablesBuffer& drawablesBuffer() const;
    DrawDataBuffer& drawDataBuffer();
    const DrawDataBuffer& drawDataBuffer() const;
    BackgroundBuffer& backgroundBuffer();
    const BackgroundBuffer& backgroundBuffer() const;
    PointLightsBuffer& pointLightsBuffer();
    const PointLightsBuffer& pointLightsBuffer() const;
    SpotLightsBuffer& spotLightsBuffer();
    const SpotLightsBuffer& spotLightsBuffer() const;
    DirectionalLightsBuffer& directionalLightsBuffer();
    const DirectionalLightsBuffer& directionalLightsBuffer() const;
    ImageBasedLightsBuffer& imageBasedLightsBuffer();
    const ImageBasedLightsBuffer& imageBasedLightsBuffer() const;

    graphics::PDrawArraysIndirectCommandsConstBuffer screenQuadCommandsBuffer() const;

    static std::shared_ptr<SceneData> create();

private:
    SceneData();

    VertexDataBuffer m_vertexDataBuffer;
    ElementsBuffer m_elementsBuffer;
    MeshesBuffer m_meshesBuffer;
    MaterialMapsBuffer m_materialMapsBuffer;
    MaterialsBuffer m_materialsBuffer;
    DrawablesBuffer m_drawablesBuffer;
    DrawDataBuffer m_drawDataBuffer;
    BackgroundBuffer m_backgroundBuffer;
    PointLightsBuffer m_pointLightsBuffer;
    SpotLightsBuffer m_spotLightsBuffer;
    DirectionalLightsBuffer m_directionalLightsBuffer;
    ImageBasedLightsBuffer m_imageBasedLightsBuffer;

    graphics::PDrawArraysIndirectCommandsBuffer m_screenQuadCommandsBuffer;

    utils::IDsGenerator m_meshIDsGenerator;
    utils::IDsGenerator m_materialMapIDsGenerator;
    utils::IDsGenerator m_materialIDsGenerator;
    utils::IDsGenerator m_drawableIDsGenerator;
    utils::IDsGenerator m_drawDataIDsGenerator;
    utils::IDsGenerator m_pointLightIDsGenerator;
    utils::IDsGenerator m_spotLightIDsGenerator;
    utils::IDsGenerator m_directionalLightIDsGenerator;
    utils::IDsGenerator m_imageBasedLightIDsGenerator;

    std::deque<graphics::PTextureHandle> m_textureHandles;
    std::weak_ptr<BackgroundHandler> m_backgroundHandler;

    std::shared_ptr<Mesh> m_screenQuadMesh;
    std::shared_ptr<Mesh> m_pointLightAreaMesh;
    std::shared_ptr<Mesh> m_spotLightAreaMesh;
    std::shared_ptr<Mesh> m_directionalLightAreaMesh;
};

}
}

#endif // CORE_SCENEDATA_H
