#include<bounding_box.glsl>
#include<constants.glsl>
#include<frustum.glsl>
#include<line.glsl>
#include<oriented_bounding_box.glsl>
#include<plane.glsl>
#include<pyramid.glsl>
#include<range.glsl>
#include<sphere.glsl>

bool rangeVsRange(in Range r0, in Range r1)
{
	return !isRangeEmpty(rangeIntersect(r0, r1));
}

bool orientedBoundingBoxVsFrustumSuperFast(
	in OrientedBoundingBox obb,
	in Line fFaceNormalLines[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in Range fRanges[FRUSTUM_FACE_NORMAL_LINES_COUNT])
{
	if (isOrientedBoundingBoxEmpty(obb))
		return false;
	
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Range obbRange = orientedBoundingBoxProjectOnLine(obb, fFaceNormalLines[i]);
		
		if (!rangeVsRange(fRanges[i], obbRange))
			return false;
	}
	
	return true;
}

bool orientedBoundingBoxVsFrustumFast(
	in OrientedBoundingBox obb,
	in Line fFaceNormalLines[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in Range fRanges[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in vec3 fPoints[FRUSTUM_POINTS_COUNT])
{
	if (!orientedBoundingBoxVsFrustumSuperFast(obb, fFaceNormalLines, fRanges))
		return false;
	
	Line obbFaceNormalLines[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT];
	Range obbRanges[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT];
	orientedBoundingBoxCalculateFaceNormalLinesAndRanges(obb, obbFaceNormalLines, obbRanges);
	
	for (uint i = 0u; i < BOUNDING_BOX_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Range fRange = frustumProjectOnLine(fPoints, obbFaceNormalLines[i]);
		
		if (!rangeVsRange(fRange, obbRanges[i]))
			return false;
	}
	
	return true;
}

bool orientedBoundingBoxVsFrustum(
	in OrientedBoundingBox obb,
	in Line fFaceNormalLines[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in Range fRanges[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in vec3 fPoints[FRUSTUM_POINTS_COUNT],
	in vec3 fEdgeDirections[FRUSTUM_EDGE_DIRECTIONS_COUNT])
{
	if (!orientedBoundingBoxVsFrustumFast(obb, fFaceNormalLines, fRanges, fPoints))
		return false;
	
	const vec3 obbEdgeDirections[BOUNDING_BOX_EDGE_DIRECTIONS_COUNT] = orientedBoundingBoxCalculateEdgeDirections(obb);
	
	for (uint i = 0u; i < FRUSTUM_EDGE_DIRECTIONS_COUNT; ++i)
	{
		for (uint j = 0u; j < BOUNDING_BOX_EDGE_DIRECTIONS_COUNT; ++j)
		{
			const vec3 dir = cross(fEdgeDirections[i], obbEdgeDirections[j]);
			if (length(dir) < EPS)
				continue;
			
			const Line l = makeLine(vec3(0.0f), dir);
			
			const Range fRange = frustumProjectOnLine(fPoints, l);
			const Range obbRange = orientedBoundingBoxProjectOnLine(obb, l);
			
			if (!rangeVsRange(fRange, obbRange))
				return false;
		}
	}
	
	return true;
}

bool sphereVsFrustum(in Sphere s, in mat4x4 projectionMatrixInverted)
{
	const vec3 c = sphereCenter(s);
	return distance(frustumClosestPoint(projectionMatrixInverted, c), c) <= sphereRadius(s);
}

bool sphereVsBoundingBox(in Sphere s, in BoundingBox bb)
{	
	const vec3 c = sphereCenter(s);
	return distance(boundingBoxClosestPoint(bb, c), c) <= sphereRadius(s);
}

bool pyramidVsFrustumSuperFast(
	in Pyramid p,
	in Line fFaceNormalLines[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in Range fRanges[FRUSTUM_FACE_NORMAL_LINES_COUNT])
{		
	for (uint i = 0u; i < FRUSTUM_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Range pRange = pyramidProjectOnLine(p, fFaceNormalLines[i]);
		
		if (!rangeVsRange(fRanges[i], pRange))
			return false;
	}
	
	return true;
}

bool pyramidVsFrustumFast(
	in Pyramid p,
	in Line fFaceNormalLines[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in Range fRanges[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in vec3 fPoints[FRUSTUM_POINTS_COUNT])
{
	if (!pyramidVsFrustumSuperFast(p, fFaceNormalLines, fRanges))
		return false;
	
	Line pFaceNormalLines[PYRAMID_FACE_NORMAL_LINES_COUNT];
	Range pRanges[PYRAMID_FACE_NORMAL_LINES_COUNT];
	pyramidCalculateFaceNormalLinesAndRanges(p, pFaceNormalLines, pRanges);
	
	for (uint i = 0u; i < PYRAMID_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Range fRange = frustumProjectOnLine(fPoints, pFaceNormalLines[i]);
		
		if (!rangeVsRange(fRange, pRanges[i]))
			return false;
	}
	
	return true;
}

bool pyramidVsFrustum(
	in Pyramid p,
	in Line fFaceNormalLines[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in Range fRanges[FRUSTUM_FACE_NORMAL_LINES_COUNT],
	in vec3 fPoints[FRUSTUM_POINTS_COUNT],
	in vec3 fEdgeDirections[FRUSTUM_EDGE_DIRECTIONS_COUNT])
{
	if (!pyramidVsFrustumFast(p, fFaceNormalLines, fRanges, fPoints))
		return false;
	
	const vec3 pEdgeDirections[PYRAMID_EDGE_DIRECTIONS_COUNT] = pyramidCalculateEdgeDirections(p);
	
	for (uint i = 0u; i < FRUSTUM_EDGE_DIRECTIONS_COUNT; ++i)
	{
		for (uint j = 0u; j < PYRAMID_EDGE_DIRECTIONS_COUNT; ++j)
		{
			const vec3 dir = cross(fEdgeDirections[i], pEdgeDirections[j]);
			if (length(dir) < EPS)
				continue;
			
			const Line l = makeLine(vec3(0.0f), dir);
			
			const Range fRange = frustumProjectOnLine(fPoints, l);
			const Range pRange = pyramidProjectOnLine(p, l);
			
			if (!rangeVsRange(fRange, pRange))
				return false;
		}
	}
	
	return true;
}

bool pyramidVsBoundingBoxSuperFast(in Pyramid p, in BoundingBox bb)
{	
	Line bbFaceNormalLines[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT];
	Range bbRanges[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT];
	boundingBoxCalculateFaceNormalLinesAndRanges(bb, bbFaceNormalLines, bbRanges);
	
	for (uint i = 0u; i < BOUNDING_BOX_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Range pRange = pyramidProjectOnLine(p, bbFaceNormalLines[i]);
		
		if (!rangeVsRange(bbRanges[i], pRange))
			return false;
	}
	
	return true;
}

bool pyramidVsBoundingBoxFast(in Pyramid p, in BoundingBox bb)
{	
	if (!pyramidVsBoundingBoxSuperFast(p, bb))
		return false;
	
	Line pFaceNormalLines[PYRAMID_FACE_NORMAL_LINES_COUNT];
	Range pRanges[PYRAMID_FACE_NORMAL_LINES_COUNT];
	pyramidCalculateFaceNormalLinesAndRanges(p, pFaceNormalLines, pRanges);
	
	for (uint i = 0u; i < PYRAMID_FACE_NORMAL_LINES_COUNT; ++i)
	{
		const Range bbRange = boundingBoxProjectOnLine(bb, pFaceNormalLines[i]);
		
		if (!rangeVsRange(pRanges[i], bbRange))
			return false;
	}
	
	return true;
}

bool pyramidVsBoundingBox(in Pyramid p, in BoundingBox bb)
{	
	if (!pyramidVsBoundingBoxFast(p, bb))
		return false;
	
	const vec3 pEdgeDirections[PYRAMID_EDGE_DIRECTIONS_COUNT] = pyramidCalculateEdgeDirections(p);
	const vec3 bbEdgeDirections[BOUNDING_BOX_EDGE_DIRECTIONS_COUNT] = boundingBoxCalculateEdgeDirections();
	
	for (uint i = 0u; i < PYRAMID_EDGE_DIRECTIONS_COUNT; ++i)
	{
		for (uint j = 0u; j < BOUNDING_BOX_EDGE_DIRECTIONS_COUNT; ++j)
		{
			const vec3 dir = cross(pEdgeDirections[i], bbEdgeDirections[j]);
			if (length(dir) < EPS)
				continue;
			
			const Line l = makeLine(vec3(0.0f), dir);
			
			const Range pRange = pyramidProjectOnLine(p, l);
			const Range bbRange = boundingBoxProjectOnLine(bb, l);
			
			if (!rangeVsRange(pRange, bbRange))
				return false;
		}
	}
	
	return true;
}
