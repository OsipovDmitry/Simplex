#include<bounding_box.glsl>
#include<constants.glsl>
#include<line.glsl>
#include<range.glsl>
#include<transform.glsl>
#include<quat.glsl>
#include<utils.glsl>

struct OrientedBoundingBox
{
	Quat rotation;
	vec3 translation;
	vec3 halfSize;
};

OrientedBoundingBox makeOrientedBoundingBox(in Quat rotation, in vec3 translation, in vec3 halfSize)
{
	return OrientedBoundingBox(rotation, translation, halfSize);
}

OrientedBoundingBox makeOrientedBoundingBox(in BoundingBox bb)
{
	return makeOrientedBoundingBox(makeIdentityQuat(), boundingBoxCenter(bb), boundingBoxHalfSize(bb));
}

OrientedBoundingBox makeEmptyOrientedBoundingBox()
{
	return makeOrientedBoundingBox(makeIdentityQuat(), vec3(0.0f), vec3(-FLT_MAX));
}

bool isOrientedBoundingBoxEmpty(in OrientedBoundingBox obb)
{
	return any(greaterThan(vec3(0.0f), obb.halfSize));
}

Quat orientedBoundingBoxRotation(in OrientedBoundingBox obb)
{
	return obb.rotation;
}

vec3 orientedBoundingBoxTranslation(in OrientedBoundingBox obb)
{
	return obb.translation;
}

vec3 orientedBoundingBoxHalfSize(in OrientedBoundingBox obb)
{
	return obb.halfSize;
}

vec3 orientedBoundingBoxPoint(in OrientedBoundingBox obb, in uint ID)
{
	const vec3 v = vec3(
		float(bitfieldExtract(ID, 0, 1)),
		float(bitfieldExtract(ID, 1, 1)),
		float(bitfieldExtract(ID, 2, 1)));
	
	vec3 result = 2.0f * v - vec3(1.0f);
	result = result * obb.halfSize;
	result = rotateVector(obb.rotation, result);
	result = result + obb.translation;
	
	return result;
}

vec3[BOUNDING_BOX_POINTS_COUNT] orientedBoundingBoxPoints(in OrientedBoundingBox obb)
{
	vec3 result[BOUNDING_BOX_POINTS_COUNT];
	
	for (uint p = 0u; p < BOUNDING_BOX_POINTS_COUNT; ++p)
	{
		const vec3 v = vec3(
			float(bitfieldExtract(p, 0, 1)),
			float(bitfieldExtract(p, 1, 1)),
			float(bitfieldExtract(p, 2, 1)));
		
		vec3 point = 2.0f * v - vec3(1.0f);
		point = point * obb.halfSize;
		point = rotateVector(obb.rotation, point);
		point = point + obb.translation;
		
		result[p] = point;
	}
		
	return result;
}

void orientedBoundingBoxCalculateFaceNormalLinesAndRanges(
	in OrientedBoundingBox obb,
	out Line faceNormalLines[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT],
	out Range ranges[BOUNDING_BOX_FACE_NORMAL_LINES_COUNT])
{
	for (uint i = 0u; i < BOUNDING_BOX_FACE_NORMAL_LINES_COUNT; ++i)
	{
		faceNormalLines[i] = makeLine(obb.translation, rotateVector(obb.rotation, orth(i)));
		ranges[i] = makeRange(-obb.halfSize[i], obb.halfSize[i]);
	}
}

vec3[BOUNDING_BOX_EDGE_DIRECTIONS_COUNT] orientedBoundingBoxCalculateEdgeDirections(in OrientedBoundingBox obb)
{
	vec3 result[BOUNDING_BOX_EDGE_DIRECTIONS_COUNT];

	for (uint i = 0u; i < BOUNDING_BOX_EDGE_DIRECTIONS_COUNT; ++i)
	{
		result[i] = rotateVector(obb.rotation, orth(i));
	}
	
	return result;
}

Range orientedBoundingBoxProjectOnLine(in OrientedBoundingBox obb, in Line l)
{
	const Line transformedL = transformLine(
		transformMult(
			makeTransform(quatConjugate(obb.rotation)),
			makeTransform(-obb.translation)),
		l);
	
	return boundingBoxProjectOnLine(
		makeBoundingBox(-obb.halfSize, obb.halfSize),
		transformedL);
}

OrientedBoundingBox transformOrientedBoundingBox(in Transform t, in OrientedBoundingBox obb)
{
	return makeOrientedBoundingBox(
		transformQuat(t, obb.rotation),
		transformPoint(t, obb.translation),
		transformSize(t, obb.halfSize));
}