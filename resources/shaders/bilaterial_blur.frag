#include<oit_node.glsl>

uniform uvec2 u_viewportSize;
uniform mat4 u_projectionMatrixInverse;

uniform sampler2DRect u_sourceMap;
uniform sampler2DRect u_sourceDepthMap;

uniform ivec2 u_blurDirection;
uniform float u_blurMaxDepthDifference;

layout (std430) buffer ssbo_blurKernelBuffer { uint blurKernelRadius; uint padding[3u]; vec4 blurKernel[]; };
    
out vec4 o_fragColor;

vec3 restorePositionViewSpace(in float depth)
{
    vec4 pixelPos = u_projectionMatrixInverse * vec4(unpackNormal(vec3(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)), depth)), 1.0f);
    return pixelPos.xyz / pixelPos.w;
}

void main(void)
{
    ivec2 sourceMapSizeMinusOne = textureSize(u_sourceMap) - ivec2(1);
    ivec2 sourceFragCoord = ivec2(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)) * vec2(sourceMapSizeMinusOne));
    vec4 color = blurKernel[0][0] * texelFetch(u_sourceMap, sourceFragCoord);
    
    ivec2 sourceDepthMapSizeMinusOne = textureSize(u_sourceDepthMap) - ivec2(1);
    ivec2 sourceDepthFragCoord = ivec2(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)) * vec2(sourceDepthMapSizeMinusOne));
    float depth = restorePositionViewSpace(texelFetch(u_sourceDepthMap, sourceDepthFragCoord).r).z;
    
    for (int i = 1; i < blurKernelRadius; ++i)
    {
        float weight = blurKernel[i/4][i%4];
        
        ivec2 sampleDepthFragCoord = clamp(sourceDepthFragCoord + u_blurDirection * i, ivec2(0), sourceDepthMapSizeMinusOne);
        float sampleDepth = restorePositionViewSpace(texelFetch(u_sourceDepthMap, sampleDepthFragCoord).r).z;
        if (abs(sampleDepth - depth) < u_blurMaxDepthDifference)
        {
            ivec2 sampleFragCoord = clamp(sourceFragCoord + u_blurDirection * i, ivec2(0), sourceMapSizeMinusOne);
            color += weight * texelFetch(u_sourceMap, sampleFragCoord);
        }
        
        sampleDepthFragCoord = clamp(sourceDepthFragCoord - u_blurDirection * i, ivec2(0), sourceDepthMapSizeMinusOne);
        sampleDepth = restorePositionViewSpace(texelFetch(u_sourceDepthMap, sampleDepthFragCoord).r).z;
        if (abs(sampleDepth - depth) < u_blurMaxDepthDifference)
        {
            ivec2 sampleFragCoord = clamp(sourceFragCoord - u_blurDirection * i, ivec2(0), sourceMapSizeMinusOne);
            color += weight * texelFetch(u_sourceMap, sampleFragCoord);
        }
    }

    o_fragColor = color;
}
