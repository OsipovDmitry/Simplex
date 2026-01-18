#include<line.glsl>
#include<range.glsl>
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

vec3 lineSegmentStartPoint(in LineSegment ls)
{
	return ls.startPoint;
}

vec3 lineSegmentEndPoint(in LineSegment ls)
{
	return ls.endPoint;
}

Range lineSegmentProjectOnLine(in LineSegment ls, in Line l)
{
	return makeRange(lineProjectOn(l, ls.startPoint), lineProjectOn(l, ls.endPoint));
}

LineSegment transformLineSegment(in Transform t, in LineSegment ls)
{
	return makeLineSegment(transformPoint(t, ls.startPoint), transformPoint(t, ls.endPoint));
}
