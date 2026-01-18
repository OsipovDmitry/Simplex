#include<transform.glsl>

struct Line
{
	vec3 origin;
	vec3 direction;
};

Line makeLine(in vec3 origin, in vec3 direction)
{
	return Line(origin, normalize(direction));
}

vec3 lineOrigin(in Line l)
{
	return l.origin;
}

vec3 lineDirection(in Line l)
{
	return l.direction;
}

vec3 linePoint(in Line l, in float t)
{
	return l.origin + t * l.direction;
}

float lineProjectOn(in Line l, in vec3 p)
{
	return dot(p - l.origin, l.direction);
}

Line transformLine(in Transform t, in Line l)
{
	const vec3 point0 = transformPoint(t, linePoint(l, 0.0f));
	const vec3 point1 = transformPoint(t, linePoint(l, 1.0f));
	return makeLine(point0, point1 - point0);
}
