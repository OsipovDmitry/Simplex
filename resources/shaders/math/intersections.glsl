#include<bounding_box.glsl>
#include<cone.glsl>
#include<constants.glsl>
#include<frustum.glsl>
#include<line.glsl>
#include<line_segment.glsl>
#include<plane.glsl>
#include<range.glsl>
#include<utils.glsl>

struct LineIntersectPlaneResult{ uint count; float t[1u]; };
LineIntersectPlaneResult lineIntersectPlane(in Line l, in Plane p)
{
	LineIntersectPlaneResult result = LineIntersectPlaneResult(0u, float[1u](0.0f));
	
	const float denom = dot(planeNormal(p), lineDirection(l));
	if (abs(denom) < EPS)
	{
		if (abs(distanceToPlane(p, lineOrigin(l))) < EPS)
			result.t[result.count++] = 0.0f;
	}
	else
	{
		result.t[result.count++] = -distanceToPlane(p, lineOrigin(l)) / denom;
	}
	
	return result;
}

struct LineIntersectBoundingBoxResult { uint count; float t[2u]; };
LineIntersectBoundingBoxResult lineIntersectBoundingBox(in Line l, in BoundingBox bb)
{
	const vec3 lineOrig = lineOrigin(l);
	const vec3 lineDir = lineDirection(l);
	const vec3 boxMin = boundingBoxMinPoint(bb);
	const vec3 boxMax = boundingBoxMaxPoint(bb);
	float tNearFar[2u] = float[2u](-FLT_MAX, FLT_MAX);
	
	bool culled = false;
	for (uint i = 0u; i < 3u; ++i)
	{
		if (abs(lineDir[i]) < EPS)
		{
			if ((lineOrig[i] < boxMin[i]) || (lineOrig[i] > boxMax[i]))
				culled = true;
		}
		else
		{
			float t[2u] = float[2u]((boxMin[i] - lineOrig[i]) / lineDir[i], (boxMax[i] - lineOrig[i]) / lineDir[i]);
			if (t[0u] > t[1u])
				swap(t[0u], t[1u]);
			
			tNearFar[0u] = max(tNearFar[0u], t[0u]);
			tNearFar[1u] = min(tNearFar[1u], t[1u]);
			
			if (tNearFar[0u] > tNearFar[1u])
				culled = true;
		}
		
		if (culled)
			break;
	}
	
	LineIntersectBoundingBoxResult result = LineIntersectBoundingBoxResult(0u, float[2u](0.0f, 0.0f));
	if (!culled)
	{
		result.count = 2u;
		result.t = tNearFar;
	}
	
	return result;
}

struct LineIntersectConeResult { uint count; float t[2u]; };
LineIntersectConeResult lineIntersectCone(in Line l, in Cone c)
{
	const Quat rot = makeQuat(coneDirection(c), vec3(0.0f, 0.0f, 1.0f));
	const vec3 trans = -rotateVector(rot, coneOrigin(c));

	const Line newL = transformLine(makeTransform(1.0f, rot, trans), l);
	const vec3 lo = lineOrigin(newL);
	const vec3 ld = lineDirection(newL);

	const Range r = makeRange(0.0f, coneHeight(c));
	const float angle = coneHalfAngle(c);
	const float tanAngle = tan(angle);
	const float tanAngleSqr = tanAngle * tanAngle;

	const float coefA = ld.x * ld.x + ld.y * ld.y - tanAngleSqr * ld.z * ld.z;
	const float coefB = 2.0f * (lo.x * ld.x + lo.y * ld.y - tanAngleSqr * lo.z * ld.z);
	const float coefC = lo.x * lo.x + lo.y * lo.y - tanAngleSqr * lo.z * lo.z;

	uint resCount = 0u;
	float resT[2u] = float[2u](0.0f, 0.0f);

	if (abs(coefA) < EPS)
	{
		if (abs(coefB) > EPS)
		{
			float t = -coefC / coefB;
			if (rangeIsPointInside(r, lo.z + t * ld.z))
				resT[resCount++] = t;
		}
	}
	else
	{
		const float d = coefB * coefB - 4.0f * coefA * coefC;
		const float inv2A = 1.0f / (2.0f * coefA);

		if (d < 0.0f)
		{
		}
		else if (d < EPS)
		{
			if (abs(ld.z) > cos(angle))
				resT[resCount++] = -coefB * inv2A;
		}
		else
		{
			const float sqrtD = sqrt(d);

			const float t0 = (-coefB + sqrtD) * inv2A;
			if (rangeIsPointInside(r, lo.z + t0 * ld.z))
				resT[resCount++] = t0;

			const float t1 = (-coefB - sqrtD) * inv2A;
			if (rangeIsPointInside(r, lo.z + t1 * ld.z))
				resT[resCount++] = t1;
		}
	}

	if (resCount == 1u)
		resT[resCount++] = (r[1u] - lo.z) / ld.z;

	if ((resCount > 0u) && (resT[0u] > resT[1u]))
		swap(resT[0u], resT[1u]);

	return LineIntersectConeResult(resCount, resT);
}

struct LineIntersectFrustumResult { uint count; float t[2u]; };
LineIntersectFrustumResult lineIntersectFrustum(in Line l, in Frustum f)
{
	uint resCount = 0u;
	float resT[2u] = float[2u](0.0f, 0.0f);
	
	const vec3 lineOrig = lineOrigin(l);
	const vec3 lineDir = lineDirection(l);
	const Plane[FRUSTUM_PLANES_COUNT] planes = frustumPlanes(f);
	
	for (uint i = 0u; i < FRUSTUM_PLANES_COUNT; ++i)
	{
		const float denom = dot(planeNormal(planes[i]), lineDir);
		if (abs(denom) > EPS)
		{
			const float t = -distanceToPlane(planes[i], lineOrig) / denom;
			const vec3 point = linePoint(l, t);
			
			bool inside = true;
			for (uint j = 0u; j < FRUSTUM_PLANES_COUNT; ++j)
			{
				if (i == j)
					continue;
					
				inside = (distanceToPlane(planes[j], point) > 0.0f);
				
				if (!inside)
					break;
			}
			
			if (inside)
				resT[resCount++] = t;
				
			if (resCount == 2u)
				break;
		}
	}
	
	if (resCount == 1u)
		resT[resCount++] = resT[0u];
	
	if (resT[0u] > resT[1u])
		swap(resT[0u], resT[1u]);
	
	return LineIntersectFrustumResult(resCount, resT);
}

struct LineSegmentIntersectPlaneResult { uint count; vec3 p[1u]; };
LineSegmentIntersectPlaneResult lineSegmentIntersectPlane(in LineSegment ls, in Plane p)
{
	LineSegmentIntersectPlaneResult result = LineSegmentIntersectPlaneResult(0u, vec3[1u](vec3(0.0f)));
	
	const vec3 sp = lineSegmentStartPoint(ls);
	const vec3 dir = lineSegmentEndPoint(ls) - sp;
    const float dirLen = length(dir);
    if (dirLen < EPS)
	{
		if (abs(distanceToPlane(p, sp)) < EPS)
			result.p[result.count++] = sp;
	}
	else
	{	
		const Line l = makeLine(sp, dir);
		const LineIntersectPlaneResult lipr = lineIntersectPlane(l, p);
		if (lipr.count > 0u)
		{
			const float t = lipr.t[0u];
			if ((t >= 0.0f) && (t <= dirLen))
				result.p[result.count++] = linePoint(l, t);
		}
	}
	
	return result;
}

struct LineSegmentIntersectBoundingBoxResult { uint count; vec3 p[2u]; };
LineSegmentIntersectBoundingBoxResult lineSegmentIntersectBoundingBox(in LineSegment ls, in BoundingBox bb)
{
	LineSegmentIntersectBoundingBoxResult result = LineSegmentIntersectBoundingBoxResult(0u, vec3[2u](vec3(0.0f), vec3(0.0f)));
	
	const vec3 sp = lineSegmentStartPoint(ls);
	const vec3 ep = lineSegmentEndPoint(ls);
	const vec3 dir = ep - sp;
    const float dirLen = length(dir);
    if (dirLen < EPS)
	{
		if (boundingBoxIsPointInside(bb, sp))
		{
			result.p[result.count++] = sp;
			result.p[result.count++] = ep;
		}
	}
	else
	{
		const Line l = makeLine(sp, dir);
		const LineIntersectBoundingBoxResult libbr = lineIntersectBoundingBox(l, bb);
		
		if (libbr.count > 0u)
		{
			if ((libbr.t[0u] <= dirLen) && (0.0f <= libbr.t[1u]))
			{
				result.p[result.count++] = linePoint(l, max(0.0f, libbr.t[0u]));
				result.p[result.count++] = linePoint(l, min(dirLen, libbr.t[1u]));
			}
		}
	}
	
	return result;
}

struct LineSegmentIntersectConeResult { uint count; vec3 p[2u]; };
LineSegmentIntersectConeResult lineSegmentIntersectCone(in LineSegment ls, in Cone c)
{
	LineSegmentIntersectConeResult result = LineSegmentIntersectConeResult(0u, vec3[2u](vec3(0.0f), vec3(0.0f)));
	
	const vec3 sp = lineSegmentStartPoint(ls);
	const vec3 ep = lineSegmentEndPoint(ls);
	const vec3 dir = ep - sp;
    const float dirLen = length(dir);
    if (dirLen < EPS)
	{
		if (coneIsPointInside(c, sp))
		{
			result.p[result.count++] = sp;
			result.p[result.count++] = ep;
		}
	}
	else
	{
		const Line l = makeLine(sp, dir);
		const LineIntersectConeResult libbr = lineIntersectCone(l, c);
		
		if (libbr.count > 0u)
		{
			if ((libbr.t[0u] <= dirLen) && (0.0f <= libbr.t[1u]))
			{
				result.p[result.count++] = linePoint(l, max(0.0f, libbr.t[0u]));
				result.p[result.count++] = linePoint(l, min(dirLen, libbr.t[1u]));
			}
		}
	}
	
	return result;
}

struct LineSegmentIntersectFrustumResult { uint count; vec3 p[2u]; };
LineSegmentIntersectFrustumResult lineSegmentIntersectFrustum(in LineSegment ls, in Frustum f)
{
	LineSegmentIntersectFrustumResult result = LineSegmentIntersectFrustumResult(0u, vec3[2u](vec3(0.0f), vec3(0.0f)));
	
	const vec3 sp = lineSegmentStartPoint(ls);
	const vec3 ep = lineSegmentEndPoint(ls);
	const vec3 dir = ep - sp;
    const float dirLen = length(dir);
    if (dirLen < EPS)
	{
		if (frustumIsPointInside(f, sp))
		{
			result.p[result.count++] = sp;
			result.p[result.count++] = ep;
		}
	}
	else
	{
		const Line l = makeLine(sp, dir);
		const LineIntersectFrustumResult lifr = lineIntersectFrustum(l, f);
		
		if (lifr.count > 0u)
		{
			if ((lifr.t[0u] <= dirLen) && (0.0f <= lifr.t[1u]))
			{
				result.p[result.count++] = linePoint(l, max(0.0f, lifr.t[0u]));
				result.p[result.count++] = linePoint(l, min(dirLen, lifr.t[1u]));
			}
		}
	}
	
	return result;
}