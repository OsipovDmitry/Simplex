#include<gammacorrection.glsl>

uniform sampler2DRect u_GBufferColorMap0;

out vec4 o_fragColor;

void main(void)
{
    vec3 color = toSRGB(texelFetch(u_GBufferColorMap0, ivec2(gl_FragCoord.xy)).rgb);
    o_fragColor = vec4(color, 1.0f);
}