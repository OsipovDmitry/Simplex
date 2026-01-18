#include<math/bounding_box.glsl>
#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };

BoundingBox meshBoundingBox(in uint meshID)
{
	return makeBoundingBox(
		meshes[meshID].boundingBoxMinPointAndNumVertexData.xyz,
		meshes[meshID].boundingBoxMaxPointAndNumElements.xyz);
}

uint meshNumVertexData(in uint meshID)
{
	return floatBitsToUint(meshes[meshID].boundingBoxMinPointAndNumVertexData.w);
}

uint meshNumElements(in uint meshID)
{
	return floatBitsToUint(meshes[meshID].boundingBoxMaxPointAndNumElements.w);
}

uint meshVertexDataOffset(in uint meshID)
{
	return meshes[meshID].vertexDataOffset;
}

uint meshElementsOffset(in uint meshID)
{
	return meshes[meshID].elementsOffset;
}

uint meshPositionComponentsCount(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 0, 2);
}

uint meshNormalComponentsCount(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 2, 2);
}

uint meshTexCoordsComponentsCount(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 4, 2);
}

uint meshBonesCount(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 6, 3);
}

bool meshTangentFlag(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 9, 1) != 0u;
}

uint meshColorComponentsCount(in uint meshID)
{
	return bitfieldExtract(meshes[meshID].flags, 10, 3);
}

bool isMeshTransparent(in uint meshID)
{
	return meshColorComponentsCount(meshID) >= 4u;
}

uint meshVertexStride(in uint meshID)
{
	return
		meshPositionComponentsCount(meshID) +
		meshNormalComponentsCount(meshID) +
		meshTexCoordsComponentsCount(meshID) +
		meshBonesCount(meshID) * 2u +
		(meshTangentFlag(meshID) ? 4u : 0u) +
		meshColorComponentsCount(meshID);
}