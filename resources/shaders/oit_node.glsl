#include<normal.glsl>
#include<packing.glsl>

struct OITNode 
{
    uint packedBaseColor;
    uint packedFinalColor;
    uint packedORMAlpha;
    uint packedNormalFlags;
    float depth;
    uint next;
};

void packWholeOITNode(out OITNode node,
                      in vec3 baseColor,
                      in vec3 finalColor,
                      in float occlusion,
                      in float roughness,
                      in float metalness,
                      in float alpha,
                      in vec3 normal,
                      in bvec2 flags,
                      in float depth,
                      in uint next)
{   
    node.packedBaseColor = packF2x11_1x10(baseColor);
    node.packedFinalColor = packF2x11_1x10(finalColor);
    node.packedORMAlpha = packUnorm4x8(vec4(occlusion, roughness, metalness, alpha));
    node.packedNormalFlags = packUnorm3x10_Bool2(packNormal(normal), flags);
    node.depth = depth;
    node.next = next;
}

void unpackWholeOITNode(in OITNode node,
                        out vec3 baseColor,
                        out vec3 finalColor,
                        out float occlusion,
                        out float roughness,
                        out float metalness,
                        out float alpha,
                        out vec3 normal,
                        out bvec2 flags,
                        out float depth,
                        out uint next)
{ 
    vec4 unpackedORMAlpha = unpackUnorm4x8(node.packedORMAlpha);
    vec3 unpackedNormal;
    unpackUnorm3x10_Bool2(node.packedNormalFlags, unpackedNormal, flags);

    baseColor = unpackF2x11_1x10(node.packedBaseColor);
    finalColor = unpackF2x11_1x10(node.packedFinalColor);
    occlusion = unpackedORMAlpha.r;
    roughness = unpackedORMAlpha.g;
    metalness = unpackedORMAlpha.b;
    alpha = unpackedORMAlpha.a;
    normal = normalize(unpackNormal(unpackedNormal));
    depth = node.depth;
    next = node.next;
}

void packOITNodeMaterial(inout OITNode node, in vec3 baseColor, in float occlusion, in float roughness, in float metalness)
{
    node.packedBaseColor = packF2x11_1x10(baseColor);
    packUnorm8(node.packedORMAlpha, occlusion, 0);
    packUnorm8(node.packedORMAlpha, roughness, 1);
    packUnorm8(node.packedORMAlpha, metalness, 2);
}

void unpackOITNodeMaterial(in OITNode node, out vec3 baseColor, out float occlusion, out float roughness, out float metalness)
{
    baseColor = unpackF2x11_1x10(node.packedBaseColor);
    occlusion = unpackUnorm8(node.packedORMAlpha, 0);
    roughness = unpackUnorm8(node.packedORMAlpha, 1);
    metalness = unpackUnorm8(node.packedORMAlpha, 2);
}

void packOITNodeAlpha(inout OITNode node, in float alpha)
{
    packUnorm8(node.packedORMAlpha, alpha, 3);
}

float unpackOITNodeAlpha(in OITNode node)
{
    return unpackUnorm8(node.packedORMAlpha, 3);
}

void packOITNodeFinalColor(inout OITNode node, in vec3 finalColor)
{
    node.packedFinalColor = packF2x11_1x10(finalColor);
}

vec3 unpackOITNodeFinalColor(in OITNode node)
{
    return unpackF2x11_1x10(node.packedFinalColor);
}

void packOITNodeNormal(inout OITNode node, in vec3 normal)
{
    packUnorm3x10(node.packedNormalFlags, packNormal(normal));
}

vec3 unpackOITNodeNormal(in OITNode node)
{
    return normalize(unpackNormal(unpackUnorm3x10(node.packedNormalFlags)));
}

void packOITNodeFlags(inout OITNode node, in bvec2 flags)
{
    packBool2(node.packedNormalFlags, flags);
}

bvec2 unpackOITNodeFlags(in OITNode node)
{
    return unpackBool2(node.packedNormalFlags);
}