#include<oit_node.glsl>

uniform usampler2DRect u_GBufferColorMap0;
uniform usampler2DRect u_OITIndicesMap;

layout (std430) readonly buffer ssbo_OITBuffer {
	OITBufferReservedData OITReservedData;
	OITBufferNode OITNodes[];
};

out vec4 o_fragColor0;

vec4 blend(in vec4 src, in vec4 dst)
{
    return vec4(src.rgb * src.a + dst.rgb * (1.0f - src.a), src.a + dst.a * (1.0f - src.a));
}

void main(void)
{
	ivec2 texelCoord = ivec2(gl_FragCoord.xy);
	
	uvec4 PBRData = texelFetch(u_GBufferColorMap0, texelCoord);
	vec4 color = vec4(unpackF2x11_1x10(PBRData[0u]), unpackUnorm4x8(PBRData[2u])[3u]);
    
	uint OITIndex = texelFetch(u_OITIndicesMap, texelCoord).r;
	while (OITIndex != 0xFFFFFFFFu)
	{
		vec4 baseColor;
		baseColor.rgb = unpackF2x11_1x10(OITNodes[OITIndex].packedPBRData[0u]);
		baseColor.a = unpackUnorm4x8(OITNodes[OITIndex].packedPBRData[2u])[3u];
		
		color = blend(baseColor, color);
		OITIndex = OITNodes[OITIndex].next;
	}
	
	o_fragColor0 = color;
}
