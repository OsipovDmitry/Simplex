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
    uint vertexDataOffset;
    uint numVertexData;
    uint elementsOffset; // draw arrays is used if 0xFFFFFFFFu 
    uint numElements;
    uint flags;
    //  0.. 1 - position components count [0..3]
    //  2.. 3 - normal components count [0..3]
    //  4.. 5 - texture coords component count [0..3]
    //  6.. 8 - bones count [0..7]
    //  9.. 9 - tangent space flag [0 - no tangents, 1 - tangents + binormals flags]
    // 10..12 - color components count [0 - no colors, 1 - grayscale, 2 - rrayscale,alpha, 3 - RGB, 4 - RGBA, 5..7 - not used]
    // 13..31 - free (19 bits)
};

uint meshPositionComponentsCount(in uint flags) { return bitfieldExtract(flags, 0, 2); }
uint meshNormalComponentsCount(in uint flags) { return bitfieldExtract(flags, 2, 2); }
uint meshTexCoordsComponentsCount(in uint flags) { return bitfieldExtract(flags, 4, 2); }
uint meshBonesCount(in uint flags) { return bitfieldExtract(flags, 6, 3); }
bool meshTangentFlag(in uint flags) { return bitfieldExtract(flags, 9, 1) != 0u; }
uint meshColorComponentsCount(in uint flags) { return bitfieldExtract(flags, 10, 3); }
bool isMeshTransparent(in MeshDescription description) { return meshColorComponentsCount(description.flags) >= 4u; }
uint meshVertexStride(in MeshDescription description) { return
		meshPositionComponentsCount(description.flags) +
		meshNormalComponentsCount(description.flags) +
		meshTexCoordsComponentsCount(description.flags) +
		meshBonesCount(description.flags) * 2u +
		(meshTangentFlag(description.flags) ? 4u : 0u) +
		meshColorComponentsCount(description.flags);
}

struct MaterialDescription
{
    uint baseColorTextureOffset;
    uint emissionTextureOffset;
    uint occlusionTextureOffset;
    uint roughnessTextureOffset;
    uint metalnessTextureOffset;
    uint normalTextureOffset;
    float roughness;
    float metalness;
    vec4 baseColor;
    vec4 emissionAndFlags;
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

vec3 materialEmission(in MaterialDescription desc) { return desc.emissionAndFlags.rgb; }
uint materialFlags(in MaterialDescription desc) { return floatBitsToUint(desc.emissionAndFlags.a); }
bool isMaterialTransparent(in MaterialDescription desc) { return bitfieldExtract(materialFlags(desc), 24, 1) != 0u; }

struct DrawableDescription
{
    uint meshOffset;
    uint materialOffset;
};

struct DrawDataBufferReservedData { uint count; uint padding[3u]; };
struct DrawDataDescription
{
    mat4x4 modelMatrix;
    mat3x4 normalMatrixAndDrawableOffset;
	// [0][0]..[2][2] - normal matrix
	// [0][3] - drawable offset
};

mat4x4 drawDataModelMatrix(in DrawDataDescription desc) { return mat4x4(desc.modelMatrix); }
mat3x3 drawDataNormalMatrix(in DrawDataDescription desc) { return mat3x3(desc.normalMatrixAndDrawableOffset); }
uint drawDataDrawableOffset(in DrawDataDescription desc) { return floatBitsToUint(desc.normalMatrixAndDrawableOffset[0u][3u]); }

struct BackgroundsBufferReservedData { uint count; uint padding[3u]; };
struct BackgroundDescription
{
    uint meshOffset;
    uint environmentMapOffset;
    vec4 environmentColorAndBlurPower;
};

vec3 backgroundEnvironmentColor(in BackgroundDescription desc) { return desc.environmentColorAndBlurPower.rgb; }
float backgroundBlurPower(in BackgroundDescription desc) { return desc.environmentColorAndBlurPower.a; }

struct DrawIndirectCommandsBufferReservedData { uint count; uint padding[3u]; };
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