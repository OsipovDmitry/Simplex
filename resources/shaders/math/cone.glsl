#include<constants.glsl>
#include<line.glsl>
#include<range.glsl>
#include<transform.glsl>

struct Cone
{
	vec3 origin;
	vec3 direction;
	float height;
	float halfAngle;
};

Cone makeCone(in vec3 origin, in vec3 direction, in float height, in float halfAngle)
{
	return Cone(origin, normalize(direction), height, halfAngle);
}

vec3 coneOrigin(in Cone c)
{
	return c.origin;
}

vec3 coneDirection(in Cone c)
{
	return c.direction;
}

float coneHeight(in Cone c)
{
	return c.height;
}

float coneHalfAngle(in Cone c)
{
	return c.halfAngle;
}

Range coneProjectOnLine(in Cone c, in Line l)
{
    const vec3 coneEnd = c.origin + c.direction * c.height;

    const vec3 crossVec = cross(c.direction, lineDirection(l));
    const float crossVecLen = length(crossVec);

    Range result = makeRange(lineProjectOn(l, c.origin));

    if (crossVecLen < EPS)
    {
        result = rangeExpand(result, lineProjectOn(l, coneEnd));
    }
    else
    {
        const vec3 r = cross(c.direction, crossVec / crossVecLen) * sin(c.halfAngle) * c.height;
        result = rangeExpand(result, lineProjectOn(l, coneEnd + r));
        result = rangeExpand(result, lineProjectOn(l, coneEnd - r));
    }

    return result;
}

bool coneIsPointInside(in Cone c, in vec3 v)
{
	const vec3 CtoV = v - c.origin;
	const float t = dot(c.direction, CtoV);
	
	return (t <= c.height) && (t >= cos(c.halfAngle) * length(CtoV));
}

Cone transformCone(in Transform t, in Cone c)
{
	return makeCone(
		transformPoint(t, c.origin),
		transformVector(t, c.direction),
		transformDistance(t, c.height),
		c.halfAngle);
}