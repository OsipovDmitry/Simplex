#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_vertexDataBuffer { float vertexData[]; };
layout (std430) readonly buffer ssbo_elementsBuffer { uint elements[]; };
layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };

vec3 vertexDataPosition(in uint vertexDataOffset, in uint vertexStride, in uint vertexIndex, in uint vertexRelativeOffset, in uint numComponents)
{
	vec3 result = vec3(0.0f);
	for (uint i = 0u; i < min(3u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + i];
	return result;
}

void main(void)
{
	uint meshID = gl_BaseInstance;
	MeshDescription meshDescription = meshes[meshID];

	uint elementID = elements[gl_VertexID];
		
	uint vertexStride = meshVertexStride(meshDescription);
	uint vertexRelativeOffset = 0u;
	
	vec3 position = vec3(0.0f);
	uint positionComponentsCount = meshPositionComponentsCount(meshDescription);
	if (positionComponentsCount > 0u)
	{
		position = vertexDataPosition(meshDescription.vertexDataOffset, vertexStride, elementID, vertexRelativeOffset, positionComponentsCount);
		vertexRelativeOffset += positionComponentsCount;
	}
	
    gl_Position = vec4(position, 1.0f);
}
