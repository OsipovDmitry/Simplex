#include<math/transform.glsl>

#define QuatDescription vec4
#define TextureHandle uvec2
#define ImageHandle uvec2

Quat makeQuat(in QuatDescription desc)
{
	return desc;
}

QuatDescription makeQuatDescription(in Quat q)
{
	return q;
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

Transform makeTransform(in TransformDescription desc)
{
	return makeTransform(desc.translationAndScale.w, makeQuat(desc.rotation), desc.translationAndScale.xyz);
}

struct ClipSpaceDescription
{
    vec4 params;
    uint type;
    uint padding[3u];
};

struct CameraDescription
{
	uvec4 clusterMaxSize;
    TransformDescription viewTransform;
    ClipSpaceDescription clipSpace;
    vec2 cullPlaneLimits;
    uvec2 ZRange; // uvec2 for atomic operations
    TransformDescription viewTransformInverted;
    vec4 viewPosition;
    vec4 viewXDirection;
    vec4 viewYDirection;
    vec4 viewZDirection;
    mat4x4 projectionMatrix;
    mat4x4 projectionMatrixInverted;
    mat4x4 viewProjectionMatrix;
    mat4x4 viewProjectionMatrixInverted;
};

struct ClusterNodeDescription
{
	vec4 boundingBoxMinPoint;
	vec4 boundingBoxMaxPoint;
	uint firstLightNodeID;
	uint padding[3u];
};

struct LightNodeDescription
{
	uint lightID;
	uint nextID;
    uint padding[2u];
};

struct SceneInfoDescription
{
    uint time;
    uint drawDataCount;
    uint skeletalAnimatedDataCount;
    uint lightsCount;
    uint opaqueCommandsCount;
    uint transparentCommandsCount;
	uint lightNodesMaxCount;
	uint lightNodesCount;
    uint firstGlobalLightNodeID;
    uint padding[3u];
};

struct GBufferDescription
{
    TextureHandle colorTextureHandle;
    TextureHandle depthTextureHandle;
    ImageHandle OITNodeIDImageHandle;
    TextureHandle finalTextureHandle;
    uvec2 size;
    uint OITNodesMaxCount;
    uint OITNodesCount;
};

struct OITNodeDescription
{
    uvec4 PBRData;
    float depth;
    uint nextID;
	uint padding[2u];
};

struct MeshDescription
{
	vec4 boundingBoxMinPointAndVerticesDataSize;
	vec4 boundingBoxMaxPointAndElementsDataSize;
    uint verticesDataOffset;
    uint elementsDataOffset; // draw arrays is used if 0xFFFFFFFFu
    uint flags;
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
    
	uint padding[1u];
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

struct BackgroundDescription
{
	QuatDescription rotation;
    vec4 environmentColorAndBlurPower;
    uint environmentMapID;
    uint padding[3u];
};

struct LightDescription
{
    TransformDescription transform;
    vec4 flags0;
    vec4 flags1;
};

struct SkeletonDescription
{
    uint dataOffset;
    uint dataSize;
    uint padding[2u];
};

struct DrawDataDescription
{
	TransformDescription transform;
	uint drawableID;
	uint padding[3u];
};

struct SkeletalAnimatedDataDescription
{
    uint skeletonID;
    uint currentAnimationID;
    uint bonesTransfromsDataOffset;
    uint bonesTransfromsDataSize;

    //uint padding[0u];
};