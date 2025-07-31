#include<oit_node.glsl>

uniform uvec2 u_viewportSize;
uniform mat4 u_viewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_projectionMatrixInverse;

uniform sampler2DRect u_GBufferColor1Map;
uniform sampler2DRect u_GBufferDepthMap;
    
#if defined(SSAO_MODE_OPAQUE_AND_TRANSPARENT) || defined(SSAO_MODE_COLOR)
    layout (std430) buffer ssbo_OITNodesBuffer { uint OITMaxNumNodes; uint OITNumNodes; OITNode OITNodes[]; };
    layout (r32ui) uniform uimage2DRect u_OITIndicesImage;
#endif

uniform float u_shadowDepthBias;

layout (std430) buffer ssbo_SSAOKernelBuffer { uint numSSAOSamples; uint padding[3u]; vec4 SSAOsamples[]; };
uniform sampler2DRect u_SSAONoiseMap;
uniform float u_SSAORadius;
uniform float u_SSAOContribution;

out vec4 o_fragColor;

#if defined(SSAO_MODE_OPAQUE) || defined(SSAO_MODE_OPAQUE_AND_TRANSPARENT)
    #define SSAO_TYPE float
#elif defined(SSAO_MODE_COLOR)
    #define SSAO_TYPE vec3
#endif

vec3 restorePositionViewSpace(in float depth)
{
    vec4 pixelPos = u_projectionMatrixInverse * vec4(unpackNormal(vec3(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)), depth)), 1.0f);
    return pixelPos.xyz / pixelPos.w;
}

SSAO_TYPE calculateOcclusion(in vec3 pixelPos, in vec3 samplePos)
{
    vec4 sampleClipSpace = u_projectionMatrix * vec4(samplePos, 1.0f);
    sampleClipSpace /= sampleClipSpace.w;
    sampleClipSpace.xyz = packNormal(sampleClipSpace.xyz);
    
    SSAO_TYPE result = SSAO_TYPE(0.0f);
    if (any(lessThan(sampleClipSpace.xy, vec2(0.0f))) || any(greaterThan(sampleClipSpace.xy, vec2(1.0f))))
        return result;

    ivec2 occluderFragCoord = ivec2(sampleClipSpace.xy * (textureSize(u_GBufferDepthMap) - ivec2(1)));
    float occluderDepth = texelFetch(u_GBufferDepthMap, occluderFragCoord).r;
    vec4 occluderViewSpace = u_projectionMatrixInverse * vec4(unpackNormal(vec3(sampleClipSpace.xy, occluderDepth)), 1.0f);
    occluderViewSpace /= occluderViewSpace.w;

    float rangeCheck = 1.0f - step(u_SSAORadius, occluderViewSpace.z - pixelPos.z);
    result += SSAO_TYPE(step(samplePos.z + u_shadowDepthBias, occluderViewSpace.z) * rangeCheck);

#if defined(SSAO_MODE_OPAQUE_AND_TRANSPARENT) || defined(SSAO_MODE_COLOR)
    occluderFragCoord = ivec2(sampleClipSpace.xy * (imageSize(u_OITIndicesImage) - ivec2(1)));
    uint OITIndex = imageLoad(u_OITIndicesImage, occluderFragCoord).r;
    while (OITIndex != 0xFFFFFFFF)
    {
        vec4 occluderBaseColor;
        unpackOITNode(OITNodes[OITIndex], occluderBaseColor, occluderDepth, OITIndex);
        
        #if defined(SSAO_MODE_OPAQUE_AND_TRANSPARENT)
            SSAO_TYPE ssaoColor = SSAO_TYPE(1.0f);
        #elif defined(SSAO_MODE_COLOR)
            SSAO_TYPE ssaoColor = SSAO_TYPE(1.0f) - occluderBaseColor.rgb;
        #endif
        
        occluderViewSpace = u_projectionMatrixInverse * vec4(unpackNormal(vec3(sampleClipSpace.xy, occluderDepth)), 1.0f);
        occluderViewSpace /= occluderViewSpace.w;
        
        rangeCheck = 1.0f - step(u_SSAORadius, occluderViewSpace.z - pixelPos.z);
        result += step(samplePos.z + u_shadowDepthBias, occluderViewSpace.z) * rangeCheck * occluderBaseColor.a * ssaoColor;
    }
#endif
    
    return clamp(result, SSAO_TYPE(0.0f), SSAO_TYPE(1.0f));
}

void main(void)
{
    ivec2 GBufferFragCoord = ivec2(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)) * vec2(textureSize(u_GBufferDepthMap) - ivec2(1)));
    vec4 GBufferColor1 = texelFetch(u_GBufferColor1Map, GBufferFragCoord);
    float GBufferDepth = texelFetch(u_GBufferDepthMap, GBufferFragCoord).r;
    
    SSAO_TYPE ssao = SSAO_TYPE(0.0f);
    if (GBufferDepth != 1.0f)
    {
        vec3 pixelPos = restorePositionViewSpace(GBufferDepth);
        vec3 normal = normalize(vec3(u_viewMatrix * vec4(normalize(unpackNormal(GBufferColor1.rgb)), 0.0f)));
        
        ivec2 noiseMapSize = textureSize(u_SSAONoiseMap);
        ivec2 noiseTexCoord = ivec2(int(gl_FragCoord.x) % noiseMapSize.x, int(gl_FragCoord.y) % noiseMapSize.y);
        vec3 noiseVec = normalize(vec3(texelFetch(u_SSAONoiseMap, noiseTexCoord).rg, 0.0f));
        
        vec3 tangent = normalize(noiseVec - normal * dot(noiseVec, normal));
        mat3 TBN = mat3(tangent, cross(normal, tangent), normal);
        
        for (uint i = 0; i < numSSAOSamples; ++i)
        {
            vec3 samplePos = pixelPos + TBN * SSAOsamples[i].xyz * u_SSAORadius;
            ssao += calculateOcclusion(pixelPos, samplePos);
        }
        
        ssao /= float(numSSAOSamples);
    }
    
    o_fragColor = vec4(vec3(1.0f - ssao * u_SSAOContribution), 1.0f);
}
