#include<constants.glsl>
#include<transform.glsl>

#define Sphere vec4

Sphere makeSphere(in vec3 center, in float radius)
{
	return Sphere(center, radius);
}

vec3 sphereCenter(in Sphere s)
{
	return s.xyz;
}

float sphereRadius(in Sphere s)
{
	return s.w;
}

bool sphereIsPointInside(in Sphere s, in vec3 v)
{
	return distance(s.xyz, v) <= s.w;
}

bool sphereIsFrustumInside(in Sphere s, in vec3 fPoints[FRUSTUM_POINTS_COUNT])
{
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		if (!sphereIsPointInside(s, fPoints[i]))
			return false;
	
	return true;
}

Sphere transformSphere(in Transform t, in Sphere s)
{
	return makeSphere(
		transformPoint(t, s.xyz),
		transformSize(t, s.w));
}