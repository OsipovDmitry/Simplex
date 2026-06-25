#include<constants.glsl>
#include<line.glsl>
#include<range.glsl>
#include<transform.glsl>
#include<quat.glsl>
#include<utils.glsl>

struct Pyramid
{
	Quat rotation;
	vec3 translation;
	float height;
	float halfAngle;
};

Pyramid makePyramid(in Quat rotation, in vec3 translation, in float height, in float halfAngle)
{
	return Pyramid(rotation, translation, height, halfAngle);
}

Quat pyramidRotation(in Pyramid p)
{
	return p.rotation;
}

vec3 pyramidTranslation(in Pyramid p)
{
	return p.translation;
}

float pyramidHeight(in Pyramid p)
{
	return p.height;
}

float pyramidHalfAngle(in Pyramid p)
{
	return p.halfAngle;
}

void pyramidCalculateFaceNormalLinesAndRanges(
	in Pyramid p,
	out Line faceNormalLines[PYRAMID_FACE_NORMAL_LINES_COUNT],
	out Range ranges[PYRAMID_FACE_NORMAL_LINES_COUNT])
{	
	const float pSinHalfAngle = sin(p.halfAngle);
	const float pCosHalfAngle = cos(p.halfAngle);
	
	const float pLocalSideNormalXY = -pCosHalfAngle;
	const float pLocalSideNormalZ = -pSinHalfAngle;
	const Range pSideRange = makeRange(0.0f, 2.0f * p.height * pSinHalfAngle);
	
	for (uint i = 0u; i < 4u; ++i)
	{
		const uint orthID = bitfieldExtract(i, 0, 1);
		const float sign = ZO2NO(float(bitfieldExtract(i, 1, 1)));
		
		const vec3 pLocalSideNormal = orth(orthID) * pLocalSideNormalXY * sign + orth(2u) * pLocalSideNormalZ;
		
		faceNormalLines[i] = makeLine(p.translation, rotateVector(p.rotation, pLocalSideNormal));
		ranges[i] = pSideRange;
	}
	
	const vec3 pLocalBaseNormal = orth(2u);
	faceNormalLines[4u] = makeLine(p.translation, rotateVector(p.rotation, pLocalBaseNormal));
	ranges[4u] = makeRange(-p.height, 0.0f); 
}

vec3[PYRAMID_EDGE_DIRECTIONS_COUNT] pyramidCalculateEdgeDirections(in Pyramid p)
{
	vec3 result[PYRAMID_EDGE_DIRECTIONS_COUNT];
	
	const vec3 pLocalSideEdgeAbsDirection = normalize(vec3(vec2(tan(p.halfAngle)), -1.0f));
	
	for (uint i = 0u; i < 4u; ++i)
	{
		const vec3 sign = ZO2NO(vec3(float(bitfieldExtract(i, 0, 1)), float(bitfieldExtract(i, 1, 1)), 1.0f));
		result[i] = rotateVector(p.rotation, pLocalSideEdgeAbsDirection * sign);
	}
	
	result[4u] = rotateVector(p.rotation, orth(0u));
	result[5u] = rotateVector(p.rotation, orth(1u));
	
	return result;
}

Range pyramidProjectOnLine(in Pyramid p, in Line l)
{
	const Line transformedL = transformLine(
		transformInverted(makeTransform(1.0f, p.rotation, p.translation)),
		l);
		
	const float tanHalfAngle = tan(p.halfAngle);
		
	Range result = makeRange(lineProjectOn(transformedL, vec3(0.0f)));
	for (uint i = 0u; i < 4u; ++i)
	{
		const vec2 v = vec2(
			float(bitfieldExtract(i, 0, 1)),
			float(bitfieldExtract(i, 1, 1)));
			
		const vec3 point = p.height * vec3(ZO2NO(v) * tanHalfAngle, -1.0f);
		result = rangeExpand(result, lineProjectOn(transformedL, point));
	}
	
	return result;
}

bool pyramidIsPointInside(in Pyramid p, in vec3 v)
{
	const vec3 transformedV = transformPoint(
		transformInverted(makeTransform(1.0f, p.rotation, p.translation)),
		v);
		
	const float z = -transformedV[2u];
	
	if ((z < 0.0f) || (z > p.height))
		return false;
		
	const float d = z * tan(p.halfAngle);
	return all(lessThanEqual(abs(transformedV.xy), vec2(d)));
}

bool pyramidIsFrustumInside(in Pyramid p, in vec3 fPoints[FRUSTUM_POINTS_COUNT])
{
	for (uint i = 0u; i < FRUSTUM_POINTS_COUNT; ++i)
		if (!pyramidIsPointInside(p, fPoints[i]))
			return false;
	
	return true;
}

Pyramid transformPyramid(in Transform t, in Pyramid p)
{
	return makePyramid(
		transformQuat(t, p.rotation),
		transformPoint(t, p.translation),
		transformSize(t, p.height),
		p.halfAngle);
}