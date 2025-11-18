#include<math/frustum.glsl>

struct CameraDescription
{
    Transform viewTransform;
    Transform viewTransformInverted;
    mat4x4 projectionMatrix;
    mat4x4 projectionMatrixInverted;
    mat4x4 viewProjectionMatrix;
    mat4x4 viewProjectionMatrixInverted;
    vec4 viewPosition;
    vec4 viewXDirection;
    vec4 viewYDirection;
    vec4 viewZDirection;
    vec4 frustumPoints[FRUSTUM_POINTS_COUNT];
    vec4 frustumEdges[FRUSTUM_EDGES_COUNT][2u];
    vec4 frustumPlanes[FRUSTUM_PLANES_COUNT];
};

layout (std430) readonly buffer ssbo_cameraBuffer { CameraDescription camera; };

Transform cameraViewTransform()
{
	return camera.viewTransform;
}

Transform cameraViewTransformInverted()
{
	return camera.viewTransformInverted;
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

Frustum cameraFrustum()
{
	Frustum frustum;
	
	frustum.viewTransform = camera.viewTransform;
	frustum.projectionMatrix = camera.projectionMatrix;
	
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		frustum.points[i] = camera.frustumPoints[i].xyz;
	
	for (uint i = 0u; i < FRUSTUM_EDGES_COUNT; ++i)
	{
		frustum.edges[i][0u] = camera.frustumEdges[i][0u].xyz;
		frustum.edges[i][1u] = camera.frustumEdges[i][1u].xyz;
	}
	
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
		frustum.planes[i] = camera.frustumPlanes[i];
		
	return frustum;
}