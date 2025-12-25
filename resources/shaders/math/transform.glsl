#include<quat.glsl>

struct Transform
{
	Quat rotation;
	vec4 translationAndScale;
};

vec3 transformVector(in Transform t, in vec3 v)
{
	return rotateVector(t.rotation, v);
}

vec3 transformPoint(in Transform t, in vec3 p)
{
	return transformVector(t, p * t.translationAndScale.w) + t.translationAndScale.xyz;
}

float transformDistance(in Transform t, in float d)
{
	return t.translationAndScale.w * d;
}

Transform transformInverted(in Transform t)
{
	const float scale = 1.0f / t.translationAndScale.w;
	const Quat rotation = vec4(-vec3(t.rotation), t.rotation.w);
	const vec3 translation = -rotateVector(rotation, (scale * t.translationAndScale.xyz));
	return Transform(rotation, vec4(translation, scale));
}

Transform transformMult(in Transform t1, in Transform t2)
{
	const vec3 translation = t1.translationAndScale.xyz + t1.translationAndScale.w * rotateVector(t1.rotation, t2.translationAndScale.xyz);
	const Quat rotation = quatMult(t1.rotation, t2.rotation);
	const float scale = t1.translationAndScale.w * t2.translationAndScale.w;
	return Transform(rotation, vec4(translation, scale));
}