struct PositionDescription
{
	float position[3u];
};

struct BonesDescription
{
    uvec4 IDs;
    vec4 weights;
};

struct MeshDescription
{
	vec4 boundingBoxMinPointAndNumVertexData;
	vec4 boundingBoxMaxPointAndNumElements;
    uint vertexDataOffset;
    uint elementsOffset; // draw arrays is used if 0xFFFFFFFFu
    uint flags;
    //  0.. 1 - position components count [0..3]
    //  2.. 3 - normal components count [0..3]
    //  4.. 5 - texture coords component count [0..3]
    //  6.. 8 - bones count [0..7]
    //  9.. 9 - tangent space flag [0 - no tangents, 1 - tangents + binormals flags]
    // 10..12 - color components count [0 - no colors, 1 - grayscale, 2 - grayscale,alpha, 3 - RGB, 4 - RGBA, 5..7 - not used]
    // 13..31 - free (19 bits)
    uint padding[1u];
};

vec3 meshBoundingBoxMinPoint(in MeshDescription desc) { return desc.boundingBoxMinPointAndNumVertexData.xyz; }
vec3 meshBoundingBoxMaxPoint(in MeshDescription desc) { return desc.boundingBoxMaxPointAndNumElements.xyz; }
uint meshNumVertexData(in MeshDescription desc) { return floatBitsToUint(desc.boundingBoxMinPointAndNumVertexData.w); }
uint meshNumElements(in MeshDescription desc) { return floatBitsToUint(desc.boundingBoxMaxPointAndNumElements.w); }
uint meshPositionComponentsCount(in MeshDescription desc) { return bitfieldExtract(desc.flags, 0, 2); }
uint meshNormalComponentsCount(in MeshDescription desc) { return bitfieldExtract(desc.flags, 2, 2); }
uint meshTexCoordsComponentsCount(in MeshDescription desc) { return bitfieldExtract(desc.flags, 4, 2); }
uint meshBonesCount(in MeshDescription desc) { return bitfieldExtract(desc.flags, 6, 3); }
bool meshTangentFlag(in MeshDescription desc) { return bitfieldExtract(desc.flags, 9, 1) != 0u; }
uint meshColorComponentsCount(in MeshDescription desc) { return bitfieldExtract(desc.flags, 10, 3); }
bool isMeshTransparent(in MeshDescription desc) { return meshColorComponentsCount(desc) >= 4u; }
uint meshVertexStride(in MeshDescription desc) { return
		meshPositionComponentsCount(desc) +
		meshNormalComponentsCount(desc) +
		meshTexCoordsComponentsCount(desc) +
		meshBonesCount(desc) * 2u +
		(meshTangentFlag(desc) ? 4u : 0u) +
		meshColorComponentsCount(desc);
}

struct MaterialDescription
{
    vec4 baseColor;
    vec4 emission;
    uint baseColorTextureID;
    uint emissionTextureID;
    uint occlusionTextureID;
    uint roughnessTextureID;
    uint metalnessTextureID;
    uint normalTextureID;
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
};

float materialRoughness(in MaterialDescription desc) { return float(bitfieldExtract(desc.flags0, 0, 8)) / 255.0f; }
float materialMetalness(in MaterialDescription desc) { return float(bitfieldExtract(desc.flags0, 8, 8)) / 255.0f; }
float materialOcclusionMapStrength(in MaterialDescription desc) { return float(bitfieldExtract(desc.flags0, 16, 8)) / 255.0f; }
float materialNormalMapScale(in MaterialDescription desc) { return float(bitfieldExtract(desc.flags0, 24, 8)) / 255.0f; }
uint materialOcclusionMapSwizzle(in MaterialDescription desc) { return bitfieldExtract(desc.flags1, 0, 2); }
uint materialRoughnessMapSwizzle(in MaterialDescription desc) { return bitfieldExtract(desc.flags1, 2, 2); }
uint materialMetalnessMapSwizzle(in MaterialDescription desc) { return bitfieldExtract(desc.flags1, 4, 2); }
bool isMaterialLighted(in MaterialDescription desc) { return bitfieldExtract(desc.flags1, 6, 1) != 0u; }
bool isMaterialShadowed(in MaterialDescription desc) { return bitfieldExtract(desc.flags1, 7, 1) != 0u; }
bool isMaterialTransparent(in MaterialDescription desc) { return bitfieldExtract(desc.flags1, 8, 1) != 0u; }
bool isMaterialDoubleSided(in MaterialDescription desc) { return bitfieldExtract(desc.flags1, 9, 1) != 0u; }
float materialAlphaCutoff(in MaterialDescription desc) { return float(bitfieldExtract(desc.flags1, 10, 8)) / 255.0f; }

struct DrawableDescription
{
    uint meshID;
    uint materialID;
};

struct DrawDataBufferReservedData { uint count; uint padding[3u]; };
struct DrawDataDescription
{
    mat4x4 modelMatrix;
    mat3x4 normalMatrixAndDrawableID;
	// [0][0]..[2][2] - normal matrix
	// [0][3] - drawable ID
};

mat4x4 drawDataModelMatrix(in DrawDataDescription desc) { return mat4x4(desc.modelMatrix); }
mat3x3 drawDataNormalMatrix(in DrawDataDescription desc) { return mat3x3(desc.normalMatrixAndDrawableID); }
uint drawDataDrawableID(in DrawDataDescription desc) { return floatBitsToUint(desc.normalMatrixAndDrawableID[0u][3u]); }

struct BackgroundDescription
{
    vec4 environmentColorAndBlurPower;
    uint environmentMapID;
};

vec3 backgroundEnvironmentColor(in BackgroundDescription desc) { return desc.environmentColorAndBlurPower.rgb; }
float backgroundBlurPower(in BackgroundDescription desc) { return desc.environmentColorAndBlurPower.a; }

struct LightBufferReservedData
{
	uint count;
	uint areaMeshID;
	uint padding[2u];
};

struct PointLightDescription
{
    mat4x4 modelMatrix;
    vec4 areaScaleAndInnerRadius;
    vec4 colorAndOuterRadius;
};

vec3 pointLightAreaScale(in PointLightDescription desc) { return desc.areaScaleAndInnerRadius.xyz; }
vec3 pointLightColor(in PointLightDescription desc) { return desc.colorAndOuterRadius.xyz; }
vec3 pointLightRadiuses(in PointLightDescription desc) { return vec2(desc.areaScaleAndInnerRadius.w, desc.colorAndOuterRadius.w); }

struct SpotLightDescription
{
    mat4x4 modelMarix;
    vec4 areaScaleAndInnerRadius;
    vec4 colorAndOuterRadius;
    vec2 cosAngles;
};

struct DirectionalLightDescription
{
    vec4 direction;
    vec4 color;
};

struct ImageBasedLightDescription
{
    uint BRDFLutTextureID;
    uint diffuseTextureID;
    uint specularTextureID;
    float contribution;
};

struct DrawIndirectCommandsBufferReservedData { uint count; uint maxSize; uint padding[2u]; };
struct DrawArraysIndirectCommand
{
    uint count;
    uint instanceCount;
    uint first;
    uint baseInstance;
};
struct DrawElementsIndirectCommand
{
    uint count;
    uint instanceCount;
    uint firstIndex;
    int  baseVertex;
    uint baseInstance;
};