#ifndef CORE_DESCRIPTIONS_H
#define CORE_DESCRIPTIONS_H

#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/glm/ext/quaternion_float.hpp>
#include <utils/forwarddecl.h>

namespace simplex
{
namespace core
{

struct QuatDescription
{
    glm::vec4 q;

    static QuatDescription make(const glm::quat&);
};

struct TransformDescription
{
    QuatDescription rotation;
    glm::vec4 translationAndScale;

    static TransformDescription makeEmpty();
    static TransformDescription make(const utils::Transform&);
};

struct ClipSpaceDescription
{
    glm::vec4 params;
    uint32_t type;
    uint32_t padding[3u];

    static ClipSpaceDescription make(const utils::ClipSpace&);
};

struct CameraDescription
{
    glm::uvec4 clusterMaxSize;
    TransformDescription viewTransform;
    ClipSpaceDescription clipSpace;
    glm::vec2 cullPlaneLimits;
    glm::uvec2 ZRange; // uvec2 for atomic operations
    TransformDescription viewTransformInverted;
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 projectionMatrixInverted;
    glm::mat4x4 viewProjectionMatrix;
    glm::mat4x4 viewProjectionMatrixInverted;
    glm::vec4 viewPosition;
    glm::vec4 viewXDirection;
    glm::vec4 viewYDirection;
    glm::vec4 viewZDirection;

    static CameraDescription make(
        const glm::uvec3& clusterMaxSize,
        const utils::Transform&,
        const utils::ClipSpace&,
        const utils::Range&);
};

struct ClusterNodeDescription
{
    glm::vec4 boundingBoxMinPoint;
    glm::vec4 boundingBoxMaxPoint;
    uint32_t firstLightNodeID;
    uint32_t padding[3u];
};

struct LightNodeDescription
{
    uint32_t lightID;
    uint32_t nextID;
    uint32_t padding[2u];
};

struct SceneInfoDescription
{
    uint32_t drawDataCount;
    uint32_t lightsCount;
    uint32_t opaqueCommandsCount;
    uint32_t transparentCommandsCount;
    uint32_t lightNodesMaxCount;
    uint32_t lightNodesCount;
    uint32_t firstGlobalLightNodeID;
    uint32_t padding[1u];

    static SceneInfoDescription make(
        uint32_t drawDataCount,
        uint32_t lightsCount,
        uint32_t lightNodesMaxCount);
};

struct GBufferDescription
{
    graphics::TextureHandle colorTextureHandle;
    graphics::TextureHandle depthTextureHandle;
    graphics::ImageHandle OITIndicesImageeHandle;
    graphics::TextureHandle finalTextureHandle;
    glm::uvec2 size;
    uint32_t OITNodesMaxCount;
    uint32_t OITNodesCount;

    static GBufferDescription make(
        graphics::TextureHandle colorTextureHandle,
        graphics::TextureHandle depthTextureHandle,
        graphics::ImageHandle OITIndicesImageHandle,
        graphics::TextureHandle finalTextureHandle,
        const glm::uvec2& size,
        uint32_t OITNodesMaxCount);
};

struct OITNodeDescription
{
    glm::u32vec4 PBRData;
    float depth;
    uint32_t nextID;
    uint32_t padding[2u];
};

struct MeshDescription
{
    glm::vec4 boundingBoxMinPointAndNumVertexData;
    glm::vec4 boundingBoxMaxPointAndNumElements;
    uint32_t vertexDataOffset;
    uint32_t elementsOffset; // draw arrays is used if 0xFFFFFFFFu
    uint32_t bonesDataOffset;
    uint32_t flags;
    //  0.. 1 - position components count [0..3]
    //  2.. 3 - normal components count [0..3]
    //  4.. 5 - texture coords component count [0..3]
    //  6.. 8 - bones count [0..7]
    //  9.. 9 - tangent space flag [0 - no tangents, 1 - tangents + binormals flags]
    // 10..12 - color components count [0 - no colors, 1 - grayscale, 2 - grayscale,alpha, 3 - RGB, 4 - RGBA, 5..7 - not used]
    // 13..31 - free (19 bits)
    
    //uint32_t padding[0u];

    static MeshDescription make(
        const utils::BoundingBox&,
        uint32_t numVertexData,
        uint32_t numElements,
        uint32_t vertexDataOffset,
        uint32_t elementsOffset,
        uint32_t bonesDataOffset,
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
    graphics::TextureHandle textureHandle;

    static MaterialMapDescription make(graphics::TextureHandle);
};

struct MaterialDescription
{
    glm::vec4 baseColor;
    glm::vec4 emission;
    uint32_t baseColorMapID;
    uint32_t emissionMapID;
    uint32_t occlusionMapID;
    uint32_t roughnessMapID;
    uint32_t metalnessMapID;
    uint32_t normalMapID;
    uint32_t flags0; // 0.. 7 - roughness, 8..15 - metalness, 16..23 - occlusionMapStrength, 24..31 - normalMapScale
    uint32_t flags1;
    //  0.. 1 - occlusion map swizzle
    //  2.. 3 - roughness map swizzle
    //  4.. 5 - metalness map swizzle
    //  6.. 6 - is lighted
    //  7.. 7 - is shadowed
    //  8.. 8 - is shadow casted
    //  9.. 9 - is double sided
    // 10..10 - is transparent
    // 11..18 - alpha cutoff
    // 19..31 - free (13 bits)

    static MaterialDescription make(
        const glm::vec4& baseColor,
        const glm::vec4& emission,
        uint32_t baseColorMapID,
        uint32_t emissionMapID,
        uint32_t occlusionMapID,
        uint32_t roughnessMapID,
        uint32_t metalnessMapID,
        uint32_t normalMapID,
        float roughness,
        float metalness,
        float occlusionMapStrength,
        float normalMapScale,
        const glm::u32vec3& ORMSwizzleMask,
        bool isLighted,
        bool isShadowed,
        bool isShadowCasted,
        bool isDoubleSided,
        bool isTransparent,
        float alphaCutoff);
};

struct DrawableDescription
{
    uint32_t meshID;
    uint32_t materialID;
    uint32_t padding[2u];

    static DrawableDescription make(uint32_t meshID, uint32_t materialID);
};

struct DrawDataBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct DrawDataDescription
{
    TransformDescription transform;
    uint32_t drawableID;
    uint32_t padding[3u];

    static DrawDataDescription makeEmpty();
    static DrawDataDescription make(const utils::Transform& transform, uint32_t drawableID);
};

struct BackgroundDescription
{
    QuatDescription rotation;
    glm::vec4 environmentColorAndBlurPower;
    uint32_t environmentMapID;
    uint32_t padding[3u];

    static BackgroundDescription makeEmpty();
    static BackgroundDescription make(
        const glm::quat& rotation,
        const glm::vec3& environmentColor,
        float blurPower,
        uint32_t environmentMapID);
};

struct LightBufferReservedData { uint32_t count; uint32_t padding[3u]; };
struct LightDescription
{
    TransformDescription transform;
    glm::vec4 flags0;
    glm::vec4 flags1;

    static LightDescription makeEmpty();
    static LightDescription makePoint(const utils::Transform&, const glm::vec3&, const glm::vec2&);
    static LightDescription makeSpot(const utils::Transform&, const glm::vec3&, const glm::vec2&, const glm::vec2&);
    static LightDescription makeDirectional(const utils::Transform&, const glm::vec3&);
    static LightDescription makeImageBased(const utils::Transform&, uint32_t, uint32_t, uint32_t, float);
};

}
}

#endif // CORE_DESCRIPTIONS_H
