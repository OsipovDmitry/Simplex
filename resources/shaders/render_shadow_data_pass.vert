#include<bones_transforms_data.glsl>
#include<drawable.glsl>
#include<draw_data.glsl>
#include<mesh.glsl>
#include<render_info.glsl>
#include<shadow.glsl>
#include<shadow_data.glsl>
#include<shadow_transforms_data.glsl>
#include<skeletal_animated_data.glsl>
#include<vertex_data.glsl>

#include<math/transform.glsl>
#include<math/utils.glsl>

flat out uint v_layerID;
flat out uint v_meshID;
flat out uint v_materialID;
out float v_linearNormalizedDepth;
out vec2 v_texCoords;
out vec4 v_color;

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
	relativeOffset += normalComponentsCount;
	
	const uint texCoordsComponentsCount = meshTexCoordsComponentsCount(v_meshID);
	vec2 texCoords = vec2(0.0f);
	if (texCoordsComponentsCount > 0u)
	{
		texCoords = verticesDataVertexTexCoords(verticesDataOffset, vertexStride, vertexID, relativeOffset, texCoordsComponentsCount);
		relativeOffset += texCoordsComponentsCount;
	}
	
	const bool hasTangent = meshTangentFlag(v_meshID);
	relativeOffset += (hasTangent) ? 4u : 0u;

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
		
		position = vec3(boneTransform * vec4(position, 1.0f));
		relativeOffset += 2u * bonesCount;
	}
	
	const uint colorComponentsCount = meshColorComponentsCount(v_meshID);
	vec4 color = vec4(1.0f);
	if (colorComponentsCount > 0u)
	{
		color = verticesDataVertexColor(verticesDataOffset, vertexStride, vertexID, relativeOffset, colorComponentsCount);
		relativeOffset += colorComponentsCount;
	}
	
	const uint shadowID = shadowDataShadowID(shadowDataID);
	const uint layerID = shadowDataLayerID(shadowDataID, uint(gl_InstanceID));
	const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
	
	const uint mapSize = shadowMapSize(shadowID);
	const uvec3 mapCoords = shadowTransformsDataMapCoords(transformsDataOffset, layerID);
	
	const uint shadowAtlasSize = renderInfoShadowAtlasSize();
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
	const float linearNormalizedDepth = (-positionLVS[2u] - rangeStart(layerZRange)) / (rangeEnd(layerZRange) - rangeStart(layerZRange));
	
	gl_Position = vec4(NDC_XY, positionLCS.zw);
	v_layerID = mapCoords[2u];
	v_linearNormalizedDepth = linearNormalizedDepth;
	v_texCoords = texCoords;
	v_color = color;	
}
