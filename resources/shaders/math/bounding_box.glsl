#include<line.glsl>
#include<line_segment.glsl>
#include<transform.glsl>

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

vec2 boundingBoxProjectOnLine(in BoundingBox bb, in Line l)
{
	vec3 vmin = bb.points[0u];
	vec3 vmax = bb.points[1u];
	for (uint i = 0u; i < 3u; ++i)
	{
		if (l.direction[i] < 0.0f)
		{
			float tmp = vmin[i];
			vmin[i] = vmax[i];
			vmax[i] = tmp;
		}
	}
    return vec2(lineProjectOn(l, vmin), lineProjectOn(l, vmax));
}

BoundingBox boundingBoxAdd(in BoundingBox bb, in vec3 v)
{
	return makeBoundingBox(min(bb.points[0u], v), max(bb.points[1u], v));
}

vec3 boundingBoxClosestPoint(in BoundingBox bb, in vec3 v)
{
	return clamp(v, bb.points[0u], bb.points[1u]);
}