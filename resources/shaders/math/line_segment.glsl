#include<line.glsl>

struct LineSegment
{
	vec3 startPoint;
	vec3 endPoint;
};

LineSegment makeLineSegment(in vec3 startPoint, in vec3 endPoint)
{
	return LineSegment(startPoint, endPoint);
}

int lineSegmentClassifyByPlane(in LineSegment ls, in vec4 p)
{
	const float s1 = distanceToPlane(p, ls.startPoint);
	const float s2 = distanceToPlane(p, ls.endPoint);
	if ((s1 > 0.0f) && (s2 > 0.0f))
		return +1;
	else if ((s1 < 0.0f) && (s2 < 0.0f))
		return -1;
	else
		return 0;
}

bool lineSegmentIntersectPlane(in LineSegment ls, in vec4 p, out vec3 resPoint)
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

LineSegment transformLine(in Transform t, in LineSegment ls)
{
	return makeLineSegment(transformPoint(t, ls.startPoint), transformPoint(t, ls.endPoint));
}
