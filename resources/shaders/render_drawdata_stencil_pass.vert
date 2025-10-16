#include<descriptions.glsl>

uniform mat4x4 u_viewProjectionMatrix;

layout (std430) readonly buffer ssbo_vertexDataBuffer { float vertexData[]; };
layout (std430) readonly buffer ssbo_elementsBuffer { uint elements[]; };
layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };
layout (std430) readonly buffer ssbo_pointLightsBuffer {
	LightBufferReservedData pointLightsBufferReservedData;
	PointLightDescription pointLights[];
};
layout (std430) readonly buffer ssbo_spotLightsBuffer {
	LightBufferReservedData spotLightsBufferReservedData;
	SpotLightDescription spotLights[];
};
layout (std430) readonly buffer ssbo_directionalLightsBuffer {
	LightBufferReservedData directionalLightsBufferReservedData;
	DirectionalLightDescription directionalLights[];
};
layout (std430) readonly buffer ssbo_imageBasedLightsBuffer {
	LightBufferReservedData imageBasedLightsBufferReservedData;
	ImageBasedLightDescription imageBasedLights[];
};

vec3 vertexDataPosition(in uint vertexDataOffset, in uint vertexStride, in uint vertexIndex, in uint vertexRelativeOffset, in uint numComponents)
{
	vec3 result = vec3(0.0f);
	for (uint i = 0u; i < min(3u, numComponents); ++i)
		result[i] = vertexData[vertexDataOffset + vertexStride * vertexIndex + vertexRelativeOffset + i];
	return result;
}

void main(void)
{
	uint totalLightsCount =
		pointLightsBufferReservedData.count +
		spotLightsBufferReservedData.count +
		directionalLightsBufferReservedData.count +
		imageBasedLightsBufferReservedData.count;
		
	uint lightID = gl_BaseInstance;
	if (lightID < totalLightsCount)
	{
		MeshDescription meshDescription;
		mat4x4 viewProjectionMatrix = mat4x4(1.0f);
		mat4x4 modelMatrix = mat4x4(1.0f);
		vec3 areaScale = vec3(1.0f); 		
		
		if (lightID < pointLightsBufferReservedData.count)
		{
			meshDescription = meshes[pointLightsBufferReservedData.areaMeshID];
			
			viewProjectionMatrix = u_viewProjectionMatrix;
			PointLightDescription desc = pointLights[lightID];
			modelMatrix = desc.modelMatrix;
			areaScale = pointLightAreaScale(desc);
		}
		else if ((lightID -= pointLightsBufferReservedData.count) < spotLightsBufferReservedData.count)
		{
			meshDescription = meshes[spotLightsBufferReservedData.areaMeshID];
		}
		else if ((lightID -= spotLightsBufferReservedData.count) < directionalLightsBufferReservedData.count)
		{
			meshDescription = meshes[directionalLightsBufferReservedData.areaMeshID];
		}
		else if ((lightID -= directionalLightsBufferReservedData.count) < imageBasedLightsBufferReservedData.count)
		{
			MeshDescription meshDescription = meshes[imageBasedLightsBufferReservedData.areaMeshID];
		}
		
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
		
		gl_Position = viewProjectionMatrix * modelMatrix * vec4(position * areaScale, 1.0f);
	}
}
