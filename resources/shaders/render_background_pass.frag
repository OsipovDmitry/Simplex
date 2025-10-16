#extension GL_ARB_bindless_texture : enable

#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_materialMapsBuffer { uvec2 materialMaps[]; };
layout (std430) readonly buffer ssbo_backgroundBuffer { BackgroundDescription background; };

in vec3 v_texCoords;

out vec4 o_fragColor0;

void main(void)
{
    vec3 color = vec3(1.0f);
	color *= backgroundEnvironmentColor(background);
    
	if (background.environmentMapID != 0xFFFFFFFFu)
	{
		float blurPower = backgroundBlurPower(background);
		color *= textureLod(samplerCube(materialMaps[background.environmentMapID]), v_texCoords, blurPower).rgb;
	}
	
    o_fragColor0 = vec4(color, 1.0f);
}