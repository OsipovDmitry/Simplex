#define CLIP_SPACE_TYPE_ORTHO 0
#define CLIP_SPACE_TYPE_PERSPECTIVE 1
#define CLIP_SPACE_TYPE_SPHERICAL 2

struct ClipSpace
{
	vec4 params;
	uint type;
};

ClipSpace makeClipSpace(in uint type, in vec4 params)
{
	return ClipSpace(params, type);
}

mat4x4 clipSpaceProjectionMatrix(in ClipSpace cs, in vec2 Z)
{
	mat4x4 result = mat4x4(1.0f);
	
	if (cs.type == CLIP_SPACE_TYPE_ORTHO)
	{
		result[0u][0u] = 2.0f / (cs.params[1u] - cs.params[0u]);
		result[1u][1u] = 2.0f / (cs.params[3u] - cs.params[2u]);
		result[2u][2u] = -2.0f / (Z[1u] - Z[0u]);
		result[3u][0u] = -(cs.params[1u] + cs.params[0u]) / (cs.params[1u] - cs.params[0u]);
		result[3u][1u] = -(cs.params[3u] + cs.params[2u]) / (cs.params[3u] - cs.params[2u]);
		result[3u][2u] = -(Z[1u] + Z[0u]) / (Z[1u] - Z[0u]);
	}
	else if (cs.type == CLIP_SPACE_TYPE_PERSPECTIVE)
	{
		const float left = cs.params[0u] * Z[0u];
		const float right = cs.params[1u] * Z[0u];
		const float bottom = cs.params[2u] * Z[0u];
		const float top = cs.params[3u] * Z[0u];
		
		result[0u][0u] = (2.0f * Z[0u]) / (right - left);
		result[1u][1u] = (2.0f * Z[0u]) / (top - bottom);
		result[2u][0u] = (right + left) / (right - left);
		result[2u][1u] = (top + bottom) / (top - bottom);
		result[2u][2u] = -(Z[1u] + Z[0u]) / (Z[1u] - Z[0u]);
		result[2u][3u] = -1.0f;
		result[3u][2u] = -(2.0f * Z[1u] * Z[0u]) / (Z[1u] - Z[0u]);
		result[3u][3u] = 0.0f;
	}
	else if (cs.type == CLIP_SPACE_TYPE_SPHERICAL)
	{
        const float farInverted = 1.0f / Z[1u];
		result[0u][0u] = farInverted;
		result[1u][1u] = farInverted;
		result[2u][2u] = -farInverted;
	}
		
	return result;
}

