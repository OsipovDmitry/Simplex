#include<math/clip_space.glsl>
#include<math/frustum.glsl>
#include<math/transform.glsl>
#include<descriptions.glsl>

layout (std430) buffer ssbo_cameraBuffer { CameraDescription camera; };

uvec3 cameraClusterSize()
{
	return camera.clusterMaxSize.xyz;
}

uint cameraClusterTotalSize()
{
	return camera.clusterMaxSize.x * camera.clusterMaxSize.y * camera.clusterMaxSize.z;
}

Transform cameraViewTransform()
{
	return makeTransform(camera.viewTransform);
}

ClipSpace cameraClipSpace()
{
	return makeClipSpace(camera.clipSpace.type, camera.clipSpace.params);
}

vec2 cameraCullPlaneLimits()
{
	return camera.cullPlaneLimits;
}

void cameraExpandZRange(in vec2 nearFar)
{
	atomicMin(camera.ZRange[0u], floatBitsToUint(nearFar[0u]));
	atomicMax(camera.ZRange[1u], floatBitsToUint(nearFar[1u]));
}

vec2 cameraZRange()
{
	return vec2(uintBitsToFloat(camera.ZRange[0u]), uintBitsToFloat(camera.ZRange[1u]));
}

Transform cameraViewTransformInverted()
{
	return makeTransform(camera.viewTransformInverted);
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

void cameraUpdateMatrices(in vec2 Z)
{
	const ClipSpace cs = cameraClipSpace();
	camera.projectionMatrix = clipSpaceProjectionMatrix(cs, Z);
	camera.projectionMatrixInverted = inverse(camera.projectionMatrix);
	camera.viewProjectionMatrix = camera.projectionMatrix * transformMat4x4(makeTransform(camera.viewTransform));
	camera.viewProjectionMatrixInverted = inverse(camera.viewProjectionMatrix);
}

uint cameraClusterNodeID(in vec3 NDC_ZO)
{
	const vec3 texelPosVS = projectPoint(camera.projectionMatrixInverted, 2.0f * NDC_ZO - vec3(1.0f));
	
	const vec2 depthRange = cameraZRange();
	const float linearDepth = (-texelPosVS.z - depthRange[0u]) / (depthRange[1u] - depthRange[0u]);
	
	const uvec3 clusterSize = cameraClusterSize(); // TODO: replace the call cameraClusterSize() when cluster size is implemented as dynamic
	const uvec3 ID = min(uvec3(vec3(clusterSize) * vec3(NDC_ZO.xy, linearDepth)), clusterSize - uvec3(1u));
	
	return ID.x + ID.y * clusterSize.x + ID.z * clusterSize.x * clusterSize.y;
}