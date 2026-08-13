#include<math/bounding_box.glsl>
#include<math/clip_space.glsl>
#include<math/constants.glsl>
#include<math/oriented_bounding_box.glsl>
#include<math/plane.glsl>
#include<math/range.glsl>
#include<math/transform.glsl>
#include<math/quat.glsl>

#define TextureHandle uvec2
#define ImageHandle uvec2

struct QuatDescription
{
	vec4 q;
	//uint padding[0u];
};

QuatDescription makeQuatDescription(in Quat q)
{
	return QuatDescription(q);
}

Quat toQuat(in QuatDescription desc)
{
	return makeQuat(desc.q);
}

struct RangeDescription
{
	vec2 r;
	uint padding[2u];
};

RangeDescription makeRangeDescription(in Range r)
{
	return RangeDescription(r, uint[2u](0u, 0u));
}

Range toRange(in RangeDescription desc)
{
	return makeRange(desc.r);
}

struct PlaneDescription
{
	vec4 p;
	//uint padding[0u];
};

PlaneDescription makePlaneDescription(in Plane p)
{
	return PlaneDescription(p);
}

Plane toPlane(in PlaneDescription desc)
{
	return makePlane(desc.p);
}

struct BoundingBoxDescription
{
    vec4 minPoint;
    vec4 maxPoint;
    // uint32_t padding[0u];
};

BoundingBoxDescription makeBoundingBoxDescription(in BoundingBox bb)
{
	return BoundingBoxDescription(vec4(boundingBoxMinPoint(bb), 0.0f), vec4(boundingBoxMaxPoint(bb), 0.0f));
}

BoundingBox toBoundingBox(in BoundingBoxDescription desc)
{
	return makeBoundingBox(desc.minPoint.xyz, desc.maxPoint.xyz);
}

struct OrientedBoundingBoxDescription
{
    QuatDescription rotation;
    vec4 translation;
    vec4 halfSizes;
    // uint32_t padding[0u];
};

OrientedBoundingBox toOrientedBoundingBox(in OrientedBoundingBoxDescription desc)
{
	return makeOrientedBoundingBox(toQuat(desc.rotation), desc.translation.xyz, desc.halfSizes.xyz);
}

struct TransformDescription
{
    QuatDescription rotation;
    vec4 translationAndScale;
};

TransformDescription makeTransformDescription(in Transform t)
{
	return TransformDescription(
		makeQuatDescription(t.rotation),
		vec4(t.translation, t.scale));
}

Transform toTransform(in TransformDescription desc)
{
	return makeTransform(desc.translationAndScale.w, toQuat(desc.rotation), desc.translationAndScale.xyz);
}

struct ClipSpaceDescription
{
    vec4 params;
    uint typeID;
    uint padding[3u];
};

ClipSpaceDescription makeClipSpaceDescription(in ClipSpace clipSpace)
{
	return ClipSpaceDescription(clipSpace.params, clipSpace.typeID, uint[3u](0u, 0u, 0u));
}

ClipSpace toClipSpace(in ClipSpaceDescription desc)
{
	return makeClipSpace(desc.typeID, desc.params);
}

struct CameraDescription
{
    uvec4 clusterSize;
    TransformDescription viewTransform;
    TransformDescription viewTransformInverted;
    ClipSpaceDescription clipSpace;
    RangeDescription ZRange;
    mat4x4 projectionMatrix;
    mat4x4 projectionMatrixInverted;
    mat4x4 viewProjectionMatrix;
    mat4x4 viewProjectionMatrixInverted;
    vec4 viewPosition;
    vec4 viewXDirection;
    vec4 viewYDirection;
    vec4 viewZDirection;
    vec4 frustumPoints[FRUSTUM_POINTS_COUNT];
	vec4 frustumFaceNormalLinesAndRanges0[FRUSTUM_FACE_NORMAL_LINES_COUNT];
	vec4 frustumFaceNormalLinesAndRanges1[FRUSTUM_FACE_NORMAL_LINES_COUNT];
    PlaneDescription frustumPlanes[FRUSTUM_PLANES_COUNT];
    vec4 frustumEdgeDirections[FRUSTUM_EDGE_DIRECTIONS_COUNT];

	// padding
    //uint padding[0u];
};

struct ClusterNodeDescription
{
	BoundingBoxDescription boundingBox;
	uint firstLightNodeID; // it's stored separately because of atomic access
	
	// padding
	uint padding[3u];
};

ClusterNodeDescription makeClusterNodeDescription(in BoundingBox bb, in uint firstLightNodeID)
{
	return ClusterNodeDescription(
		makeBoundingBoxDescription(bb),
		firstLightNodeID,
		uint[3u](0u, 0u, 0u));
}

struct ClusterLocalLightDescription
{
    uint lightID;

	// padding
    uint padding[3u];
};

ClusterLocalLightDescription makeClusterLocalLightDescription(in uint lightID)
{
	return ClusterLocalLightDescription(lightID, uint[3u](0u, 0u, 0u));
}

struct LightNodeDescription
{
	uint lightID;
	uint nextID;
	
	// padding
    uint padding[2u];
};

LightNodeDescription makeLightNodeDescription(in uint lightID, in uint nextLightNodeID)
{
	return LightNodeDescription(lightID, nextLightNodeID, uint[2u](0u, 0u));
}

struct RenderInfoDescription
{
    // global
    uvec2 viewportSize;
    uint time;
    uint dt;
	float dielectricSpecular;
	
	uint globalPadding[3u];

    // scene
    OrientedBoundingBoxDescription globalBoundingBox;
    uint drawDataCount;
    uint skeletalAnimatedDataCount;
    uint shadowsCount;
    uint lightsCount;
	
	//uint scenePadding[0u];

    // camera
    uvec4 clusterSize;
    TransformDescription viewTransform;
    ClipSpaceDescription clipSpace;
    RangeDescription cullPlaneLimits;
	
	//uint cameraPadding[0u];
};

struct CountersDescription
{
    uvec2 ZRange; // uvec2 for atomic operations
    uint firstGlobalLightNodeID;
	uint clusterLocalLightsCount;
    uint lightNodesCount;
    uint skeletalAnimatedDataToUpdateCount;
    uint shadowsToUpdateCount;
    uint opaqueDrawDataRenderCommandsCount;
    uint transparentDrawDataRenderCommandsCount;
    uint shadowDataCount;
    uint opaqueShadowDataRenderCommandsCount;
    uint transparentShadowDataRenderCommandsCount;

    //uint padding[0u];
};

struct GBufferDescription
{
    TextureHandle colorTextureHandle;
    TextureHandle depthTextureHandle;
    ImageHandle OITNodeIDImageHandle;
    uint OITNodesMaxCount;
    uint OITNodesCount;
	
	// uint padding[0u];
};

struct OITNodeDescription
{
    uvec4 PBRData;
    float depth;
    uint nextID;
	
	uint padding[2u];
};

OITNodeDescription makeOITNodeDescription(in uvec4 PBRData, in float depth, in uint nextOITNodeID)
{
	return OITNodeDescription(PBRData, depth, nextOITNodeID, uint[2u](0u, 0u));
}

struct PositionNormalTexCoordsDataDescription
{
	// Don't use vec3 here beacuse of SSBO alignement, but keep using glm::vec3 on CPU
    float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct BoneDataDescription
{
    uint ID;
    float weight;
};

#define TangentDataDescription vec4
#define ElementDataDescription uint

struct MeshDescription
{
    BoundingBoxDescription boundingBox;
    uint positionNormalTexCoordsDataOffset;
    uint tangentDataOffset;
    uint boneDataOffset;
    uint elementDataOffset;
    uint elementDataSize;
    uint flags;
    //  0.. 0 - has position
    //  1.. 1 - has normals
    //  2.. 2 - has tex coords
    //  3.. 5 - bones count [0..7]
    //  6..31 - free (26 bits)

    uint padding[2u];
};

struct MaterialDescription
{
    vec4 baseColor;
    vec4 emission;
    uint baseColorMapID;
    uint opacityMapID;
    uint emissionMapID;
    uint occlusionMapID;
    uint roughnessMapID;
    uint metalnessMapID;
    uint normalMapID;
    uint flags0; // 0.. 7 - roughness, 8..15 - metalness, 16..23 - occlusionMapStrength, 24..31 - normalMapScale
    uint flags1;
    //  0.. 1 - occlusion map swizzle
    //  2.. 3 - roughness map swizzle
    //  4.. 5 - metalness map swizzle
    //  6.. 6 - is lighted
    //  7.. 7 - is shadowed
    //  8.. 8 - is transparent
    //  9.. 9 - is double sided
    // 10..17 - alpha cutoff
    // 18..31 - free (14 bits)
	
	uint padding[3u];
};

struct DrawableDescription
{
    uint meshID;
    uint materialID;
	
	uint padding[2u];
};

struct DrawDataDescription
{
	TransformDescription transform;
	uint drawableID;
    uint skeletalAnimatedDataID;
	
	uint padding[2u];
};

struct BackgroundDescription
{
	QuatDescription rotation;
    vec4 environmentColorAndBlurPower;
    uint environmentMapID;
	
    uint padding[3u];
};

struct ShadowTransformsDataDescription
{
    TransformDescription viewTransform;
    RangeDescription ZRange;
    mat4x4 projectionMatrix;
    mat4x4 viewProjectionMatrix;
    uvec4 mapCoords;
	vec4 frustumPoints[FRUSTUM_POINTS_COUNT];
	vec4 frustumFaceNormalLinesAndRanges0[FRUSTUM_FACE_NORMAL_LINES_COUNT];
	vec4 frustumFaceNormalLinesAndRanges1[FRUSTUM_FACE_NORMAL_LINES_COUNT];
    PlaneDescription frustumPlanes[FRUSTUM_PLANES_COUNT];
    vec4 frustumEdgeDirections[FRUSTUM_EDGE_DIRECTIONS_COUNT];
	
	//uint padding[0u];
};

struct ShadowDescription
{
    RangeDescription cullPlaneLimits;
	uint mapSize;
    uint transformsDataOffset;
	uint flags;
    //  0.. 2 - shadow filter
    //  3..31 - free (29 bits)

	uint padding[1u];
};

struct LightDescription
{
    TransformDescription transform;
    uvec4 params0;
    uvec4 params1;
    uvec4 params2;

    // uint32_t padding[0u];
};

struct ShadowToUpdateDescription
{
    uint shadowID;

    uint padding[3u];
};

struct ShadowDataDescription
{
    uint drawDataID;
    uint shadowID;
    uint layerIDs[6u];

    // uint padding[0u];
};

ShadowDataDescription makeShadowDataDescription(in uint drawDataID, in uint shadowID, in uint layerIDs[6u])
{
	return ShadowDataDescription(drawDataID, shadowID, layerIDs);
}

#define SkeletonsDataDescription float

struct SkeletonDescription
{
    uint dataOffset;
	
    uint padding[3u];
};

const uint ShadowMapsBlurKernelSize = 32u;
struct ShadowMapsDescription
{
    TextureHandle depthTextureHandle;
    TextureHandle momentsTextureHandle;
    TextureHandle colorTextureHandle;
	TextureHandle momentsBluredTextureHandle;
	TextureHandle colorBluredTextureHandle;
	uint padding0[2u]; // graphics::TextureHandle is uvec2 (uint64_t)

	uint atlasSize;
	float lightBleedingAmount;
    float positiveExponent;
    float negativeExponent;
    float momentsBias;
    float depthBiasFactor;
    float cascadesBlendDistanceFactor;
    float cascadesDistancePower;
    //uint padding1[0u];

	float blurKernel[ShadowMapsBlurKernelSize];
	uint blurRadius;
	uint padding2[3u];
};

struct HDRDescription
{
    TextureHandle textureHandle;
	float bloomContribution;
    uint bloomPassIndex;

    //uint padding[0u]; // graphics::TextureHandle is uvec2 (uint64_t)
};

const uint ToneMappingBinsCount = 256u;
struct ToneMappingDescription
{
    uint bins[ToneMappingBinsCount];
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
	
    uint padding[1u];
};

struct BlurInfoDescription
{
    TextureHandle sourceTextureHandle;
    ivec2 direction;
    float kernel[31u];
    uint radius;

    // uint padding[0u]; // graphics::TextureHandle is uvec2 (uint64_t)
};

BlurInfoDescription makeBlurInfoDescription(
	in TextureHandle sourceTextureHandle,
    in ivec2 direction,
    in float kernel[31u],
    in uint radius)
{
	return BlurInfoDescription(sourceTextureHandle, direction, kernel, radius);
}

#define BonesTransformsDataDescription TransformDescription

struct SkeletalAnimatedDataDescription
{
    uint skeletonID;
    uint currentAnimationID;
    uint bonesTransformsDataOffset;
    uint lastUpdateTime;

    //uint padding[0u];
};

struct SkeletalAnimatedDataToUpdateDescription
{
    uint skeletalAnimatedDataID;

    uint padding[3u];
};