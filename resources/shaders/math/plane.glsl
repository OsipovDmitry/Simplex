#include<transform.glsl>

#define Plane vec4

Plane makePlane(in vec3 n, in float d)
{
	return Plane(normalize(n), -d);
}

Plane makePlane(in vec4 coefs)
{
	return Plane(coefs / length(coefs.xyz));
}

vec3 planeNormal(in Plane plane)
{
	return plane.xyz;
}

float planeDist(in Plane plane)
{
	return -plane.w;
}

vec3 planeAnyPoint(in Plane plane, in float distanceToPlane)
{
	return planeNormal(plane) * (distanceToPlane + planeDist(plane));
}

vec3 planeAnyPoint(in Plane plane)
{
	return planeAnyPoint(plane, 0.0f);
}

float distanceToPlane(in Plane plane, in vec3 point)
{
	return dot(plane, vec4(point, 1.0f));
}

Plane transformPlane(in Transform t, in Plane plane)
{
	const vec3 anyPoint0 = transformPoint(t, planeAnyPoint(plane, 0.0f));
	const vec3 anyPoint1 = transformPoint(t, planeAnyPoint(plane, 1.0f));
	const vec3 newNormal = normalize(anyPoint1 - anyPoint0);
	return makePlane(newNormal, dot(newNormal, anyPoint0));
}