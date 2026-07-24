#include<math/utils.glsl>

out vec2 v_fragCoords;

void main(void)
{
	v_fragCoords = screenQuadVertexZO(gl_VertexID);
	gl_Position = vec4(screenQuadVertex(gl_VertexID), 0.0f, 1.0f);
}
