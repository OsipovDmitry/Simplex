#include<math/bounding_box.glsl>
#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };

BoundingBox meshBoundingBox(in uint meshID)
{
	return toBoundingBox(meshes[meshID].boundingBox);
}

uint meshPositionNormalTexCoordsDataOffset(in uint meshID)
{
	return meshes[meshID].positionNormalTexCoordsDataOffset;
}

uint meshTangentDataOffset(in uint meshID)
{
	return meshes[meshID].tangentDataOffset;
}

uint meshBoneDataOffset(in uint meshID)
{
	return meshes[meshID].boneDataOffset;
}

uint meshElementDataOffset(in uint meshID)
{
	return meshes[meshID].elementDataOffset;
}

uint meshElementDataSize(in uint meshID)
{
	return meshes[meshID].elementDataSize;
}

bool meshHasPositions(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 0, 1) != 0u;
}

bool meshHasNormals(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 1, 1) != 0u;
}

bool meshHasTexCoords(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 2, 1) != 0u;
}

uint meshBonesCount(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 3, 3);
}