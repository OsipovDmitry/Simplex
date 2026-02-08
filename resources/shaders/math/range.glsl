#include<constants.glsl>
#include<transform.glsl>

#define Range vec2

Range makeRange(in float t0, in float t1)
{
	return (t0 < t1) ? Range(t0, t1) : Range(t1, t0);
}

Range makeRange(in float t)
{
	return Range(t, t);
}

Range makeRange(in vec2 v)
{
	return Range(v[0u], v[1u]);
}

Range makeEmptyRange()
{
	return Range(FLT_MAX, -FLT_MAX);
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

Range rangeExpand(in Range r, in float t)
{
	return makeRange(min(r[0u], t), max(r[1u], t));
}

bool rangeLessThan(in Range r, in float t)
{
	return all(lessThan(r, vec2(t)));
}

bool rangeGreaterThan(in Range r, in float t)
{
	return all(greaterThan(r, vec2(t)));
}

bool rangeIsPointInside(in Range r, in float t)
{
	return (r[0u] <= t) && (t <= r[1u]);
}

Range absRange(in Range r)
{
	return makeRange(abs(r));
}

Range transformRange(in Transform t, in Range r)
{
	return makeRange(transformDistance(t, r[0u]), transformDistance(t, r[1u]));
}


