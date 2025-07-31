#include<oit_node.glsl>

layout (std430) buffer ssbo_OITNodesBuffer { uint OITMaxNumNodes; uint OITNumNodes; OITNode OITNodes[]; };
layout (r32ui) uniform uimage2DRect u_OITIndicesImage;

out vec4 o_fragColor;

vec4 blend(in vec4 src, in vec4 dst)
{
    return vec4(src.rgb * src.a + dst.rgb * (1.0f - src.a), src.a + dst.a * (1.0f - src.a));
}

void main(void)
{
    vec4 color = vec4(0.0f);
    
    uint OITIndex = imageLoad(u_OITIndicesImage, ivec2(gl_FragCoord.xy)).r;
    while (OITIndex != 0xFFFFFFFF)
    {
        color = blend(vec4(unpackOITNodeFinalColor(OITNodes[OITIndex]), unpackOITNodeAlpha(OITNodes[OITIndex])), color);
        OITIndex = OITNodes[OITIndex].next;
    }

    o_fragColor = color;
}