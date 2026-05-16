#include<range.glsl>

const uint OrthoClipSpaceTypeID = 0u;
const uint PerspectiveClipSpaceTypeID = 1u;
const uint SphericalClipSpaceTypeID = 2u;
const uint UndefinedClipSpaceTypeID = 0xFFFFFFFFu;

struct ClipSpace
{
	vec4 params;
	uint typeID;
};

ClipSpace makeClipSpace(in uint typeID, in vec4 params)
{
	return ClipSpace(params, typeID);
}

ClipSpace makeOrthoClipSpace(in float left, in float right, in float bottom, in float top)
{
	return makeClipSpace(OrthoClipSpaceTypeID, vec4(left, right, bottom, top));
}

ClipSpace makePerspectiveClipSpace(in float aspectRatio, in float halfFOV)
{
    const float fH = tan(halfFOV);
    const float fW = fH * aspectRatio;
	return makeClipSpace(PerspectiveClipSpaceTypeID, vec4(-fW, fW, -fH, fH));
}

mat4x4 clipSpaceProjectionMatrix(in ClipSpace cs, in Range ZRange)
{
	const float n = rangeStart(ZRange);
	const float f = rangeEnd(ZRange);

	mat4x4 result = mat4x4(1.0f);
	
	if (cs.typeID == OrthoClipSpaceTypeID)
	{
		result[0u][0u] = 2.0f / (cs.params[1u] - cs.params[0u]);
		result[1u][1u] = 2.0f / (cs.params[3u] - cs.params[2u]);
		result[2u][2u] = -2.0f / (f - n);
		result[3u][0u] = -(cs.params[1u] + cs.params[0u]) / (cs.params[1u] - cs.params[0u]);
		result[3u][1u] = -(cs.params[3u] + cs.params[2u]) / (cs.params[3u] - cs.params[2u]);
		result[3u][2u] = -(f + n) / (f - n);
	}
	else if (cs.typeID == PerspectiveClipSpaceTypeID)
	{
		const float left = cs.params[0u] * n;
		const float right = cs.params[1u] * n;
		const float bottom = cs.params[2u] * n;
		const float top = cs.params[3u] * n;
		
		result[0u][0u] = (2.0f * n) / (right - left);
		result[1u][1u] = (2.0f * n) / (top - bottom);
		result[2u][0u] = (right + left) / (right - left);
		result[2u][1u] = (top + bottom) / (top - bottom);
		result[2u][2u] = -(f + n) / (f - n);
		result[2u][3u] = -1.0f;
		result[3u][2u] = -(2.0f * f * n) / (f - n);
		result[3u][3u] = 0.0f;
	}
	else if (cs.typeID == SphericalClipSpaceTypeID)
	{
        const float farInverted = 1.0f / f;
		result[0u][0u] = farInverted;
		result[1u][1u] = farInverted;
		result[2u][2u] = -farInverted;
	}
		
	return result;
}

