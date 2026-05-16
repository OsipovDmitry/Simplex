#include<constants.glsl>

#define Quat vec4

Quat makeQuat(in vec4 q)
{
	return Quat(normalize(q));
}

Quat makeQuat(in float x, in float y, in float z, in float w)
{
	return makeQuat(vec4(x, y, z, w));
}

Quat makeIdentityQuat()
{
	return makeQuat(0.0f, 0.0f, 0.0f, 1.0f);
}

// Create a quaternion from two normalized axis
Quat makeQuat(in vec3 u, in vec3 v)
{
	const float norm_u_norm_v = sqrt(dot(u, u) * dot(v, v));
	float real_part = norm_u_norm_v + dot(u, v);
	vec3 t;

	if (real_part < EPS * norm_u_norm_v)
	{
		real_part = 0.0f;
		t = abs(u.x) > abs(u.z) ? vec3(-u.y, u.x, 0.0f) : vec3(0.0f, -u.z, u.y);
	}
	else
		t = cross(u, v);

	return makeQuat(vec4(t, real_part));
}

Quat makeQuat(in mat3x3 m)
{
	const float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
	const float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
	const float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
	const float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

	uint biggestIndex = 0u;
	float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1u;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2u;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3u;
	}

	const float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
	const float mult = 0.25f / biggestVal;

	if (biggestIndex == 0u)
	{
		return makeQuat((m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult, biggestVal);
	}
	if (biggestIndex == 1u)
	{
		return makeQuat(biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] - m[2][1]) * mult);
	}
	if (biggestIndex == 2u)
	{
		return makeQuat((m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult, (m[2][0] - m[0][2]) * mult);
	}
	if (biggestIndex == 3u)
	{
		return makeQuat((m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal, (m[0][1] - m[1][0]) * mult);
	}
	
	return makeIdentityQuat();
}

Quat makeLookAtQuat(in vec3 direction, in vec3 up)
{
	mat3x3 Result;

	Result[2] = -direction;
	const vec3 Right = cross(up, Result[2]);
	Result[0] = Right * inversesqrt(max(EPS, dot(Right, Right)));
	Result[1] = cross(Result[2], Result[0]);

	return makeQuat(Result);
}

vec3 rotateVector(in Quat q, in vec3 v)
{
	const vec3 QuatVector = q.xyz;
	const vec3 uv = cross(QuatVector, v);
	const vec3 uuv = cross(QuatVector, uv);
	return v + ((uv * q.w) + uuv) * 2.0f;
}

Quat quatMult(in Quat p, in Quat q)
{
	return makeQuat(
		p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y,
		p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z,
		p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x,
		p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z);
}

Quat quatConjugate(in Quat q)
{
	return makeQuat(-q.x, -q.y, -q.z, q.w);
}

Quat slerp(in Quat x, in Quat y, in float a)
{
	Quat z = y;

	float cosTheta = dot(x, y);
	if(cosTheta < 0.0f)
	{
		z = -y;
		cosTheta = -cosTheta;
	}

	if(cosTheta > 1.0f - EPS)
	{
		return mix(x, z, a);
	}
	else
	{
		const float angle = acos(cosTheta);
		return (sin((1.0f - a) * angle) * x + sin(a * angle) * z) / sin(angle);
	}
}