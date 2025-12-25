#include<bounding_box.glsl>
#include<constants.glsl>
#include<frustum.glsl>
#include<sphere.glsl>

int boundingBoxClassifyPoint(in BoundingBox bb, in vec3 p)
{
	if (any(lessThan(p, bb.points[0u])) || any(greaterThan(p, bb.points[1u])))
		return OUTSIDE;
		
	return INSIDE;
}

//int boundingBoxClassifySphere(in BoundingBox bb, in Sphere s)
//{
//	const vec3 c = sphereCenter(s);
//	const float r = sphereRadius(s);
//	
//	const vec3 cp = boundingBoxClosestPoint(bb, c) - c;
//	if (dot(cp, cp) > r * r)
//		return OUTSIDE;
//		
//	const vec3 r3 = vec3(r);
//	if (all(greaterThan(c - bb.points[0u]), r3) && all(greaterThan(bb.points[1u] - c), r3)))
//		return INSIDE;
//	
//	return INTERSECT;
//}

int planeClassifyLineSegment(in Plane p, in LineSegment ls)
{
	const vec2 dists = vec2(distanceToPlane(p, ls.startPoint), distanceToPlane(p, ls.endPoint));
	
	if (all(greaterThan(dists, vec2(0.0f))))
		return IN_FRONT;
	else if (all(lessThan(dists, vec2(0.0f))))
		return IN_BACK;
	else
		return INTERSECT;
}

int planeClassifyBoundingBox(in Plane p, in BoundingBox bb)
{
	const vec2 dists = boundingBoxProjectOnLine(bb, makeLine(planeAnyPoint(p), planeNormal(p)));
	
	if (all(greaterThan(dists, vec2(0.0f))))
		return IN_FRONT;
	else if (all(lessThan(dists, vec2(0.0f))))
		return IN_BACK;
	else
		return INTERSECT;
}

int sphereClassifyBoundingBox(in Sphere s, in BoundingBox bb)
{	
	const vec3 c = sphereCenter(s);
	const float r = sphereRadius(s);
	
	const vec3 cp = boundingBoxClosestPoint(bb, c) - c;
	if (dot(cp, cp) > r * r)
		return OUTSIDE;
	
	for (uint i = 0u; i < BOUNDING_BOX_POINTS_COUNT; ++i)
		if (distance(c, boundingBoxPoint(bb, i)) > r)
			return INTERSECT;
	
	return INSIDE;
}

int sphereClassifyFrustum(in Sphere s, in Frustum f)
{
	const vec3 c = sphereCenter(s);
	const float r = sphereRadius(s);
	
	const vec3 cp = frustumClosestPoint(f, c) - c;
	if (dot(cp, cp) > r * r)
		return OUTSIDE;
	
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		if (distance(c, f.points[i]) > r)
			return INTERSECT;
	
	return INSIDE;
}

int frustumClassifyPoint(in Frustum f, in vec3 p)
{
	int result = 1;
	for (uint i = 0u; (result != -1) && (i < FRUSTUM_PLANES_COUNT); ++i)
		if (distanceToPlane(f.planes[i], p) < 0.0f)
			result = -1;
	return result;
}

int frustumClassifyBoundingBox(in Frustum f, in BoundingBox bb)
{
	if (isBoundingBoxEmpty(bb))
		return OUTSIDE;
		
	bool inside = true;
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
	{
		int c = planeClassifyBoundingBox(f.planes[i], bb);
		if (c == IN_BACK)
			return OUTSIDE;
		else if (c != IN_FRONT)
			inside = false;
	}
	
	if (inside)
		return INSIDE;
	
	for (uint k = 0u; k < 3u; ++k)
	{
		bool outside = true;
		for (uint i = 0u; outside && (i < FRUSTUM_POINTS_COUNT); ++i)
			if (f.points[i][k] >= bb.points[0u][k])
				outside = false;
		if (outside)
			return OUTSIDE;
			
		outside = true;
		for (uint i = 0u; outside && (i < FRUSTUM_POINTS_COUNT); ++i)
			if (f.points[i][k] <= bb.points[1u][k])
				outside = false;
		if (outside)
			return OUTSIDE;
	}
	
	return INTERSECT;
}

