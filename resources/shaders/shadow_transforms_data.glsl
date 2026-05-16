#include<descriptions.glsl>

#include<math/clip_space.glsl>
#include<math/constants.glsl>
#include<math/frustum.glsl>
#include<math/line.glsl>
#include<math/transform.glsl>
#include<math/range.glsl>
#include<math/utils.glsl>

layout (std430) buffer ssbo_shadowTransformsDataBuffer { ShadowTransformsDataDescription shadowTransformsData[]; };

Transform shadowTransformsDataViewTransform(in uint transformsDataOffset, in uint layerID)
{
	return toTransform(shadowTransformsData[transformsDataOffset + layerID].viewTransform);
}

Range shadowTransformsDataZRange(in uint transformsDataOffset, in uint layerID)
{
	return toRange(shadowTransformsData[transformsDataOffset + layerID].ZRange);
}

mat4x4 shadowTransformsDataProjectionMatrix(in uint transformsDataOffset, in uint layerID)
{
	return shadowTransformsData[transformsDataOffset + layerID].projectionMatrix;
}

uvec3 shadowTransformsDataMapCoords(in uint transformsDataOffset, in uint layerID)
{
	return uvec3(shadowTransformsData[transformsDataOffset + layerID].mapCoordsAndPackerItemID);
}

vec3[FRUSTUM_POINTS_COUNT] shadowTransformsDataFrustumPoints(in uint transformsDataOffset, in uint layerID)
{
	const uint offset = transformsDataOffset + layerID;
	vec3 result[FRUSTUM_POINTS_COUNT];
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		result[i] = vec3(shadowTransformsData[offset].frustumPoints[i]);
	return result;
}

Line[FRUSTUM_FACE_NORMAL_LINES_COUNT] shadowTransformsDataFrustumFaceNormalLines(in uint transformsDataOffset, in uint layerID)
{
	const uint offset = transformsDataOffset + layerID;
	Line result[FRUSTUM_FACE_NORMAL_LINES_COUNT];
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
		result[i] = makeLine(
			vec3(shadowTransformsData[offset].frustumFaceNormalLinesAndRanges0[i]),
			vec3(shadowTransformsData[offset].frustumFaceNormalLinesAndRanges1[i]));
	return result;
}

Range[FRUSTUM_FACE_NORMAL_LINES_COUNT] shadowTransformsDataFrustumRanges(in uint transformsDataOffset, in uint layerID)
{
	const uint offset = transformsDataOffset + layerID;
	Range result[FRUSTUM_FACE_NORMAL_LINES_COUNT];
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
		result[i] = makeRange(
			shadowTransformsData[offset].frustumFaceNormalLinesAndRanges0[i][3u],
			shadowTransformsData[offset].frustumFaceNormalLinesAndRanges1[i][3u]);
	return result;
}

Plane[FRUSTUM_PLANES_COUNT] shadowTransformsDataFrustumPlanes(in uint transformsDataOffset, in uint layerID)
{
	const uint offset = transformsDataOffset + layerID;
	Plane result[FRUSTUM_PLANES_COUNT];
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
		result[i] = toPlane(shadowTransformsData[offset].frustumPlanes[i]);
	return result;
}

vec3[FRUSTUM_EDGE_DIRECTIONS_COUNT] shadowTransformsDataFrustumEdgeDirections(in uint transformsDataOffset, in uint layerID)
{
	const uint offset = transformsDataOffset + layerID;
	vec3 result[FRUSTUM_EDGE_DIRECTIONS_COUNT];
	for (uint i = 0u; i < FRUSTUM_EDGE_DIRECTIONS_COUNT; ++i)
		result[i] = vec3(shadowTransformsData[offset].frustumEdgeDirections[i]);
	return result;
}

void shadowTransformsDataInitialize(
	in uint transformsDataOffset,
	in uint layerID,
	in Transform viewTransform,
	in ClipSpace clipSpace,
	in Range ZRange)
{
	const mat4x4 projectionMatrix = clipSpaceProjectionMatrix(clipSpace, ZRange);
	const mat4x4 projectionMatrixInverted = inverse(projectionMatrix);

	const uint offset = transformsDataOffset + layerID;
	shadowTransformsData[offset].viewTransform = makeTransformDescription(viewTransform);
	shadowTransformsData[offset].ZRange = makeRangeDescription(ZRange);
	shadowTransformsData[offset].projectionMatrix = projectionMatrix;
	
	vec3 fPoints[FRUSTUM_POINTS_COUNT];
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
	{
		fPoints[i] = frustumCalculatePoint(projectionMatrixInverted, i);
		shadowTransformsData[offset].frustumPoints[i] = vec4(fPoints[i], 1.0f);
	}
	
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Line l = frustumCalculateFaceNormalLine(projectionMatrix, i);
		const Range r = frustumProjectOnLine(fPoints, l);
		shadowTransformsData[offset].frustumFaceNormalLinesAndRanges0[i] = vec4(lineOrigin(l), rangeStart(r));
		shadowTransformsData[offset].frustumFaceNormalLinesAndRanges1[i] = vec4(lineDirection(l), rangeEnd(r));
	}

	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
	{
		shadowTransformsData[offset].frustumPlanes[i] = makePlaneDescription(frustumCalculatePlane(projectionMatrix, i));
	}	
	
	const vec3 fSideEdgeAbsDirection = frustumCalculateSideEdgeAbsDirection(projectionMatrixInverted);
	for (uint i = 0u; i < 4u; ++i)
	{
		vec3 v = fSideEdgeAbsDirection;
		v.x *= float(i % 2) * 2.0f - 1.0f;
		v.y *= float(i / 2) * 2.0f - 1.0f;
		shadowTransformsData[offset].frustumEdgeDirections[i] = vec4(v, 0.0f);
	}
	shadowTransformsData[offset].frustumEdgeDirections[4u] = vec4(orth(0u), 0.0f);
	shadowTransformsData[offset].frustumEdgeDirections[5u] = vec4(orth(1u), 0.0f);
}
