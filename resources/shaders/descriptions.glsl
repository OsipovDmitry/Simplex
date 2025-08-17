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
    uint positionOffset;
    uint normalOffset;
    uint texCoordsOffset;
    uint bonesOffset;
    uint tangentOffset;
    uint colorOffset;
    uint indexOffset;
    uint numElements;
	uint flags;
    //  0.. 0 - is transparent
    //  1..31 - free (31 bits)
};

bool isMeshTransparent(in uint flags) { return bitfieldExtract(flags, 0, 1) != 0u; }

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
bool isMaterialTransparent(in uint flags) { return bitfieldExtract(flags, 24, 1) != 0u; }

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

vec3 backgroundEnvironmentColor(in BackgroundDescription desc) { return desc.rgb; }
float backgroundBlurPower(in BackgroundDescription desc) { return desc.a; }

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