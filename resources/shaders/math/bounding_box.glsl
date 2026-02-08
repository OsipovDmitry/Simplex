#include<constants.glsl>
#include<line.glsl>
#include<line_segment.glsl>
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
	const uint index = clamp(ID, 0u, BOUNDING_BOX_POINTS_COUNT - 1u);
	vec3 result;
	for (uint i = 0u; i < 3u; ++i)
		result[i] = bb.points[(index & (1u << i)) >> i][i];
	return result;
}

LineSegment boundingBoxEdge(in BoundingBox bb, in uint ID)
{
	const uvec2 edgeIndices = boundingBoxEdgeIndices(ID);
	return makeLineSegment(
		boundingBoxPoint(bb, edgeIndices[0u]),
		boundingBoxPoint(bb, edgeIndices[1u]));
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

BoundingBox transformBoundingBox(in Transform t, in BoundingBox bb)
{
	mat3 axes = mat3(1.0f);
	for (uint i = 0u; i < 3u; ++i)
	{
		vec3 axis = vec3(0.0f);
		axis[i] = 1.0f;
		
		axes[i] = abs(transformVector(t, axis));
	}

    const vec3 halfSize = axes * (boundingBoxHalfSize(bb) * transformDistance(t, 1.0f));
    const vec3 center = transformPoint(t, boundingBoxCenter(bb));
    return makeBoundingBox(center - halfSize, center + halfSize);
}