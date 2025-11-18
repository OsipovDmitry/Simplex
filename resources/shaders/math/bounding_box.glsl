#include<line_segment.glsl>

struct BoundingBox
{
	vec3 points[2u];
};

BoundingBox makeBoundingBox(in vec3 minPoint, in vec3 maxPoint)
{
	return BoundingBox(vec3[2u](minPoint, maxPoint));
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
	uvec2 edgeIndices = boundingBoxEdgeIndices(ID);
	return makeLineSegment(
		boundingBoxPoint(bb, edgeIndices[0u]),
		boundingBoxPoint(bb, edgeIndices[1u]));
}

int boundingBoxClassifyPoint(in BoundingBox bb, in vec3 p)
{
	if (any(lessThan(p, bb.points[0u])) || any(greaterThan(p, bb.points[1u])))
		return -1;
		
	return +1;
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
    return vec2(projectOnLine(l, vmin), projectOnLine(l, vmax));
}

vec2 boundingBoxPairDistancesToPlane(in BoundingBox bb, in vec4 plane)
{
	return boundingBoxProjectOnLine(bb, makeLine(planeAnyPoint(plane), planeNormal(plane)));
}

float boundingBoxDistanceToPlane(in BoundingBox bb, in vec4 plane)
{
	vec2 dists = boundingBoxPairDistancesToPlane(bb, plane);
	return (dists[0u] * dists[1u] <= 0.0f) ?
		0.0f :
		((dists[0u] > 0.0f) ? dists[0u] : dists[1u]);
}