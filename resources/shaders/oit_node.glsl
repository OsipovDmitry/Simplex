#include<packing.glsl>

struct OITBufferReservedData
{
	uint maxNodesCount;
	uint nodesCount;
	uint padding[2u];
};
struct OITBufferNode 
{
    uvec4 packedPBRData;
    float depth;
    uint next;
};

OITBufferNode packOITBufferNode(
	in vec3 baseColor,
    in vec3 emission,
    in float occlusion,
    in float roughness,
    in float metalness,
    in float alpha,
    in vec3 normal,
	in bool isLighted,
	in bool isShadowed,
    in float depth,
    in uint next)
{
	return OITBufferNode(
		packPBRData(
			baseColor,
			emission,
			occlusion,
			roughness,
			metalness,
			alpha,
			normal,
			isLighted,
			isShadowed),
		depth,
		next);
}

void unpackOITBufferNode(
	in OITBufferNode node,
	out vec3 baseColor,
	out vec3 emission,
	out float occlusion,
	out float roughness,
	out float metalness,
	out float alpha,
	out vec3 normal,
	in bool isLighted,
	in bool isShadowed,
	out float depth,
	out uint next)
{ 
    unpackPBRData(
		node.packedPBRData,
		baseColor,
		emission,
		occlusion,
		roughness,
		metalness,
		alpha,
		normal,
		isLighted,
		isShadowed);
    depth = node.depth;
    next = node.next;
}

/*void packOITNodeMaterial(inout OITBufferNode node, in vec3 baseColor, in float occlusion, in float roughness, in float metalness)
{
    node.packedBaseColor = packF2x11_1x10(baseColor);
    packUnorm8(node.packedORMAlpha, occlusion, 0);
    packUnorm8(node.packedORMAlpha, roughness, 1);
    packUnorm8(node.packedORMAlpha, metalness, 2);
}

void unpackOITNodeMaterial(in OITBufferNode node, out vec3 baseColor, out float occlusion, out float roughness, out float metalness)
{
    baseColor = unpackF2x11_1x10(node.packedBaseColor);
    occlusion = unpackUnorm8(node.packedORMAlpha, 0);
    roughness = unpackUnorm8(node.packedORMAlpha, 1);
    metalness = unpackUnorm8(node.packedORMAlpha, 2);
}

void packOITNodeAlpha(inout OITBufferNode node, in float alpha)
{
    packUnorm8(node.packedORMAlpha, alpha, 3);
}

float unpackOITNodeAlpha(in OITBufferNode node)
{
    return unpackUnorm8(node.packedORMAlpha, 3);
}

void packOITNodeFinalColor(inout OITBufferNode node, in vec3 finalColor)
{
    node.packedFinalColor = packF2x11_1x10(finalColor);
}

vec3 unpackOITNodeFinalColor(in OITBufferNode node)
{
    return unpackF2x11_1x10(node.packedFinalColor);
}

void packOITNodeNormal(inout OITBufferNode node, in vec3 normal)
{
    packUnorm3x10(node.packedNormalFlags, packNormal(normal));
}

vec3 unpackOITNodeNormal(in OITBufferNode node)
{
    return normalize(unpackNormal(unpackUnorm3x10(node.packedNormalFlags)));
}

void packOITNodeFlags(inout OITBufferNode node, in bvec2 flags)
{
    packBool2(node.packedNormalFlags, flags);
}

bvec2 unpackOITNodeFlags(in OITBufferNode node)
{
    return unpackBool2(node.packedNormalFlags);
}*/