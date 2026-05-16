#include<background.glsl>
#include<camera.glsl>

#include<math/utils.glsl>

out vec3 v_texCoords;

void main(void)
{
	const vec3 position = vec3(screenQuadVertex(gl_VertexID), 0.0f);
	
    v_texCoords = rotateVector(backgroundRotation(), projectPoint(cameraViewProjectionMatrixInverted(), position) - cameraViewPosition());
	gl_Position = vec4(position, 1.0f);
}
