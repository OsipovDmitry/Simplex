#include<gammacorrection.glsl>

uniform sampler2DRect u_GBufferFinalMap;

out vec4 o_fragColor0;

//tmp
uniform sampler2DRect u_GBufferColorMap0;
uniform sampler2DRect u_GBufferColorMap1;
uniform sampler2DRect u_GBufferColorMap2;

void main(void)
{
    //vec3 color = toSRGB(texelFetch(u_GBufferFinalMap, ivec2(gl_FragCoord.xy)).rgb);
	vec3 color = toSRGB(texelFetch(u_GBufferColorMap0, ivec2(gl_FragCoord.xy)).rgb);
	o_fragColor0 = vec4(color, 1.0f);
}