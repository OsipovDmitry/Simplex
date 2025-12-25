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

Sphere transformSphere(in Transform t, in Sphere s)
{
	return makeSphere(
		transformPoint(t, s.xyz),
		transformDistance(t, s.w));
}