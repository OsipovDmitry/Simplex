layout (std430) readonly buffer ssbo_vertexDataBuffer { float vertexData[]; };
layout (std430) readonly buffer ssbo_elementsBuffer { uint elements[]; };

vec3 vertexDataPosition(
	in uint vertexDataOffset,
	in uint vertexStride,
	in uint vertexID,
	in uint relativeOffset,
	in uint numComponents)
{
	vec3 result = vec3(0.0f);
	for (uint i = 0u; i < min(3u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + i];
	return result;
}

vec3 vertexDataNormal(
	in uint vertexDataOffset,
	in uint vertexStride,
	in uint vertexID,
	in uint relativeOffset,
	in uint numComponents)
{
	vec3 result = vec3(0.0f);
	for (uint i = 0u; i < min(3u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + i];
	return normalize(result);
}

vec2 vertexDataTexCoords(
	in uint vertexDataOffset,
	in uint vertexStride,
	in uint vertexID,
	in uint relativeOffset,
	in uint numComponents)
{
	vec2 result = vec2(0.0f);
	for (uint i = 0u; i < min(2u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + i];
	return result;
}

void vertexDataBoneIDAndWeight(
	in uint vertexDataOffset,
	in uint vertexStride,
	in uint vertexID,
	in uint relativeOffset,
	in uint boneIndex,
	out uint boneID,
	out float boneWeight)
{
	boneID = floatBitsToUint(vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + 2u * boneIndex + 0u]);
	boneWeight = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + 2u * boneIndex + 1u];
}

void vertexDataTangentAndBinormalFlag(
	in uint vertexDataOffset,
	in uint vertexStride,
	in uint vertexID,
	in uint relativeOffset,
	out vec3 tangent,
	out float binormalFlag)
{
	for (uint i = 0u; i < 3u; ++i)
		tangent[i] = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + i];
	normalize(tangent);
	binormalFlag = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + 3u];
}

vec4 vertexDataColor(
	in uint vertexDataOffset,
	in uint vertexStride,
	in uint vertexID,
	in uint relativeOffset,
	in uint numComponents)
{
	vec4 result = vec4(vec3(0.0f), 1.0f);
	switch (numComponents)
	{
	case 1u:
	{
		result.rgb = vec3(vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + 0u]);
		break;
	}
	case 2u:
	{
		result.rgb = vec3(vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + 0u]);
		result.a = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + 1u];
		break;
	}
	case 3u:
	case 4u:
	{
		for (uint i = 0u; i < numComponents; ++i)
			result[i] = vertexData[vertexDataOffset + vertexStride * vertexID + relativeOffset + i];
		break;
	}
	default:
		break;
	}
	return result;
}

uint elementID(in uint ID)
{
	return elements[ID];
}