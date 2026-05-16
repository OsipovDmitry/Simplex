layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

#include<render_info.glsl>
#include<shadow.glsl>
#include<shadow_transforms_data.glsl>

flat in uint v_shadowID[];

flat out ivec2 g_minMapPoint;
flat out ivec2 g_maxMapPoint;

void main(void)
{
	const uint atlasSize = renderInfoShadowAtlasSize();
	
	const uint mapSize = shadowMapSize(v_shadowID[0u]);
	const float mapScale = float(mapSize) / float(atlasSize);
	
	const uint layersCount = shadowLayersCount(v_shadowID[0u]);
	const uint transformsDataOffset = shadowTransformsDataOffset(v_shadowID[0u]);
	
	for (uint layerID = 0u; layerID < layersCount; ++layerID)
	{
		const uvec3 mapCoords = shadowTransformsDataMapCoords(transformsDataOffset, layerID);

		const vec2 mapTranslation = vec2(mapCoords.xy) / float(atlasSize);
		gl_Layer = int(mapCoords[2u]);
		
		g_minMapPoint = ivec2(mapCoords.xy);
		g_maxMapPoint = g_minMapPoint + ivec2(int(mapSize) - 1u);
		
		for (uint i = 0u; i < 3u; ++i)
		{
			gl_Position = vec4(
				2.0f * (mapScale * gl_in[i].gl_Position.xy + mapTranslation) - vec2(1.0f),
				0.0f,
				1.0f);
			EmitVertex();
		}
		
		EndPrimitive();
	}
}
