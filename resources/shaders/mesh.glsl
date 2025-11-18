#include<math/bounding_box.glsl>

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