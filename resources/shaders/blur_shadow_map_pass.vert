#include<math/utils.glsl>

flat out uint v_shadowID;

void main(void)
{
	gl_Position = vec4(screenQuadVertexZO(gl_VertexID), 0.0f, 1.0f);
	v_shadowID = uint(gl_InstanceID);
}
