#include<constants.glsl>
#include<plane.glsl>
#include<transform.glsl>

struct Frustum
{
    Transform viewTransform;
    mat4x4 projectionMatrix;
    vec3 points[FRUSTUM_POINTS_COUNT];
    Plane planes[FRUSTUM_PLANES_COUNT];
};

vec3 frustumClosestPoint(in Frustum f, in vec3 v)
{
	return v;
}

Frustum transformFrustum(in Transform t, in Frustum f)
{
	Frustum result;
	result.viewTransform = transformMult(f.viewTransform, transformInverted(t));
	result.projectionMatrix = f.projectionMatrix;
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		result.points[i] = transformPoint(t, f.points[i].xyz);
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
		result.planes[i] = transformPlane(t, f.planes[i]);
	return result;
}