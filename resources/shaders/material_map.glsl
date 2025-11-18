layout (std430) readonly buffer ssbo_materialMapsBuffer { uvec2 materialMaps[]; };

uvec2 materialMap(in uint materialMapID)
{
	return materialMaps[materialMapID];
}