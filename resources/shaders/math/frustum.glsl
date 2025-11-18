#include<constants.glsl>
#include<plane.glsl>

struct Frustum
{
    Transform viewTransform;
    mat4x4 projectionMatrix;
    vec3 points[FRUSTUM_POINTS_COUNT];
    vec3 edges[FRUSTUM_EDGES_COUNT][2u];
    vec4 planes[FRUSTUM_PLANES_COUNT];
};

int frustumClassifyPoint(in Frustum f, in vec3 p)
{
	int result = 1;
	for (uint i = 0u; (result != -1) && (i < FRUSTUM_PLANES_COUNT); ++i)
		if (distanceToPlane(f.planes[i], p) < 0.0f)
			result = -1;
	return result;
}

Frustum transformFrustum(in Transform t, in Frustum f)
{
	Frustum result;
	result.viewTransform = transformMult(f.viewTransform, transformInverted(t));
	result.projectionMatrix = f.projectionMatrix;
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		result.points[i] = transformPoint(t, f.points[i].xyz);
	for (uint i = 0u; i < FRUSTUM_EDGES_COUNT; ++i)
	{
		result.edges[i][0u] = transformPoint(t, f.edges[i][0u]);
		result.edges[i][1u] = transformPoint(t, f.edges[i][1u]);
	}
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
		result.planes[i] = transformPlane(t, f.planes[i]);
	return result;
}