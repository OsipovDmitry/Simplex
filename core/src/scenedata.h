#ifndef CORE_SCENEDATA_H
#define CORE_SCENEDATA_H

#include <deque>

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

using VertexDataDescription = float;
using ElementsDescription = uint32_t;

using VertexDataBuffer = std::shared_ptr<graphics::VectorBuffer<VertexDataDescription>>;
using ElementsBuffer = std::shared_ptr<graphics::VectorBuffer<ElementsDescription>>;
using MeshesBuffer = std::shared_ptr<graphics::VectorBuffer<MeshDescription>>;
using MaterialMapsBuffer = std::shared_ptr<graphics::VectorBuffer<MaterialMapDescription>>;
using MaterialsBuffer = std::shared_ptr<graphics::VectorBuffer<MaterialDescription>>;
using DrawablesBuffer = std::shared_ptr<graphics::VectorBuffer<DrawableDescription>>;
using DrawDataBuffer = std::shared_ptr<graphics::VectorBuffer<DrawDataDescription>>;
using BackgroundBuffer = std::shared_ptr<graphics::StructBuffer<BackgroundDescription>>;
using LightsBuffer = std::shared_ptr<graphics::VectorBuffer<LightDescription>>;

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

class LightHandler : public ResourceHandler
{
public:
    LightHandler(const std::weak_ptr<SceneData>&, size_t);
    ~LightHandler() override;

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
        bool isShadowCasted,
        bool isDoubleSided);

    std::shared_ptr<DrawableHandler> addDrawable(const std::shared_ptr<const Drawable>&);
    void removeDrawable(const std::shared_ptr<const Drawable>&);
    void onDrawableChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const Mesh>&, const std::shared_ptr<const Material>&);

    std::shared_ptr<DrawDataHandler> addDrawData(const std::shared_ptr<const Drawable>&, const utils::Transform&);
    void removeDrawData(utils::IDsGenerator::value_type);
    void onDrawDataChanged(utils::IDsGenerator::value_type, const std::shared_ptr<const Drawable>&, const utils::Transform&);

    void setBackground(const std::shared_ptr<const Background>&);
    void removeBackground();
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
    LightsBuffer& lightsBuffer();
    const LightsBuffer& lightsBuffer() const;

    graphics::PDrawArraysIndirectCommandsConstBuffer screenQuadCommandsBuffer() const;

    static std::shared_ptr<SceneData> create();

private:
    SceneData();

    std::shared_ptr<LightHandler> addLight();
    void onLightChanged(utils::IDsGenerator::value_type, const LightDescription&);

    static bool isMaterialTransparent(
        const glm::vec4& baseColor,
        const std::shared_ptr<const MaterialMap>& baseColorMap,
        float alphaCutoff);

    VertexDataBuffer m_vertexDataBuffer;
    ElementsBuffer m_elementsBuffer;
    MeshesBuffer m_meshesBuffer;
    MaterialMapsBuffer m_materialMapsBuffer;
    MaterialsBuffer m_materialsBuffer;
    DrawablesBuffer m_drawablesBuffer;
    DrawDataBuffer m_drawDataBuffer;
    BackgroundBuffer m_backgroundBuffer;
    LightsBuffer m_lightsBuffer;

    graphics::PDrawArraysIndirectCommandsBuffer m_screenQuadCommandsBuffer;

    utils::IDsGenerator m_meshIDsGenerator;
    utils::IDsGenerator m_materialMapIDsGenerator;
    utils::IDsGenerator m_materialIDsGenerator;
    utils::IDsGenerator m_drawableIDsGenerator;
    utils::IDsGenerator m_drawDataIDsGenerator;
    utils::IDsGenerator m_lightIDsGenerator;

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
