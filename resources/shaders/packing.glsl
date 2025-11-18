#include<math/constants.glsl>
#include<normal.glsl>

void packUnorm8(inout uint result, in float v, in int index)
{
    result = bitfieldInsert(result, uint(clamp(v, 0.0f, 1.0f) * 255.0f), index * 8, 8);
}

float unpackUnorm8(in uint v, in int index)
{
    return bitfieldExtract(v, index * 8, 8) / 255.0f;
}

uint float2packed11(in uint f)
{
    return ((((f & 0x7f800000) - 0x38000000) >> 17) & 0x07c0) | ((f >> 17) & 0x003f);
}

uint float2packed10(in uint f)
{
    return ((((f & 0x7f800000) - 0x38000000) >> 18) & 0x03E0) | ((f >> 18) & 0x001f);
}

uint floatTo11bit(in float x)
{
    if (x < EPS) return 0u;
    else if (isnan(x)) return ~0u;
    else if (isinf(x)) return 0x1Fu << 6u;

    return float2packed11(floatBitsToUint(x));
}

uint floatTo10bit(in float x)
{
    if (x < EPS) return 0u;
    else if (isnan(x)) return ~0u;
    else if (isinf(x)) return 0x1Fu << 5u;

    return float2packed10(floatBitsToUint(x));
}

uint packF2x11_1x10(in vec3 v)
{
    return ((floatTo11bit(v.x) & ((1 << 11) - 1)) <<  0) |
           ((floatTo11bit(v.y) & ((1 << 11) - 1)) << 11) |
           ((floatTo10bit(v.z) & ((1 << 10) - 1)) << 22);
}

uint packed11ToFloat(in uint p)
{
    return ((((p & 0x07c0) << 17) + 0x38000000) & 0x7f800000) | ((p & 0x003f) << 17);
}

uint packed10ToFloat(in uint p)
{
    return ((((p & 0x03E0) << 18) + 0x38000000) & 0x7f800000) | ((p & 0x001f) << 18);
}

float packed11bitToFloat(in uint x)
{
    if (x == 0) return 0.0f;
    else if (x == ((1 << 11) - 1)) return ~0;
    else if (x == (0x1f << 6)) return ~0;

    return uintBitsToFloat(packed11ToFloat(x));
}

float packed10bitToFloat(in uint x)
{
    if (x == 0) return 0.0f;
    else if (x == ((1 << 10) - 1)) return ~0;
    else if (x == (0x1f << 5)) return ~0;

    return uintBitsToFloat(packed10ToFloat(x));
}

vec3 unpackF2x11_1x10(in uint v)
{
    return vec3(packed11bitToFloat(v >> 0), packed11bitToFloat(v >> 11), packed10bitToFloat(v >> 22));
}

uint packUnorm3x10_Bool2(in vec3 v0, in bvec2 v1)
{
    uvec3 unpackV0 = uvec3(clamp(v0, 0.0f, 1.0f) * 1023.0f);
    uint unpackV1 = v1.x ? 1u : 0u;
    uint unpackV2 = v1.y ? 1u : 0u;
    
    uint result = 0u;
    result = bitfieldInsert(result, unpackV0.x, 0, 10);
    result = bitfieldInsert(result, unpackV0.y, 10, 10);
    result = bitfieldInsert(result, unpackV0.z, 20, 10);
    result = bitfieldInsert(result, unpackV1, 30, 1);
    result = bitfieldInsert(result, unpackV2, 31, 1);
    
    return result;
}

void unpackUnorm3x10_Bool2(in uint v, out vec3 v0, out bvec2 v1)
{
    v0.x = bitfieldExtract(v, 0, 10) / 1023.0f;
    v0.y = bitfieldExtract(v, 10, 10) / 1023.0f;
    v0.z = bitfieldExtract(v, 20, 10) / 1023.0f;
    v1.x = bitfieldExtract(v, 30, 1) != 0u;
    v1.y = bitfieldExtract(v, 31, 1) != 0u;
}

void packUnorm3x10(inout uint result, in vec3 v)
{
    uvec3 unpackV = uvec3(clamp(v, 0.0f, 1.0f) * 1023.0f);
    result = bitfieldInsert(result, unpackV.x, 0, 10);
    result = bitfieldInsert(result, unpackV.y, 10, 10);
    result = bitfieldInsert(result, unpackV.z, 20, 10);
}

vec3 unpackUnorm3x10(in uint v)
{
    vec3 result;
    result.x = bitfieldExtract(v, 0, 10) / 1023.0f;
    result.y = bitfieldExtract(v, 10, 10) / 1023.0f;
    result.z = bitfieldExtract(v, 20, 10) / 1023.0f;
    return result;
}

void packBool2(inout uint result, in bvec2 v)
{
    uint unpackV1 = v.x ? 1u : 0u;
    uint unpackV2 = v.y ? 1u : 0u;
    result = bitfieldInsert(result, unpackV1, 30, 1);
    result = bitfieldInsert(result, unpackV2, 31, 1);
}

bvec2 unpackBool2(in uint v)
{
    bvec2 result;
    result.x = bitfieldExtract(v, 30, 1) != 0u;
    result.y = bitfieldExtract(v, 31, 1) != 0u;
    return result;
}

uvec4 packPBRData(
	in vec3 baseColor,
	in vec3 emission,
	in float occlusion,
	in float roughness,
	in float metalness,
	in float alpha,
	in vec3 normal,
	in bool isLighted,
	in bool isShadowed)
{
	return uvec4(
		packF2x11_1x10(baseColor),
		packF2x11_1x10(emission),
		packUnorm4x8(vec4(occlusion, roughness, metalness, alpha)),
		packUnorm3x10_Bool2(packNormal(normal), bvec2(isLighted, isShadowed)));
}

void unpackPBRData(
	in uvec4 data,
	out vec3 baseColor,
	out vec3 emission,
	out float occlusion,
	out float roughness,
	out float metalness,
	out float alpha,
	out vec3 normal,
	out bool isLighted,
	out bool isShadowed)
{
	vec4 unpackedORMAlpha = unpackUnorm4x8(data[2u]);
    vec3 unpackedNormal;
	bvec2 flags;
    unpackUnorm3x10_Bool2(data[3u], unpackedNormal, flags);

    baseColor = unpackF2x11_1x10(data[0u]);
    emission = unpackF2x11_1x10(data[1u]);
    occlusion = unpackedORMAlpha[0u];
    roughness = unpackedORMAlpha[1u];
    metalness = unpackedORMAlpha[2u];
    alpha = unpackedORMAlpha[3u];
    normal = normalize(unpackNormal(unpackedNormal));
	isLighted = flags[0u];
	isShadowed = flags[1u];
}