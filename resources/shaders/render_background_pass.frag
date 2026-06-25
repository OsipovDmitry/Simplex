#include<background.glsl>
#include<map.glsl>

in vec3 v_texCoords;

out vec4 o_fragColor0;

void main(void)
{
    vec3 color = vec3(1.0f);
	color *= backgroundEnvironmentColor();
    
	uint environmentMapID = backgroundEnvironmentMapID();
	if (environmentMapID != 0xFFFFFFFFu)
	{
		const TextureHandle textureHandle = mapTextureHandle(environmentMapID);
		if (!isTextureHandleEmpty(textureHandle))
		{
			float blurPower = backgroundBlurPower();
			color *= textureLod(samplerCube(textureHandle), v_texCoords, blurPower).rgb;
		}
	}
	
    o_fragColor0 = vec4(color, 1.0f);
}