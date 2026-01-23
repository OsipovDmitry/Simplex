#include<background.glsl>
#include<camera.glsl>
#include<mesh.glsl>
#include<vertex_data.glsl>

out vec3 v_texCoords;

void main(void)
{
	uint meshID = gl_BaseInstance;
	uint vertexDataOffset = meshVertexDataOffset(meshID);
	uint vertexStride = meshVertexStride(meshID);
	uint vertexID = (meshElementsOffset(meshID) == 0xFFFFFFFFu) ? gl_VertexID : elementsDataElementID(gl_VertexID);
	uint relativeOffset = 0u;
	
	vec3 position = vec3(0.0f);
	uint positionComponentsCount = meshPositionComponentsCount(meshID);
	if (positionComponentsCount > 0u)
	{
		position = vertexDataPosition(vertexDataOffset, vertexStride, vertexID, relativeOffset, positionComponentsCount);
		relativeOffset += positionComponentsCount;
	}
	
    v_texCoords = rotateVector(backgroundRotation(), projectPoint(cameraViewProjectionMatrixInverted(), position) - cameraViewPosition());
    gl_Position = vec4(position, 1.0f);
}
