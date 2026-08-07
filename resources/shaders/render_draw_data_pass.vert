#include<bones_transforms_data.glsl>
#include<camera.glsl>
#include<drawable.glsl>
#include<draw_data.glsl>
#include<mesh.glsl>
#include<skeletal_animated_data.glsl>
#include<vertex_data.glsl>

#include<math/transform.glsl>

flat out uint v_meshID;
flat out uint v_materialID;
out vec3 v_normal;
out vec2 v_texCoords;
out vec3 v_tangent;
out vec3 v_binormal;

void main(void)
{
	const uint drawDataID = gl_BaseInstance;
	
	const Transform modelTransform = drawDataTransform(drawDataID);
	const uint drawableID = drawDataDrawableID(drawDataID);
	const uint skeletalAnimatedDataID = drawDataSkeletalAnimatedDataID(drawDataID);
	
	uint bonesTransformsDataOffset = 0xFFFFFFFFu;
	if (skeletalAnimatedDataID != 0xFFFFFFFFu)
		bonesTransformsDataOffset = skeletalAnimatedDataBonesTransformsDataOffset(skeletalAnimatedDataID);
	
	v_meshID = drawableMeshID(drawableID);
	v_materialID = drawableMaterialID(drawableID);
	
	const uint positionNormalTexCoordsDataOffset = meshPositionNormalTexCoordsDataOffset(v_meshID);
	const uint tangentDataOffset = meshTangentDataOffset(v_meshID);
	const uint boneDataOffset = meshBoneDataOffset(v_meshID);
	
	const bool hasNormal = meshHasNormals(v_meshID);
	const bool hasTangent = tangentDataOffset != 0xFFFFFFFFu;
	
	vec3 position = vec3(0.0f);
	vec3 normal = vec3(0.0f);
	vec2 texCoords = vec2(0.0f);
	if (positionNormalTexCoordsDataOffset != 0xFFFFFFFFu)
	{
		verticesDataPositionNormalTexCoords(positionNormalTexCoordsDataOffset, gl_VertexID, position, normal, texCoords);
	}
	
	vec3 tangent = vec3(0.0f);
	vec3 binormal = vec3(0.0f);
	if (hasNormal && hasTangent)
	{
		float binormalFlag = 0.0f;
		verticesDataTangentAndBinormalFlag(tangentDataOffset, gl_VertexID, tangent, binormalFlag);
		binormal = normalize(cross(normal, tangent) * binormalFlag);
	}
	
	if ((bonesTransformsDataOffset != 0xFFFFFFFFu) && (boneDataOffset != 0xFFFFFFFFu))
	{
		const uint bonesCount = meshBonesCount(v_meshID);
		
		// TODO: make boneTransform Transform instead of mat4x4
		mat4x4 boneTransform = mat4x4(0.0f);
		
		for (uint i = 0u; i < bonesCount; ++i)
		{
			uint boneID = 0xFFFFFFFFu;
			float boneWeight = 0.0f;
			verticesDataBoneIDAndWeight(boneDataOffset, bonesCount, gl_VertexID, i, boneID, boneWeight);
			
			if (boneID == 0xFFFFFFFFu)
				continue; //break;
				
			boneTransform += transformMat4x4(bonesTransformsDataBoneTransform(bonesTransformsDataOffset, boneID)) * boneWeight;
		}
		
		position = vec3(boneTransform * vec4(position, 1.0f));
		
		if (hasNormal)
		{
			const mat3 boneNormalMatrix = transpose(inverse(mat3(boneTransform)));
			normal = boneNormalMatrix * normal;
			
			if (hasTangent)
			{
				tangent = boneNormalMatrix * tangent;
				binormal = boneNormalMatrix * binormal;
			}
		}
	}
	
	gl_Position = cameraViewProjectionMatrix() * vec4(transformPoint(modelTransform, position), 1.0f);
	v_normal = transformVector(modelTransform, normal);
	v_texCoords = texCoords;
	v_tangent = transformVector(modelTransform, tangent);
	v_binormal = transformVector(modelTransform, binormal);
}
