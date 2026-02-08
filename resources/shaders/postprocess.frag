#include<gammacorrection.glsl>

uniform sampler2DRect u_GBufferFinalMap;

out vec4 o_fragColor0;

void main(void)
{
    vec3 color = toSRGB(texelFetch(u_GBufferFinalMap, ivec2(gl_FragCoord.xy)).rgb);
}