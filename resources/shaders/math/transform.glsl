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

Transform makeIdentityTransform()
{
	return Transform(Quat(0.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f), 1.0f);
}

vec3 transformVector(in Transform t, in vec3 v)
{
	return rotateVector(t.rotation, v);
}

vec3 transformPoint(in Transform t, in vec3 p)
{
	return transformVector(t, p * t.scale) + t.translation;
}

float transformDistance(in Transform t, in float d)
{
	return t.scale * d;
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
	const vec3 translation = t1.translation + (t1.scale * rotateVector(t1.rotation, t2.translation));
	const Quat rotation = quatMult(t1.rotation, t2.rotation);
	const float scale = t1.scale * t2.scale;
	return makeTransform(scale, rotation, translation);
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