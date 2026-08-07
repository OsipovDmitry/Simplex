#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_positionNormalTexCoordsDataBuffer { PositionNormalTexCoordsDataDescription positionNormalTexCoordsData[]; };
layout (std430) readonly buffer ssbo_tangentDataBuffer { TangentDataDescription tangentData[]; };
layout (std430) readonly buffer ssbo_boneDataBuffer { BoneDataDescription boneData[]; };

void verticesDataPositionNormalTexCoords(
	in uint positionNormalTexCoordsDataOffset,
	in uint vertexID,
	out vec3 position,
	out vec3 normal,
	out vec2 texCoords)
{
	const PositionNormalTexCoordsDataDescription value = positionNormalTexCoordsData[positionNormalTexCoordsDataOffset + vertexID];
	position = vec3(value.x, value.y, value.z);
	normal = vec3(value.nx, value.ny, value.nz);
	texCoords = vec2(value.u, value.v);
}

void verticesDataTangentAndBinormalFlag(
	in uint tangentDataOffset,
	in uint vertexID,
	out vec3 tangent,
	out float binormalFlag)
{
	const TangentDataDescription value = tangentData[tangentDataOffset + vertexID];
	tangent = value.xyz;
	binormalFlag = value.w;
}

void verticesDataBoneIDAndWeight(
	in uint boneDataOffset,
	in uint bonesCount,
	in uint vertexID,
	in uint boneIndex,
	out uint boneID,
	out float boneWeight)
{
	const BoneDataDescription value = boneData[boneDataOffset + vertexID * bonesCount + boneIndex];
	boneID = value.ID;
	boneWeight = value.weight;
}
