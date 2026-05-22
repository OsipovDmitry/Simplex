#ifndef CORE_DESCRIPTIONS_H
#define CORE_DESCRIPTIONS_H

#include <utils/forwarddecl.h>
#include <utils/glm/ext/quaternion_float.hpp>
#include <utils/glm/mat4x4.hpp>
#include <utils/glm/vec2.hpp>
#include <utils/glm/vec3.hpp>
#include <utils/glm/vec4.hpp>

#include <core/forwarddecl.h>

namespace simplex
{
namespace core
{

struct QuatDescription
{
    glm::vec4 q;

    static QuatDescription make(const glm::quat&);
};

struct RangeDescription
{
    glm::vec2 r;
    uint32_t padding[2u];

    static RangeDescription make(const utils::Range&);
};

struct PlaneDescription
{
    glm::vec4 p;

    static PlaneDescription make(const utils::Plane&);
};

struct BoundingBoxDescription
{
    glm::vec4 minPoint;
    glm::vec4 maxPoint;

    // uint32_t padding[0u];

    static BoundingBoxDescription make(const utils::BoundingBox&);
};

struct OrientedBoundingBoxDescription
{
    QuatDescription rotation;
    glm::vec4 translation;
    glm::vec4 halfSizes;

    // uint32_t padding[0u];

    static OrientedBoundingBoxDescription make(const utils::OrientedBoundingBox&);
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
    glm::uvec4 clusterSize;
    TransformDescription viewTransform;
    TransformDescription viewTransformInverted;
    ClipSpaceDescription clipSpace;
    RangeDescription ZRange;
    glm::mat4x4 projectionMatrix;
    glm::mat4x4 projectionMatrixInverted;
    glm::mat4x4 viewProjectionMatrix;
    glm::mat4x4 viewProjectionMatrixInverted;
    glm::vec4 viewPosition;
    glm::vec4 viewXDirection;
    glm::vec4 viewYDirection;
    glm::vec4 viewZDirection;
    glm::vec4 frustumPoints[8u];
    glm::vec4 frustumFaceNormalLinesAndRanges0[5u];
    glm::vec4 frustumFaceNormalLinesAndRanges1[5u];
    PlaneDescription frustumPlanes[6u];
    glm::vec4 frustumEdgeDirections[6u];

    // uint32_t padding[0u];
};

struct ClusterNodeDescription
{
    BoundingBoxDescription boundingBox;
    uint32_t firstLightNodeID;

    uint32_t padding[3u];
};

struct ClusterLocalLightDescription
{
    uint32_t lightID;

    uint32_t padding[3u];
};

struct LightNodeDescription
{
    uint32_t lightID;
    uint32_t nextID;
    uint32_t padding[2u];
};

struct RenderInfoDescription
{
    // global
    uint32_t time;
    float dielectricSpecular;

    uint32_t globalPadding[2u];

    // scene
    OrientedBoundingBoxDescription globalBoundingBox;
    uint32_t drawDataCount;
    uint32_t skeletalAnimatedDataCount;
    uint32_t shadowsCount;
    uint32_t lightsCount;

    // uint32_t scenePadding[0u]; // graphics::TextureHandle is uvec2 (uint64_t)

    // shadow
    static const size_t ShadowBlurKernelSize = 32u; // no affects the padding

    graphics::TextureHandle shadowVarianceBluredTextureHandle;
    graphics::TextureHandle shadowColorBluredTextureHandle;
    float shadowBlurKernel[ShadowBlurKernelSize];
    uint32_t shadowBlurRadius;
    float shadowLightBleedingAmount;
    uint32_t shadowAtlasSize;

    uint32_t shadowPadding[1u]; // graphics::TextureHandle is uvec2 (uint64_t)

    // camera
    glm::uvec4 clusterSize;
    TransformDescription viewTransform;
    ClipSpaceDescription clipSpace;
    RangeDescription cullPlaneLimits;

    // uint32_t camerPaadding[0u];

    static RenderInfoDescription make(
        uint32_t time,
        float dielectricSpecular,
        const utils::OrientedBoundingBox& globalBoundingBox,
        uint32_t drawDataCount,
        uint32_t skeletalAnimatedDataCount,
        uint32_t shadowsCount,
        uint32_t lightsCount,
        graphics::TextureHandle shadowVarianceBluredTextureHandle,
        graphics::TextureHandle shadowColorBluredTextureHandle,
        const std::vector<float>& shadowBlurKernel,
        float shadowLightBleedingAmount,
        uint32_t shadowAtlasSize,
        const glm::uvec3& clusterSize,
        const utils::Transform& viewTransform,
        const utils::ClipSpace& clipSpace,
        const utils::Range& cullPlaneLimits);
};

struct CountersDescription
{
    glm::uvec2 ZRange; // uvec2 for atomic operations
    uint32_t firstGlobalLightNodeID;
    uint32_t clusterLocalLightsCount;
    uint32_t lightNodesCount;
    uint32_t skeletalAnimatedDataToUpdateCount;
    uint32_t shadowsToUpdateCount;
    uint32_t opaqueDrawDataRenderCommandsCount;
    uint32_t transparentDrawDataRenderCommandsCount;
    uint32_t shadowDataCount;
    uint32_t opaqueShadowDataRenderCommandsCount;
    uint32_t transparentShadowDataRenderCommandsCount;

    // padding
    // uint32_t padding[0u];
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

    // padding
    // uint32_t padding[0u];

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

using VerticesDataDescription = float;
using ElementsDataDescription = uint32_t;

struct MeshDescription
{
    BoundingBoxDescription boundingBox;
    uint32_t elementsDataSize;
    uint32_t verticesDataOffset;
    uint32_t elementsDataOffset; // draw arrays is used if 0xFFFFFFFFu
    uint32_t flags;
    //  0.. 1 - position components count [0..3]
    //  2.. 3 - normal components count [0..3]
    //  4.. 5 - texture coords component count [0..3]
    //  6.. 8 - bones count [0..7]
    //  9.. 9 - tangent space flag [0 - no tangents, 1 - tangents + binormals flags]
    // 10..12 - color components count [0 - no colors, 1 - grayscale, 2 - grayscale,alpha, 3 - RGB, 4 - RGBA, 5..7 - not used]
    // 13..31 - free (19 bits)

    // uint32_t padding[0u];

    static MeshDescription makeEmpty();
    static MeshDescription make(
        const utils::BoundingBox&,
        uint32_t elementsDataSize,
        uint32_t verticesDataOffset,
        uint32_t elementsDataOffset,
        uint32_t numPositionComponents,
        uint32_t numNormalComponents,
        uint32_t numTexCoordsComponents,
        uint32_t numBones,
        bool hasTangent,
        uint32_t numColorComponents);
};

struct MapDescription
{
    graphics::TextureHandle textureHandle;

    static MapDescription makeEmpty();
    static MapDescription make(graphics::TextureHandle);
};

struct MaterialDescription
{
    glm::vec4 baseColor;
    glm::vec4 emission;
    uint32_t baseColorMapID;
    uint32_t opacityMapID;
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
    uint32_t padding[3u];

    static MaterialDescription makeEmpty();
    static MaterialDescription make(
        const glm::vec4& baseColor,
        const glm::vec3& emission,
        uint32_t baseColorMapID,
        uint32_t opacityMapID,
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

    static DrawableDescription makeEmpty();
    static DrawableDescription make(uint32_t meshID, uint32_t materialID);
};

struct DrawDataDescription
{
    TransformDescription transform;
    uint32_t drawableID;
    uint32_t skeletalAnimatedDataID;
    uint32_t padding[2u];

    static DrawDataDescription makeEmpty();
    static DrawDataDescription make(const utils::Transform& transform, uint32_t drawableID, uint32_t skeletalAnimatedDataID);
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

struct ShadowTransformsDataDescription
{
    TransformDescription viewTransform;
    RangeDescription ZRange;
    glm::mat4x4 projectionMatrix;
    glm::uvec4 mapCoords;
    glm::vec4 frustumPoints[8u];
    glm::vec4 frustumFaceNormalLinesAndRanges0[5u];
    glm::vec4 frustumFaceNormalLinesAndRanges1[5u];
    PlaneDescription frustumPlanes[6u];
    glm::vec4 frustumEdgeDirections[6u];

    // uint32_t padding[0u];

    static ShadowTransformsDataDescription make(
        const utils::Transform&,
        const utils::Range&,
        const glm::mat4x4&,
        const glm::uvec3&);
    static ShadowTransformsDataDescription make(const glm::uvec3&);
};

struct ShadowDescription
{
    RangeDescription cullPlaneLimits;
    uint32_t mapSize;
    uint32_t transformsDataOffset;
    uint32_t flags;
    //  0.. 2 - layers count
    //  3..31 - free (29 bits)

    uint32_t padding[1u];

    static ShadowDescription makeEmpty();
    static ShadowDescription make(
        uint32_t mapSize,
        const utils::Range& cullPlaneLimits,
        uint32_t layersCount,
        uint32_t transformsDataOffset);

    static uint32_t layersCount(const ShadowDescription&);
};

struct LightDescription
{
    TransformDescription transform;
    glm::vec4 params0;
    glm::vec4 params1;
    uint32_t shadowID;
    uint32_t flags;
    //  0.. 0 - is enabled
    //  1..31 - free (31 bits)

    uint32_t padding[2u];

    static LightDescription makeEmpty();
    static LightDescription makePoint(
        const utils::Transform& transform,
        bool isEnabled,
        const glm::vec3& color,
        const glm::vec2& radiuses,
        uint32_t shadowID);
    static LightDescription makeSpot(
        const utils::Transform& transform,
        bool isEnabled,
        const glm::vec3& color,
        const glm::vec2& radiuses,
        const glm::vec2& halfAngles,
        uint32_t shadowID);
    static LightDescription makeDirectional(
        const utils::Transform& transform,
        bool isEnabled,
        const glm::vec3& color,
        uint32_t shadowID);
    static LightDescription makeImageBased(
        const utils::Transform& transform,
        bool isEnabled,
        uint32_t BRDFLutMapID,
        uint32_t diffuseMapID,
        uint32_t specularMapID,
        float contribution);
    static LightDescription makeAmbient(bool isEnabled, const glm::vec3& color);
};

struct ShadowToUpdateDescription
{
    uint32_t shadowID;

    uint32_t padding[3u];
};

struct ShadowDataDescription
{
    uint32_t drawDataID;
    uint32_t shadowID;
    uint32_t layerIDs[6u];

    // uint32_t padding[0u];
};

using SkeletonsDataDescription = float;

struct SkeletonDescription
{
    uint32_t dataOffset;

    uint32_t padding[3u];

    static SkeletonDescription makeEmpty();
    static SkeletonDescription make(uint32_t dataOffset);
};

struct ShadowMapsDescription
{
    graphics::TextureHandle shadowDepthTextureHandle;
    graphics::TextureHandle shadowVarianceTextureHandle;
    graphics::TextureHandle shadowColorTextureHandle;

    uint32_t padding[2u]; // graphics::TextureHandle is uvec2 (uint64_t)

    static ShadowMapsDescription makeEmpty();
    static ShadowMapsDescription make(
        graphics::TextureHandle shadowDepthTextureHandle,
        graphics::TextureHandle shadowVarianceTextureHandle,
        graphics::TextureHandle shadowColorTextureHandle);
};

using BonesTransformsDataDescription = TransformDescription;

struct SkeletalAnimatedDataDescription
{
    uint32_t skeletonID;
    uint32_t currentAnimationID;
    uint32_t bonesTransformsDataOffset;
    uint32_t lastUpdateTime;

    // uint32_t padding[0u];

    static SkeletalAnimatedDataDescription makeEmpty();
    static SkeletalAnimatedDataDescription make(
        uint32_t skeletonID,
        uint32_t currentAnimationID,
        uint32_t bonesTransformsDataOffset,
        uint32_t lastUpdateTime);
};

struct SkeletalAnimatedDataToUpdateDescription
{
    uint32_t skeletalAnimatedDataID;

    uint32_t padding[3u];
};

} // namespace core
} // namespace simplex

#endif // CORE_DESCRIPTIONS_H
