#ifndef CORE_SCENEDATA_H
#define CORE_SCENEDATA_H

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

class MeshHandler
{
public:
    MeshHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const Mesh>& mesh, utils::IDsGenerator::value_type id);
    ~MeshHandler();

    std::weak_ptr<const Mesh> &mesh();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<Scene> m_scene;
    std::weak_ptr<const Mesh> m_mesh;
    utils::IDsGenerator::value_type m_ID;
};

class MaterialMapHandler
{
public:
    MaterialMapHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const MaterialMap>& materialMap, utils::IDsGenerator::value_type id);
    ~MaterialMapHandler();

    std::weak_ptr<const MaterialMap> &materialMap();
    utils::IDsGenerator::value_type &ID();

private:
    std::weak_ptr<Scene> m_scene;
    std::weak_ptr<const MaterialMap> m_materialMap;
    utils::IDsGenerator::value_type m_ID;
};

class MaterialHandler
{
public:
    MaterialHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const Material>& material, utils::IDsGenerator::value_type id);
    ~MaterialHandler();

    std::weak_ptr<const Material>& material();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<Scene> m_scene;
    std::weak_ptr<const Material> m_material;
    utils::IDsGenerator::value_type m_ID;
};

class DrawableHandler
{
public:
    DrawableHandler(const std::weak_ptr<Scene>& scene, const std::weak_ptr<const Drawable>& drawable, utils::IDsGenerator::value_type id);
    ~DrawableHandler();

    std::weak_ptr<const Drawable> &drawable();
    utils::IDsGenerator::value_type ID() const;

private:
    std::weak_ptr<Scene> m_scene;
    std::weak_ptr<const Drawable> m_drawable;
    utils::IDsGenerator::value_type m_ID;
};

class DrawDataHandler
{
public:
    DrawDataHandler(const std::weak_ptr<Scene>& scene, size_t id);
    ~DrawDataHandler();

    size_t ID() const;

private:
    std::weak_ptr<Scene> m_scene;
    size_t m_ID;
};

}
}

#endif // CORE_SCENEDATA_H
