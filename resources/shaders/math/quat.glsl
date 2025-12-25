#define Quat vec4

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