#include<quat.glsl>

struct Transform
{
	Quat rotation;
	vec3 translation;
	float scale;
};

Transform makeTransform(in float scale, in Quat rotation, in vec3 translation)
{
	return Transform(rotation, translation, scale);
}

Transform makeTransform(in float scale)
{
	return makeTransform(scale, makeIdentityQuat(), vec3(0.0f));
}

Transform makeTransform(in Quat rotation)
{
	return makeTransform(1.0f, rotation, vec3(0.0f));
}

Transform makeTransform(in vec3 translation)
{
	return makeTransform(1.0f, makeIdentityQuat(), translation);
}

Transform makeIdentityTransform()
{
	return makeTransform(1.0f, makeIdentityQuat(), vec3(0.0f));
}

Transform makeLookAtTransform(in vec3 eye, in vec3 center, in vec3 up)
{
	const Quat q = quatConjugate(makeLookAtQuat(normalize(center - eye), normalize(up)));
    return makeTransform(1.0f, q, -rotateVector(q, eye));
}

vec3 transformVector(in Transform t, in vec3 v)
{
	return rotateVector(t.rotation, v);
}

Quat transformQuat(in Transform t, in Quat q)
{
	return quatMult(t.rotation, q);
}

vec3 transformPoint(in Transform t, in vec3 p)
{
	return transformVector(t, p * t.scale) + t.translation;
}

float transformSize(in Transform t, in float s)
{
	return t.scale * s;
}

vec3 transformSize(in Transform t, in vec3 s)
{
	return t.scale * s;
}

Transform transformInverted(in Transform t)
{
	const float scale = 1.0f / t.scale;
	const Quat rotation = quatConjugate(t.rotation);
	const vec3 translation = -rotateVector(rotation, (scale * t.translation));
	return makeTransform(scale, rotation, translation);
}

Transform transformMult(in Transform t1, in Transform t2)
{
	return makeTransform(
		transformSize(t1, t2.scale),
		transformQuat(t1, t2.rotation),
		transformPoint(t1, t2.translation));
}

mat4x4 transformMat4x4(in Transform t)
{
	const float qxx = t.rotation.x * t.rotation.x;
	const float qyy = t.rotation.y * t.rotation.y;
	const float qzz = t.rotation.z * t.rotation.z;
	const float qxz = t.rotation.x * t.rotation.z;
	const float qxy = t.rotation.x * t.rotation.y;
	const float qyz = t.rotation.y * t.rotation.z;
	const float qwx = t.rotation.w * t.rotation.x;
	const float qwy = t.rotation.w * t.rotation.y;
	const float qwz = t.rotation.w * t.rotation.z;

	return mat4x4(
		vec4(
			(1.0f - 2.0f * (qyy +  qzz)) * t.scale,
			2.0f * (qxy + qwz),
			2.0f * (qxz - qwy),
			0.0f),
		vec4(
			2.0f * (qxy - qwz),
			(1.0f - 2.0f * (qxx +  qzz)) * t.scale,
			2.0f * (qyz + qwx),
			0.0f),
		vec4(
			2.0f * (qxz + qwy),
			2.0f * (qyz - qwx),
			(1.0f - 2.0f * (qxx +  qyy)) * t.scale,
			0.0f),
		vec4(t.translation, 1.0f));
}

vec3 projectPoint(in mat4x4 projectionMatrix, in vec3 v)
{
	vec4 ndc = projectionMatrix * vec4(v, 1.0f);
	return ndc.xyz / ndc.w;
}