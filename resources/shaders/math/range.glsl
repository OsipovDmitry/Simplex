#include<constants.glsl>

#define Range vec2

Range makeRange(in vec2 v)
{
	return v;
}

Range makeRange(in float t0, in float t1)
{
	return makeRange(vec2(t0, t1));
}

Range makeRange(in float t)
{
	return makeRange(t, t);
}

Range makeRangeFromCenterAndHalfSize(in float center, in float halfSize)
{
	return makeRange(center - halfSize, center + halfSize);
}

Range makeEmptyRange()
{
	return makeRange(FLT_MAX, -FLT_MAX);
}

bool isRangeEmpty(in Range r)
{
	return r[0u] > r[1u];
}

float rangeStart(in Range r)
{
	return r[0u];
}

float rangeEnd(in Range r)
{
	return r[1u];
}

float rangeCenter(in Range r)
{
	return 0.5f * (r[1u] + r[0u]);
}

float rangeHalfSize(in Range r)
{
	return 0.5f * (r[1u] - r[0u]);
}

Range rangeExpand(in Range r, in float t)
{
	return makeRange(min(r[0u], t), max(r[1u], t));
}

Range rangeExpand(in Range r0, in Range r1)
{
	return makeRange(min(r0[0u], r1[0u]), max(r0[1u], r1[1u]));
}

Range rangeIntersect(in Range r0, in Range r1)
{
	return makeRange(max(r0[0u], r1[0u]), min(r0[1u], r1[1u]));
}

bool rangeLessThan(in Range r, in float t)
{
	return (r[1u] < t);
}

bool rangeGreaterThan(in Range r, in float t)
{
	return (r[0u] > t);
}

bool rangeIsPointInside(in Range r, in float t)
{
	return (r[0u] <= t) && (t <= r[1u]);
}

