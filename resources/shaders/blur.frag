uniform uvec2 u_viewportSize;

uniform sampler2DRect u_sourceMap;
uniform ivec2 u_blurDirection;
layout (std430) buffer ssbo_blurKernelBuffer { uint blurKernelRadius; uint padding[3u]; vec4 blurKernel[]; };
    
out vec4 o_fragColor;

void main(void)
{
    ivec2 sourceMapSizeMinusOne = textureSize(u_sourceMap) - ivec2(1);
    ivec2 sourceFragCoord = ivec2(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)) * vec2(sourceMapSizeMinusOne));
    
    vec4 color = blurKernel[0][0] * texelFetch(u_sourceMap, sourceFragCoord);
    for (int i = 1; i < blurKernelRadius; ++i)
    {
        float weight = blurKernel[i/4][i%4];
        
        ivec2 sampleFragCoord = clamp(sourceFragCoord + u_blurDirection * i, ivec2(0), sourceMapSizeMinusOne);
        color += weight * texelFetch(u_sourceMap, sampleFragCoord);
        
        sampleFragCoord = clamp(sourceFragCoord - u_blurDirection * i, ivec2(0), sourceMapSizeMinusOne);
        color += weight * texelFetch(u_sourceMap, sampleFragCoord);
    }

    o_fragColor = color;
}
