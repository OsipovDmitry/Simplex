#include<gammacorrection.glsl>

uniform sampler2DRect u_GBufferFinalMap;

out vec4 o_fragColor0;

//tmp
#include<packing.glsl>
uniform usampler2DRect u_GBufferColorMap0;
layout (r32ui) uniform uimage2DRect u_OITIndicesImage;

void main(void)
{
    vec3 color = toSRGB(texelFetch(u_GBufferFinalMap, ivec2(gl_FragCoord.xy)).rgb);
	//vec3 color = toSRGB(unpackF2x11_1x10(texelFetch(u_GBufferColorMap0, ivec2(gl_FragCoord.xy)).r));
	
	//uint OITIndex = imageLoad(u_OITIndicesImage, ivec2(gl_FragCoord.xy)).r;
	//vec3 color = vec3(0.0f);
	//if (OITIndex != 0xFFFFFFFFu)
	//	color = vec3(1.0f);
	o_fragColor0 = vec4(color, 1.0f);
}