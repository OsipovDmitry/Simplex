#include<descriptions.glsl>
#include<math/clip_space.glsl>
#include<math/frustum.glsl>
#include<math/line.glsl>
#include<math/plane.glsl>
#include<math/range.glsl>
#include<math/transform.glsl>
#include<math/utils.glsl>

layout (std430) buffer ssbo_cameraBuffer { CameraDescription camera; };

uvec3 cameraClusterSize()
{
	return camera.clusterSize.xyz;
}

uint cameraClusterTotalSize()
{
	return camera.clusterSize.x * camera.clusterSize.y * camera.clusterSize.z;
}

Transform cameraViewTransform()
{
	return toTransform(camera.viewTransform);
}

Transform cameraViewTransformInverted()
{
	return toTransform(camera.viewTransformInverted);
}

ClipSpace cameraClipSpace()
{
	return toClipSpace(camera.clipSpace);
}

Range cameraZRange()
{
	return toRange(camera.ZRange);
}

mat4x4 cameraProjectionMatrix()
{
	return camera.projectionMatrix;
}

mat4x4 cameraProjectionMatrixInverted()
{
	return camera.projectionMatrixInverted;
}

mat4x4 cameraViewProjectionMatrix()
{
	return camera.viewProjectionMatrix;
}

mat4x4 cameraViewProjectionMatrixInverted()
{
	return camera.viewProjectionMatrixInverted;
}

vec3 cameraViewPosition()
{
	return camera.viewPosition.xyz;
}

vec3 cameraViewXDirection()
{
	return camera.viewXDirection.xyz;
}

vec3 cameraViewYDirection()
{
	return camera.viewYDirection.xyz;
}

vec3 cameraViewZDirection()
{
	return camera.viewZDirection.xyz;
}

vec3[FRUSTUM_POINTS_COUNT] cameraFrustumPoints()
{
	vec3 result[FRUSTUM_POINTS_COUNT];
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		result[i] = vec3(camera.frustumPoints[i]);
	return result;
}

Line[FRUSTUM_FACE_NORMAL_LINES_COUNT] cameraFrustumFaceNormalLines()
{
	Line result[FRUSTUM_FACE_NORMAL_LINES_COUNT];
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
		result[i] = makeLine(
			vec3(camera.frustumFaceNormalLinesAndRanges0[i]),
			vec3(camera.frustumFaceNormalLinesAndRanges1[i]));
	return result;
}

Range[FRUSTUM_FACE_NORMAL_LINES_COUNT] cameraFrustumRanges()
{
	Range result[FRUSTUM_FACE_NORMAL_LINES_COUNT];
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
		result[i] = makeRange(
			camera.frustumFaceNormalLinesAndRanges0[i][3u],
			camera.frustumFaceNormalLinesAndRanges1[i][3u]);
	return result;
}

Plane[FRUSTUM_PLANES_COUNT] cameraFrustumPlanes()
{
	Plane result[FRUSTUM_PLANES_COUNT];
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
		result[i] = toPlane(camera.frustumPlanes[i]);
	return result;
}

vec3[FRUSTUM_EDGE_DIRECTIONS_COUNT] cameraFrustumEdgeDirections()
{
	vec3 result[FRUSTUM_EDGE_DIRECTIONS_COUNT];
	for (uint i = 0u; i < FRUSTUM_EDGE_DIRECTIONS_COUNT; ++i)
		result[i] = vec3(camera.frustumEdgeDirections[i]);
	return result;
}

void cameraUpdate(in uvec3 clusterSize, in Transform viewTransform, in ClipSpace clipSpace, in Range ZRange)
{
	const Transform viewTransformInverted = transformInverted(viewTransform);
	const mat4x4 projectionMatrix = clipSpaceProjectionMatrix(clipSpace, ZRange);
	const mat4x4 projectionMatrixInverted = inverse(projectionMatrix);

	camera.clusterSize = uvec4(clusterSize, 0u);
	camera.viewTransform = makeTransformDescription(viewTransform);
	camera.viewTransformInverted = makeTransformDescription(viewTransformInverted);
	camera.clipSpace = makeClipSpaceDescription(clipSpace);
	camera.projectionMatrix = projectionMatrix;
	camera.projectionMatrixInverted = projectionMatrixInverted;
	camera.viewProjectionMatrix = projectionMatrix * transformMat4x4(viewTransform);
	camera.viewProjectionMatrixInverted = inverse(camera.viewProjectionMatrix);
	camera.viewPosition = vec4(transformPoint(viewTransformInverted, vec3(0.0f)), 1.0f);
	camera.viewXDirection = vec4(transformVector(viewTransformInverted, vec3(1.0f, 0.0f, 0.0f)), 0.0f);
	camera.viewYDirection = vec4(transformVector(viewTransformInverted, vec3(0.0f, 1.0f, 0.0f)), 0.0f);
	camera.viewZDirection = vec4(transformVector(viewTransformInverted, vec3(0.0f, 0.0f, 1.0f)), 0.0f);
	
	vec3 fPoints[FRUSTUM_POINTS_COUNT];
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
	{
		fPoints[i] = frustumCalculatePoint(projectionMatrixInverted, i);
		camera.frustumPoints[i] = vec4(fPoints[i], 1.0f);
	}
	
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Line l = frustumCalculateFaceNormalLine(projectionMatrix, i);
		const Range r = frustumProjectOnLine(fPoints, l);
		camera.frustumFaceNormalLinesAndRanges0[i] = vec4(lineOrigin(l), rangeStart(r));
		camera.frustumFaceNormalLinesAndRanges1[i] = vec4(lineDirection(l), rangeEnd(r));
	}

	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
	{
		camera.frustumPlanes[i] = makePlaneDescription(frustumCalculatePlane(projectionMatrix, i));
	}	
	
	const vec3 fSideEdgeAbsDirection = frustumCalculateSideEdgeAbsDirection(projectionMatrixInverted);
	for (uint i = 0u; i < 4u; ++i)
	{
		vec3 v = fSideEdgeAbsDirection;
		v.x *= float(i % 2) * 2.0f - 1.0f;
		v.y *= float(i / 2) * 2.0f - 1.0f;
		camera.frustumEdgeDirections[i] = vec4(v, 0.0f);
	}
	camera.frustumEdgeDirections[4u] = vec4(orth(0u), 0.0f);
	camera.frustumEdgeDirections[5u] = vec4(orth(1u), 0.0f);
	
	camera.ZRange = makeRangeDescription(ZRange);
}

uint cameraClusterNodeID(in vec3 NDC_ZO)
{
	const vec3 texelPosVS = projectPoint(camera.projectionMatrixInverted, 2.0f * NDC_ZO - vec3(1.0f));
	
	const vec2 depthRange = cameraZRange();
	const float linearDepth = (-texelPosVS.z - depthRange[0u]) / (depthRange[1u] - depthRange[0u]);
	
	const uvec3 clusterSize = cameraClusterSize(); 
	const uvec3 ID = min(uvec3(vec3(clusterSize) * vec3(NDC_ZO.xy, linearDepth)), clusterSize - uvec3(1u));
	
	return ID.x + ID.y * clusterSize.x + ID.z * clusterSize.x * clusterSize.y;
}