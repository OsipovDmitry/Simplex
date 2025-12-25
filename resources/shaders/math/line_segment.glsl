#include<line.glsl>
#include<plane.glsl>
#include<transform.glsl>

struct LineSegment
{
	vec3 startPoint;
	vec3 endPoint;
};

LineSegment makeLineSegment(in vec3 startPoint, in vec3 endPoint)
{
	return LineSegment(startPoint, endPoint);
}

bool lineSegmentIntersectPlane(in LineSegment ls, in Plane p, out vec3 resPoint)
{
	const vec3 dir = ls.endPoint - ls.startPoint;
    const float dirLen = length(dir);
    if (dirLen < EPS)
		return false;
    Line l = makeLine(ls.startPoint, dir);
    float t;
    if (!lineIntersectPlane(l, p, t))
		return false;
    if ((t < 0.0f) || (t > dirLen))
		return false;
    resPoint = linePoint(l, t);
    return true;
}

LineSegment transformLineSegment(in Transform t, in LineSegment ls)
{
	return makeLineSegment(transformPoint(t, ls.startPoint), transformPoint(t, ls.endPoint));
}
