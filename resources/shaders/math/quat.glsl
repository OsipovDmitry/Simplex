#include<constants.glsl>

#define Quat vec4

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

	return normalize(Quat(t, real_part));
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
	return Quat(
		p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y,
		p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z,
		p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x,
		p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z);
}

Quat quatConjugate(in Quat q)
{
	return Quat(-q.x, -q.y, -q.z, q.w);
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