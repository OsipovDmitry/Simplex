#include<bones_transforms_data.glsl>
#include<camera.glsl>
#include<drawable.glsl>
#include<draw_data.glsl>
#include<mesh.glsl>
#include<vertex_data.glsl>

flat out uint v_meshID;
flat out uint v_materialID;
out vec3 v_normal;
out vec2 v_texCoords;
out vec3 v_tangent;
out vec3 v_binormal;
out vec4 v_color;

void main(void)
{
	const uint drawDataID = gl_BaseInstance;
	
	const Transform modelTransform = drawDataTransform(drawDataID);
	const uint drawableID = drawDataDrawableID(drawDataID);
	const uint bonesTransformsDataOffset = drawDataBonesTransformsDataOffset(drawDataID);
	
	v_meshID = drawableMeshID(drawableID);
	v_materialID = drawableMaterialID(drawableID);
	
	const uint verticesDataOffset = meshVerticesDataOffset(v_meshID);
	const uint vertexStride = meshVertexStride(v_meshID);
	const uint vertexID = (meshElementsDataOffset(v_meshID) == 0xFFFFFFFFu) ? gl_VertexID : elementsDataElementID(gl_VertexID);
	uint relativeOffset = 0u;
	
	const uint positionComponentsCount = meshPositionComponentsCount(v_meshID);
	vec3 position = vec3(0.0f);
	if (positionComponentsCount > 0u)
	{
		position = verticesDataVertexPosition(verticesDataOffset, vertexStride, vertexID, relativeOffset, positionComponentsCount);
		relativeOffset += positionComponentsCount;
	}
	
	const uint normalComponentsCount = meshNormalComponentsCount(v_meshID);
	vec3 normal = vec3(0.0f);
	if (normalComponentsCount > 0u)
	{
		normal = verticesDataVertexNormal(verticesDataOffset, vertexStride, vertexID, relativeOffset, normalComponentsCount);
		relativeOffset += normalComponentsCount;
	}
	
	const uint texCoordsComponentsCount = meshTexCoordsComponentsCount(v_meshID);
	vec2 texCoords = vec2(0.0f);
	if (texCoordsComponentsCount > 0u)
	{
		texCoords = verticesDataVertexTexCoords(verticesDataOffset, vertexStride, vertexID, relativeOffset, texCoordsComponentsCount);
		relativeOffset += texCoordsComponentsCount;
	}
	
	vec3 tangent = vec3(0.0f);
	vec3 binormal = vec3(0.0f);
	const bool hasTangent = meshTangentFlag(v_meshID);
	if (hasTangent)
	{
		float binormalFlag = 0.0f;
		verticesDataVertexTangentAndBinormalFlag(verticesDataOffset, vertexStride, vertexID, relativeOffset, tangent, binormalFlag);
		binormal = normalize(cross(normal, tangent) * binormalFlag);
		relativeOffset += 4u;
	}

	if (bonesTransformsDataOffset != 0xFFFFFFFFu)
	{
		// TODO: make boneTransform Transform instead of mat4x4
		mat4x4 boneTransform = mat4x4(0.0f);
		
		const uint bonesCount = meshBonesCount(v_meshID);
		for (uint i = 0u; i < bonesCount; ++i)
		{
			uint boneID = 0u;
			float boneWeight = 0.0f;
			verticesDataVertexBoneIDAndWeight(verticesDataOffset, vertexStride, vertexID, relativeOffset, i, boneID, boneWeight);
			
			if (boneID == 0xFFFFFFFFu)
				continue; //break;
				
			boneTransform += transformMat4x4(bonesTransformsDataBoneTransform(bonesTransformsDataOffset, boneID)) * boneWeight;
		}
		
		mat3 boneNormalMatrix = transpose(inverse(mat3(boneTransform)));
		
		position = vec3(boneTransform * vec4(position, 1.0f));
		normal = boneNormalMatrix * normal;
		if (hasTangent)
		{
			tangent = boneNormalMatrix * tangent;
			binormal = boneNormalMatrix * binormal;
		}
		
		relativeOffset += 2u * bonesCount;
	}
	
	const uint colorComponentsCount = meshColorComponentsCount(v_meshID);
	vec4 color = vec4(1.0f);
	if (colorComponentsCount > 0u)
	{
		color = verticesDataVertexColor(verticesDataOffset, vertexStride, vertexID, relativeOffset, colorComponentsCount);
		relativeOffset += colorComponentsCount;
	}
	
	gl_Position = cameraViewProjectionMatrix() * vec4(transformPoint(modelTransform, position), 1.0f);
	v_normal = transformVector(modelTransform, normal);
	v_texCoords = texCoords;
	v_tangent = transformVector(modelTransform, tangent);
	v_binormal = transformVector(modelTransform, binormal);
	v_color = color;	
}
