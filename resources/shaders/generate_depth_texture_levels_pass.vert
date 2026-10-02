#include<math/utils.glsl>

void main(void)
{
	gl_Position = vec4(screenQuadVertex(gl_VertexID), 0.0f, 1.0f);
}
