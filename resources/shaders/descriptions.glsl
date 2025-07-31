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
};

struct MaterialDescription
{
    uint baseColorTextureOffset;
    uint emissionTextureOffset;
    uint occlusionTextureOffset;
    uint roughnessTextureOffset;
    uint metalnessTextureOffset;
    uint normalTextureOffset;
    vec4 baseColor;
    vec3 emission;
    float roughness;
    float metalness;
    uint flags;
    //  0.. 7 - occlusionMapStrength
    //  8..15 - normalMapScale
    // 16..17 - occlusion map swizzle
    // 18..19 - roughness map swizzle
    // 20..21 - metalness map swizzle
    // 22..22 - is lighted
    // 23..23 - is shadowed
    // 24..31 - free (8 bits)
};

struct DrawableDescription
{
    uint meshOffset;
    uint materialOffset;
};

struct DrawDataDescription
{
    mat4x4 modelMatrix;
    uint drawableOffset;
};

struct BackgroundDescription
{
    uint meshOffset;
    uint environmentMapOffset;
    vec3 environmentColor;
    float blurPower;
};

struct MultiDrawElementsIndirectCommand
{
    uint count;
    uint instanceCount;
    uint firstIndex;
    int  baseVertex;
    uint baseInstance;
};