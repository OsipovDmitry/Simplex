#include<shadow_to_update.glsl>

#include<math/utils.glsl>

flat out uint v_shadowID;

void main(void)
{
	const uint shadowToUpdateID = uint(gl_InstanceID);

	gl_Position = vec4(screenQuadVertexZO(gl_VertexID), 0.0f, 1.0f);
	v_shadowID = shadowToUpdateShadowID(shadowToUpdateID);
}
