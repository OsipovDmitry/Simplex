#include<hdr.glsl>
#include<tone_mapping.glsl>

//tmp
#include<geometry.glsl>
#include<camera.glsl>

out vec4 o_fragColor0;

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);
	
	vec3 color = HDRFetchColor(fragCoords);
	toneMappingApplyExposure(color);
	toneMappingApplyACES(color);
	toneMappingApplyGammaCorrection(color);

	o_fragColor0 = vec4(color, 1.0f);
	
	// tmp
	
	const int levelsCount = textureQueryLevels(sampler2D(GBuffer.depthTextureHandle));
	int totalX = 0;
	for (int i = 1; i < levelsCount; ++i)
	{
		const ivec2 size = textureSize(sampler2D(GBuffer.depthTextureHandle), i);
		const ivec2 texCoords = fragCoords - ivec2(totalX, 0);
		
		if (all(lessThanEqual(ivec2(0), texCoords)) && all(lessThan(texCoords, size)))
		{
			const float depth_z = geometryBufferDepth(texCoords, i);
			const float near = rangeStart(cameraZRange());
			const float far = rangeEnd(cameraZRange());
			
			float lin_z = (2.0 * near * far) / (far + near - depth_z * (far - near));
			lin_z = (lin_z - near) / (far - near);
			
			o_fragColor0 = vec4(vec3(pow(depth_z, 0.25f)), 1.0f);
			break;
		}
		
		totalX += size.x;
	}
}
