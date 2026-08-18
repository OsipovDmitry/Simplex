#include<bones_transforms_data.glsl>
#include<drawable.glsl>
#include<draw_data.glsl>
#include<mesh.glsl>
#include<shadow.glsl>
#include<shadow_data.glsl>
#include<shadow_maps.glsl>
#include<shadow_transforms_data.glsl>
#include<skeletal_animated_data.glsl>
#include<vertex_data.glsl>

#include<math/range.glsl>
#include<math/transform.glsl>
#include<math/utils.glsl>

flat out uint v_meshID;
flat out uint v_materialID;
flat out uint v_layerID;
out float v_linearNormalizedDepth;
out vec2 v_texCoords;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_ClipDistance[6];
};

void main(void)
{
	const uint shadowDataID = gl_BaseInstance;
	const uint drawDataID = shadowDataDrawDataID(shadowDataID);

	const uint drawableID = drawDataDrawableID(drawDataID);
	const uint skeletalAnimatedDataID = drawDataSkeletalAnimatedDataID(drawDataID);
	
	uint bonesTransformsDataOffset = 0xFFFFFFFFu;
	if (skeletalAnimatedDataID != 0xFFFFFFFFu)
		bonesTransformsDataOffset = skeletalAnimatedDataBonesTransformsDataOffset(skeletalAnimatedDataID);
	
	v_meshID = drawableMeshID(drawableID);
	v_materialID = drawableMaterialID(drawableID);
	
	const uint positionNormalTexCoordsDataOffset = meshPositionNormalTexCoordsDataOffset(v_meshID);
	const uint boneDataOffset = meshBoneDataOffset(v_meshID);
	
	vec3 position = vec3(0.0f);
	vec2 texCoords = vec2(0.0f);
	if (positionNormalTexCoordsDataOffset != 0xFFFFFFFFu)
	{
		vec3 normal; // no need later
		verticesDataPositionNormalTexCoords(positionNormalTexCoordsDataOffset, gl_VertexID, position, normal, texCoords);
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
	}
	
	const uint shadowID = shadowDataShadowID(shadowDataID);
	const uint layerID = shadowDataLayerID(shadowDataID, uint(gl_InstanceID));
	const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
	
	const uint mapSize = shadowMapSize(shadowID);
	const uvec3 mapCoords = shadowTransformsDataMapCoords(transformsDataOffset, layerID);
	
	const uint shadowAtlasSize = shadowMapsAtlasSize();
	const float scale = float(mapSize) / float(shadowAtlasSize);
	const vec2 translation = vec2(mapCoords.xy) / float(shadowAtlasSize);
	
	const vec3 positionWS = transformPoint(drawDataTransform(drawDataID), position);
	const vec3 positionLVS = transformPoint(shadowTransformsDataViewTransform(transformsDataOffset, layerID), positionWS);
	const vec4 positionLCS = shadowTransformsDataProjectionMatrix(transformsDataOffset, layerID) * vec4(positionLVS, 1.0f);
	
	for (uint i = 0u; i < 3u; ++i)
	{
		gl_ClipDistance[2u * i + 0u] = positionLCS[3u] - positionLCS[i];
		gl_ClipDistance[2u * i + 1u] = positionLCS[i] + positionLCS[3u];
	}
	
	vec2 NDC_XY = positionLCS.xy / positionLCS.w;
	NDC_XY = NO2ZO(NDC_XY);
	NDC_XY = NDC_XY * scale + translation;
	NDC_XY = ZO2NO(NDC_XY);
	NDC_XY = NDC_XY * positionLCS.w;
	
	const Range layerZRange = shadowTransformsDataZRange(transformsDataOffset, layerID);
	const float linearNormalizedDepth = rangeProjectOn(layerZRange, -positionLVS[2u]);
	
	gl_Position = vec4(NDC_XY, positionLCS.zw);
	v_layerID = mapCoords[2u];
	v_linearNormalizedDepth = linearNormalizedDepth;
	v_texCoords = texCoords;
}
