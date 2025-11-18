#include<transform.glsl>

vec3 planeNormal(in vec4 plane)
{
	return plane.xyz;
}

float planeDist(in vec4 plane)
{
	return -plane.w;
}

vec3 planeAnyPoint(in vec4 plane, in float distanceToPlane)
{
	return planeNormal(plane) * (distanceToPlane + planeDist(plane));
}

vec3 planeAnyPoint(in vec4 plane)
{
	return planeAnyPoint(plane, 0.0f);
}

float distanceToPlane(in vec4 plane, in vec3 point)
{
	return dot(plane, vec4(point, 1.0f));
}

vec4 transformPlane(in Transform t, in vec4 plane)
{
	const vec3 anyPoint0 = transformPoint(t, planeAnyPoint(plane, 0.0f));
	const vec3 anyPoint1 = transformPoint(t, planeAnyPoint(plane, 1.0f));
	const vec3 newNormal = normalize(anyPoint1 - anyPoint0);
	return vec4(newNormal, -dot(newNormal, anyPoint0));
}