#include<descriptions.glsl>

uniform mat4x4 u_viewProjectionMatrix;

layout (std430) readonly buffer ssbo_vertexDataBuffer { float vertexData[]; };
layout (std430) readonly buffer ssbo_elementsBuffer { uint elements[]; };
layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };
layout (std430) readonly buffer ssbo_drawablesBuffer { DrawableDescription drawables[]; };
layout (std430) readonly buffer ssbo_drawDataBuffer {
	DrawDataBufferReservedData drawDataBufferReservedData;
	DrawDataDescription drawData[];
};

flat out uint v_meshID;
flat out uint v_materialID;
out vec3 v_normal;
out vec2 v_texCoords;
out vec3 v_tangent;
out vec3 v_binormal;
out vec4 v_color;

vec3 vertexDataPosition(in uint vertexDataOffset, in uint vertexStride, in uint vertexIndex, in uint vertexRelativeOffset, in uint numComponents)
{
	vec3 result = vec3(0.0f);
	for (uint i = 0u; i < min(3u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + i];
	return result;
}

vec3 vertexDataNormal(in uint vertexDataOffset, in uint vertexStride, in uint vertexIndex, in uint vertexRelativeOffset, in uint numComponents)
{
	vec3 result = vec3(0.0f);
	for (uint i = 0u; i < min(3u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + i];
	return normalize(result);
}

vec2 vertexDataTexCoords(in uint vertexDataOffset, in uint vertexStride, in uint vertexIndex, in uint vertexRelativeOffset, in uint numComponents)
{
	vec2 result = vec2(0.0f);
	for (uint i = 0u; i < min(2u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + i];
	return result;
}

void vertexDataBoneIDAndWeight(
	in uint vertexDataOffset,
	in uint vertexStride,
	in uint vertexIndex,
	in uint vertexRelativeOffset,
	in uint boneIndex,
	out uint boneID,
	out float boneWeight)
{
	boneID = floatBitsToUint(vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + 2u * boneIndex + 0u]);
	boneWeight = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + 2u * boneIndex + 1u];
}

void vertexDataTangentAndBinormalFlag(in uint vertexDataOffset, in uint vertexStride, in uint vertexIndex, in uint vertexRelativeOffset, out vec3 tangent, out float binormalFlag)
{
	for (uint i = 0u; i < 3u; ++i)
		tangent[i] = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + i];
	normalize(tangent);
	binormalFlag = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + 3u];
}

vec4 vertexDataColor(in uint vertexDataOffset, in uint vertexStride, in uint vertexIndex, in uint vertexRelativeOffset, in uint numComponents)
{
	vec4 result = vec4(vec3(0.0f), 1.0f);
	switch (numComponents)
	{
	case 1u:
	{
		result.rgb = vec3(vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + 0u]);
		break;
	}
	case 2u:
	{
		result.rgb = vec3(vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + 0u]);
		result.a = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + 1u];
		break;
	}
	case 3u:
	case 4u:
	{
		for (uint i = 0u; i < numComponents; ++i)
			result[i] = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + i];
		break;
	}
	default:
		break;
	}
	return result;
}

void main(void)
{
	uint drawDataID = gl_BaseInstance;
	if (drawDataID < drawDataBufferReservedData.count)
	{
		DrawDataDescription drawDataDescription = drawData[drawDataID];
		
		mat4x4 modelMatrix = drawDataModelMatrix(drawDataDescription);
		mat3x3 normalMatrix = drawDataNormalMatrix(drawDataDescription);
		
		DrawableDescription drawableDescription = drawables[drawDataDrawableID(drawDataDescription)];
		
		v_meshID = drawableDescription.meshID;
		v_materialID = drawableDescription.materialID;
		
		MeshDescription meshDescription = meshes[v_meshID];
		
		uint elementID = elements[gl_VertexID];
		
		uint vertexStride = meshVertexStride(meshDescription);
		uint vertexRelativeOffset = 0u;
		
		vec3 position = vec3(0.0f);
		uint positionComponentsCount = meshPositionComponentsCount(meshDescription);
		if (positionComponentsCount > 0u)
		{
			position = vertexDataPosition(meshDescription.vertexDataOffset, vertexStride, elementID, vertexRelativeOffset, positionComponentsCount);
			vertexRelativeOffset += positionComponentsCount;
		}
		
		vec3 normal = vec3(0.0f);
		uint normalComponentsCount = meshNormalComponentsCount(meshDescription);
		if (normalComponentsCount > 0u)
		{
			normal = vertexDataNormal(meshDescription.vertexDataOffset, vertexStride, elementID, vertexRelativeOffset, normalComponentsCount);
			vertexRelativeOffset += normalComponentsCount;
		}
		
		vec2 texCoords = vec2(0.0f);
		uint texCoordsComponentsCount = meshTexCoordsComponentsCount(meshDescription);
		if (texCoordsComponentsCount > 0u)
		{
			texCoords = vertexDataTexCoords(meshDescription.vertexDataOffset, vertexStride, elementID, vertexRelativeOffset, texCoordsComponentsCount);
			vertexRelativeOffset += texCoordsComponentsCount;
		}
		
		vec3 tangent = vec3(0.0f);
		vec3 binormal = vec3(0.0f);
		if (meshTangentFlag(meshDescription))
		{
			float binormalFlag = 0.0f;
			vertexDataTangentAndBinormalFlag(meshDescription.vertexDataOffset, vertexStride, elementID, vertexRelativeOffset, tangent, binormalFlag);
			binormal = normalize(cross(normal, tangent) * binormalFlag);
			vertexRelativeOffset += 4u;
		}
		
		uint numBones = meshBonesCount(meshDescription);
		if (numBones > 0u)
		{
			for (uint i = 0u; i < numBones; ++i)
			{
				uint boneID = 0u;
				float boneWeight = 0.0f;
				vertexDataBoneIDAndWeight(meshDescription.vertexDataOffset, vertexStride, elementID, vertexRelativeOffset, i, boneID, boneWeight);
				
				// transform "position", "normal", "tangent" and "binormal" by bone
			}
			vertexRelativeOffset += 2u * numBones;
		}
		
		vec4 color = vec4(1.0f);
		uint colorComponentsCount = meshColorComponentsCount(meshDescription);
		if (colorComponentsCount > 0u)
		{
			color = vertexDataColor(meshDescription.vertexDataOffset, vertexStride, elementID, vertexRelativeOffset, colorComponentsCount);
			vertexRelativeOffset += colorComponentsCount;
		}
		
		gl_Position = u_viewProjectionMatrix * modelMatrix * vec4(position, 1.0f);
		v_normal = normalMatrix * normal;
		v_texCoords = texCoords;
		v_tangent = normalMatrix * tangent;
		v_binormal = normalMatrix * binormal;
		v_color = color;
	}	
}
