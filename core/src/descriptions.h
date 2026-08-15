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
    glm::uvec2 viewportSize;
    uint32_t time;
    uint32_t dt;
    float dielectricSpecular;

    uint32_t globalPadding[3u];

    // scene
    OrientedBoundingBoxDescription globalBoundingBox;
    uint32_t drawDataCount;
    uint32_t skeletalAnimatedDataCount;
    uint32_t shadowsCount;
    uint32_t lightsCount;

    // uint32_t scenePadding[0u];

    // camera
    glm::uvec4 clusterSize;
    TransformDescription viewTransform;
    ClipSpaceDescription clipSpace;
    RangeDescription cullPlaneLimits;
    RangeDescription ZRange;

    // uint32_t camerPaadding[0u];

    static RenderInfoDescription make(
        const glm::uvec2& viewportSize,
        uint32_t time,
        uint32_t dt,
        float dielectricSpecular,
        const utils::OrientedBoundingBox& globalBoundingBox,
        uint32_t drawDataCount,
        uint32_t skeletalAnimatedDataCount,
        uint32_t shadowsCount,
        uint32_t lightsCount,
        const glm::uvec3& clusterSize,
        const utils::Transform& viewTransform,
        const utils::ClipSpace& clipSpace,
        const utils::Range& cullPlaneLimits,
        const utils::Range& ZRange);
};

struct CountersDescription
{
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

    uint32_t padding[2u];
};

struct GBufferDescription
{
    graphics::TextureHandle colorTextureHandle;
    graphics::TextureHandle depthTextureHandle;
    graphics::ImageHandle OITIndicesImageeHandle;
    uint32_t OITNodesMaxCount;
    uint32_t OITNodesCount;

    // padding
    // uint32_t padding[0u];

    static GBufferDescription make(
        graphics::TextureHandle colorTextureHandle,
        graphics::TextureHandle depthTextureHandle,
        graphics::ImageHandle OITIndicesImageHandle,
        uint32_t OITNodesMaxCount);
};

struct OITNodeDescription
{
    glm::u32vec4 PBRData;
    float depth;
    uint32_t nextID;
    uint32_t padding[2u];
};

struct PositionNormalTexCoordsDataDescription
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};
static_assert(sizeof(PositionNormalTexCoordsDataDescription) == 32);

struct BoneDataDescription
{
    uint32_t ID;
    float weight;
};
static_assert(sizeof(BoneDataDescription) == 8);

using TangentDataDescription = glm::vec4;
static_assert(sizeof(TangentDataDescription) == 16);

using ElementDataDescription = uint32_t;
static_assert(sizeof(ElementDataDescription) == 4);

struct MeshDescription
{
    static constexpr uint32_t MaxBonesCount = 7u;

    BoundingBoxDescription boundingBox;
    uint32_t positionNormalTexCoordsDataOffset;
    uint32_t tangentDataOffset;
    uint32_t boneDataOffset;
    uint32_t elementDataOffset;
    uint32_t elementDataSize;
    uint32_t flags;
    //  0.. 0 - has position
    //  1.. 1 - has normals
    //  2.. 2 - has tex coords
    //  3.. 5 - bones count [0..7]
    //  6..31 - free (26 bits)

    uint32_t padding[2u];

    static MeshDescription makeEmpty();
    static MeshDescription make(
        const utils::BoundingBox& bb,
        uint32_t positionNormalTexCoordsDataOffset,
        bool hasPositions,
        bool hasNormals,
        bool hasTexCoords,
        uint32_t tangentDataOffset,
        uint32_t boneDataOffset,
        uint32_t bonesCount,
        uint32_t elementDataOffset,
        uint32_t elementDataSize);
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
    glm::mat4x4 viewProjectionMatrix;
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
    glm::uvec4 params0;
    glm::uvec4 params1;
    glm::uvec4 params2;

    // uint32_t padding[0u];

    static LightDescription makeEmpty();
    static LightDescription makeAmbient(bool isEnabled, const glm::vec3& color);
    static LightDescription makeDirectional(
        const utils::Transform& transform,
        bool isEnabled,
        const glm::vec3& color,
        uint32_t shadowID,
        bool isVolumetricScatteringEnabled);
    static LightDescription makePoint(
        const utils::Transform& transform,
        bool isEnabled,
        const glm::vec3& color,
        const utils::Range& radiuses,
        uint32_t shadowID,
        bool isVolumetricScatteringEnabled);
    static LightDescription makeSpot(
        const utils::Transform& transform,
        bool isEnabled,
        const glm::vec3& color,
        const utils::Range& radiuses,
        const utils::Range& halfAngles,
        uint32_t shadowID,
        bool isVolumetricScatteringEnabled);
    static LightDescription makeImageBased(
        const utils::Transform& transform,
        bool isEnabled,
        uint32_t BRDFLutMapID,
        uint32_t diffuseMapID,
        uint32_t specularMapID,
        float contribution);

    static uint32_t makeFlags(bool isEnabled, bool isVolumetricScatteringEnabled);
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
    static const size_t BlurKernelSize = 32u; // no affects the padding

    graphics::TextureHandle depthTextureHandle;
    graphics::TextureHandle momentsTextureHandle;
    graphics::TextureHandle colorTextureHandle;
    graphics::TextureHandle momentsBluredTextureHandle;
    graphics::TextureHandle colorBluredTextureHandle;
    uint32_t padding0[2u]; // graphics::TextureHandle is uvec2 (uint64_t)

    uint32_t atlasSize;
    float lightBleedingAmount;
    float positiveExponent;
    float negativeExponent;
    float momentsBias;
    float depthBiasFactor;
    float cascadesBlendDistanceFactor;
    float cascadesDistancePower;
    // uint32_t padding1[0u];

    float blurKernel[BlurKernelSize];
    uint32_t blurRadius;
    uint32_t padding2[3u];

    static ShadowMapsDescription makeEmpty();
    static ShadowMapsDescription make(
        graphics::TextureHandle depthTextureHandle,
        graphics::TextureHandle momentsTextureHandle,
        graphics::TextureHandle colorTextureHandle,
        graphics::TextureHandle momentsBluredTextureHandle,
        graphics::TextureHandle colorBluredTextureHandle,
        uint32_t atlasSize,
        float lightBleedingAmount,
        float positiveExponent,
        float negativeExponent,
        float momentsBias,
        float depthBiasFactor,
        float cascadesBlendDistanceFactor,
        float cascadesDistancePower,
        const std::vector<float>& blurKernel);
};

struct HDRDescription
{
    graphics::TextureHandle textureHandle;
    float bloomContribution;
    uint32_t bloomPassIndex;

    // uint32_t padding[0u]; // graphics::TextureHandle is uvec2 (uint64_t)

    static HDRDescription make(graphics::TextureHandle textureHandle, float bloomContribution);
};

struct ToneMappingDescription
{
    static const size_t BinsCount = 256u; // no affects the padding
    static_assert(BinsCount > 0u);

    uint32_t bins[BinsCount];
    float minLogLuminance;
    float rangeInverseLogLuminance;
    float minClampLuminance;
    float maxClampLuminance;
    float pixelsFractionToTrimAtStart;
    float pixelsFractionToTrimAtEnd;
    float luminancePrevFrame;
    float baseLuminance;
    float tauLight;
    float tauDark;
    float exposure;

    uint32_t padding[1u];

    static ToneMappingDescription make(
        const utils::Range& luminanceRange,
        const utils::Range& luminanceClampRange,
        const std::pair<float, float>& pixelsFractionToTrim,
        float baseLuminance,
        float tauLight,
        float tauDark);
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
