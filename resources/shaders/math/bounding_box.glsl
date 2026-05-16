#include<constants.glsl>
#include<line.glsl>
#include<range.glsl>
#include<transform.glsl>
#include<utils.glsl>

struct BoundingBox
{
	vec3 points[2u];
};

BoundingBox makeBoundingBox(in vec3 minPoint, in vec3 maxPoint)
{
	return BoundingBox(vec3[2u](minPoint, maxPoint));
}

BoundingBox makeEmptyBoundingBox()
{
	return makeBoundingBox(vec3(FLT_MAX), vec3(-FLT_MAX));
}

bool isBoundingBoxEmpty(in BoundingBox bb)
{
	return any(greaterThan(bb.points[0u], bb.points[1u]));
}

vec3 boundingBoxMinPoint(in BoundingBox bb)
{
	return bb.points[0u];
}

vec3 boundingBoxMaxPoint(in BoundingBox bb)
{
	return bb.points[1u];
}

vec3 boundingBoxCenter(in BoundingBox bb)
{
	return 0.5f * (bb.points[1u] + bb.points[0u]);
}

vec3 boundingBoxHalfSize(in BoundingBox bb)
{
	return 0.5f * (bb.points[1u] - bb.points[0u]);
}

vec3 boundingBoxPoint(in BoundingBox bb, in uint ID)
{
	vec3 result;
	for (uint i = 0u; i < 3u; ++i)
		result[i] = bb.points[bitfieldExtract(ID, int(i), 1)][i];
	return result;
}

vec3[BOUNDING_BOX_POINTS_COUNT] boundingBoxPoints(in BoundingBox bb)
{
	vec3 result[BOUNDING_BOX_POINTS_COUNT];
	
	for (uint p = 0u; p < BOUNDING_BOX_POINTS_COUNT; ++p)
		for (uint i = 0u; i < 3u; ++i)
			result[p][i] = bb.points[bitfieldExtract(p, int(i), 1)][i];
		
	return result;
}

void boundingBoxCalculateFaceNormalLinesAndRanges(
	in BoundingBox bb,
	out Line faceNormalLines[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT],
	out Range ranges[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT])
{
	for (uint i = 0u; i < BOUNDING_BOX_FACE_NORMAL_LINES_COUNT; ++i)
	{
		faceNormalLines[i] = makeLine(vec3(0.0f), orth(i));
		ranges[i] = makeRange(bb.points[0u][i], bb.points[1u][i]);
	}
}

vec3[BOUNDING_BOX_EDGE_DIRECTIONS_COUNT] boundingBoxCalculateEdgeDirections()
{
	return vec3[BOUNDING_BOX_EDGE_DIRECTIONS_COUNT](orth(0u), orth(1u), orth(2u));
}

Range boundingBoxProjectOnLine(in BoundingBox bb, in Line l)
{
	vec3 vmin = bb.points[0u];
	vec3 vmax = bb.points[1u];
	for (uint i = 0u; i < 3u; ++i)
	{
		if (l.direction[i] < 0.0f)
			swap(vmin[i], vmax[i]);
	}
    return makeRange(lineProjectOn(l, vmin), lineProjectOn(l, vmax));
}

BoundingBox boundingBoxExpand(in BoundingBox bb, in vec3 v)
{
	return makeBoundingBox(min(bb.points[0u], v), max(bb.points[1u], v));
}

BoundingBox boundingBoxExpand(in BoundingBox bb0, in BoundingBox bb1)
{
	return makeBoundingBox(min(bb0.points[0u], bb1.points[0u]), max(bb0.points[1u], bb1.points[1u]));
}

vec3 boundingBoxClosestPoint(in BoundingBox bb, in vec3 v)
{
	return clamp(v, bb.points[0u], bb.points[1u]);
}

bool boundingBoxIsPointInside(in BoundingBox bb, in vec3 v)
{
	return all(greaterThan(boundingBoxMinPoint(bb), v)) && all(greaterThan(v, boundingBoxMaxPoint(bb)));
}

// Be carefull, this method extends a bounding box! More over,
// transformBoundingBox(t, transformBoundingBox(transformInverted(t), bb)) != bb
// in common case
BoundingBox transformBoundingBox(in Transform t, in BoundingBox bb)
{
	const mat3 axes = mat3(
		abs(transformVector(t, orth(0u))),
		abs(transformVector(t, orth(1u))),
		abs(transformVector(t, orth(2u))));

    const vec3 halfSize = axes * transformSize(t, boundingBoxHalfSize(bb));
    const vec3 center = transformPoint(t, boundingBoxCenter(bb));
    return makeBoundingBox(center - halfSize, center + halfSize);
}