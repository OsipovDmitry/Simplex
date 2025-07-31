#include<normal.glsl>
#include<pbr.glsl>
#include<ibl.glsl>
#include<oit_node.glsl>

uniform uvec2 u_viewportSize;
uniform mat4 u_viewProjectionMatrixInverse;
uniform vec3 u_viewPosition;

layout (std430) buffer ssbo_OITNodesBuffer { uint OITMaxNumNodes; uint OITNumNodes; OITNode OITNodes[]; };
layout (r32ui) uniform uimage2DRect u_OITIndicesImage;

#if defined(POINT_LIGHT_TYPE) || defined(SPOT_LIGHT_TYPE) || defined(DIRECTIONAL_LIGHT_TYPE)
    #define DIRECT_LIGHTING
#elif defined(IBL_TYPE)
    #define INDIRECT_LIGHTING
#elif
    error!!!
#endif

#if defined(SHADING_MODE_OPAQUE) || defined(SHADING_MODE_OPAQUE_AND_TRANSPARENT) || defined(SHADING_MODE_COLOR)
    #define DIRECT_SHADING
    #if defined(SPOT_LIGHT_TYPE) || defined(DIRECTIONAL_LIGHT_TYPE)
        #define SHADING_MAP_2D
    #elif defined(POINT_LIGHT_TYPE)
        #define SHADING_MAP_CUBE
    #endif
#endif

uniform float u_shadowDepthBias;
uniform float u_lightDielectricSpecular;

#if defined(DIRECT_LIGHTING)
    uniform vec3 u_lightColor;
    #if defined(POINT_LIGHT_TYPE) || defined(SPOT_LIGHT_TYPE)
        uniform vec2 u_lightRadiuses;
        uniform vec3 u_modelPosition;
    #endif
    #if defined(SPOT_LIGHT_TYPE) || defined(DIRECTIONAL_LIGHT_TYPE)
        uniform vec3 u_modelZDirection;
    #endif
    #if defined(SPOT_LIGHT_TYPE)
        uniform vec2 u_lightCosHalfAngles;
    #endif
#elif defined(INDIRECT_LIGHTING)
    uniform sampler2D u_IBLBRDFLutMap;
    uniform samplerCube u_IBLDiffuseMap;
    uniform samplerCube u_IBLSpecularMap;
    uniform float u_IBLContribution;
#endif

#if defined(DIRECT_SHADING)
    uniform mat4 u_shadowViewMatrix;
    uniform mat4 u_shadowProjectionMatrix;
    
    #if defined(SPOT_LIGHT_TYPE) || defined(DIRECTIONAL_LIGHT_TYPE)
        uniform sampler2D u_shadowDepthMap;

        #if defined(SHADING_FILTER_POINT)
        #elif defined(SHADING_FILTER_PCF)
            const uint PCF_NUM_SAMPLES = 9;
            const ivec2 PCF_SAMPLES[PCF_NUM_SAMPLES] = ivec2[](
                ivec2(-1,+1), ivec2(+0,+1), ivec2(+1,+1),
                ivec2(-1,+0), ivec2(+0,+0), ivec2(+1,+0),
                ivec2(-1,-1), ivec2(+0,-1), ivec2(+1,-1));
        #elif defined(SHADING_FILTER_VSM)
        #endif

        #if defined(SHADING_MODE_COLOR)
            uniform sampler2D u_shadowColorMap;
        #endif
    #elif defined(POINT_LIGHT_TYPE)
        uniform samplerCube u_shadowDepthMap;

        #if defined(SHADING_FILTER_POINT)
        #elif defined(SHADING_FILTER_PCF)
            const uint PCF_NUM_SAMPLES = 20;
            const vec3 PCF_SAMPLES[PCF_NUM_SAMPLES] = vec3[](
                vec3(+1,+1,+1), vec3(+1,-1,+1), vec3(-1,-1,+1), vec3(-1,+1,+1), 
                vec3(+1,+1,-1), vec3(+1,-1,-1), vec3(-1,-1,-1), vec3(-1,+1,-1),
                vec3(+1,+1,+0), vec3(+1,-1,+0), vec3(-1,-1,+0), vec3(-1,+1,+0),
                vec3(+1,+0,+1), vec3(-1,+0,+1), vec3(+1,+0,-1), vec3(-1,+0,-1),
                vec3(+0,+1,+1), vec3(+0,-1,+1), vec3(+0,-1,-1), vec3(+0,+1,-1));
        #elif defined(SHADING_FILTER_VSM)
        #endif

        #if defined(SHADING_MODE_COLOR)
            uniform samplerCube u_shadowColorMap;
        #endif
    #endif
#endif

float linearStep(in float low, in float high, in float v)
{
    return clamp((v - low)/(high - low), 0.0f, 1.0f);
}

vec4 blend(in vec4 src, in vec4 dst)
{
    return vec4(src.rgb * src.a + dst.rgb * (1.0f - src.a), src.a + dst.a * (1.0f - src.a));
}

vec3 restorePosition(in float depth)
{
    vec4 pixelPos = u_viewProjectionMatrixInverse * vec4(2.0f * vec3(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)), depth) - vec3(1.0f), 1.0f);
    return pixelPos.xyz / pixelPos.w;
}

//SSAO_TYPE restoreSSAO()
//{
//    #if defined(SSAO_MODE_DISABLED)
//        return SSAO_TYPE(1.0f);
//    #else
//        vec4 ssao = texture(u_SSAOMap, ivec2(gl_FragCoord.xy / vec2(u_viewportSize - uvec2(1u)) * vec2(textureSize(u_SSAOMap) - ivec2(1))));
//        #if defined(SSAO_MODE_OPAQUE) || defined(SSAO_MODE_OPAQUE_AND_TRANSPARENT)
//            return ssao.r;
//        #elif defined(SSAO_MODE_COLOR)
//            return ssao.rgb;
//        #endif
//    #endif
//}

vec3 calculateLighting(in vec3 baseColor,
                       in float occlusion,
                       in float roughness,
                       in float metalness,
                       in vec3 pos,
                       in vec3 normal)
{
    vec3 V = normalize(u_viewPosition - pos);
    vec3 F0 = mix(vec3(u_lightDielectricSpecular), baseColor, metalness);
    
    vec3 result = vec3(0.0f);
#if defined(DIRECT_LIGHTING)
    vec3 L;
    float attenuation = 1.0f;
    
    #if defined(POINT_LIGHT_TYPE) || defined(SPOT_LIGHT_TYPE)
        L = u_modelPosition - pos;
        float dist = length(L);
        L /= dist;
        attenuation *= 1.0f - smoothstep(u_lightRadiuses.x, u_lightRadiuses.y, dist);
        #if defined(SPOT_LIGHT_TYPE)
            float cosAngle = dot(L, normalize(u_modelZDirection));
            attenuation *= 1.0f - smoothstep(u_lightCosHalfAngles.x, u_lightCosHalfAngles.y, cosAngle);
        #endif
    #elif defined(DIRECTIONAL_LIGHT_TYPE)
        L = normalize(u_modelZDirection);
    #endif
    
    result = calculatePBR(baseColor,
                          metalness,
                          roughness,
                          F0,
                          u_lightColor,
                          normal,
                          L,
                          V) * attenuation;
#elif defined(INDIRECT_LIGHTING)
    result = calculateIBL(u_IBLBRDFLutMap,
                          u_IBLDiffuseMap,
                          u_IBLSpecularMap,
                          baseColor,
                          metalness,
                          roughness,
                          F0,
                          normal,
                          V) * u_IBLContribution;
#endif

    result *= occlusion;
    return result;
}

vec3 calculateShading(in vec3 pos, in bool isTransparent)
{
    vec3 result = vec3(1.0f);

#if defined(DIRECT_SHADING)
    #if defined(SHADING_MAP_2D)
        vec4 shadowCoord = u_shadowViewMatrix * vec4(pos, 1.0f);
        shadowCoord.z += u_shadowDepthBias;
        shadowCoord = u_shadowProjectionMatrix * shadowCoord;
        shadowCoord /= shadowCoord.w;
        
        if (all(greaterThanEqual(shadowCoord.xyz, vec3(0.0f))) && all(lessThanEqual(shadowCoord.xyz, vec3(1.0f))))
        {
            vec3 shadowColor = vec3(1.0f);
            #if defined(SHADING_MODE_COLOR)
                if (!isTransparent)
                    shadowColor = texture(u_shadowColorMap, shadowCoord.xy).rgb;
            #endif
            
            #if defined(SHADING_FILTER_POINT)
                result = step(shadowCoord.z, texture(u_shadowDepthMap, shadowCoord.xy).r) * shadowColor;
            #elif defined(SHADING_FILTER_PCF)
                ivec2 shadowFragCoord = ivec2(shadowCoord.xy * (textureSize(u_shadowDepthMap, 0) - ivec2(1)));
                vec3 shadow = vec3(0.0f);
                for (uint i = 0; i < PCF_NUM_SAMPLES; ++i)
                    shadow += step(shadowCoord.z, texelFetch(u_shadowDepthMap, shadowFragCoord + PCF_SAMPLES[i], 0).r);
                result = (shadow / PCF_NUM_SAMPLES) * shadowColor;
            #elif defined(SHADING_FILTER_VSM)
                vec2 moments = texture(u_shadowDepthMap, shadowCoord.xy).xy;
                float p = step(shadowCoord.z, moments.x);
                float variance = max(moments.y - moments.x * moments.x, 0.00002f);
                float d = shadowCoord.z - moments.x;
                float pMax = linearStep(0.2f, 1.0f, variance / (variance + d*d));
                result = min(max(p, pMax), 1.0f) * shadowColor;
            #endif
        }
    #elif defined(SHADING_MAP_CUBE)
        vec4 shadowCoord = u_shadowViewMatrix * vec4(pos, 1.0f);
        float shadowMaxCoord = -max(abs(shadowCoord.x), max(abs(shadowCoord.y), abs(shadowCoord.z)));
        shadowMaxCoord += u_shadowDepthBias;
        vec4 shadowProjCoord = u_shadowProjectionMatrix * vec4(vec3(0.0f, 0.0f, shadowMaxCoord), 1.0f);
        shadowProjCoord /= shadowProjCoord.w;
        
        vec3 shadowColor = vec3(1.0f);
        #if defined(SHADING_MODE_COLOR)
            if (!isTransparent)
                shadowColor = texture(u_shadowColorMap, shadowCoord.xyz).rgb;
        #endif
        
        #if defined(SHADING_FILTER_POINT)
            result = step(shadowProjCoord.z, texture(u_shadowDepthMap, shadowCoord.xyz).r) * shadowColor;
        #elif defined(SHADING_FILTER_PCF)
            float sampleScale = 16.0f / float(textureSize(u_shadowDepthMap, 0).x);
            vec3 shadow = vec3(0.0f);
            for (uint i = 0; i < PCF_NUM_SAMPLES; ++i)
                shadow += step(shadowProjCoord.z, texture(u_shadowDepthMap, shadowCoord.xyz + PCF_SAMPLES[i] * sampleScale).r) * shadowColor;
            result = shadow / PCF_NUM_SAMPLES;
        #elif defined(SHADING_FILTER_VSM)
            vec2 moments = texture(u_shadowDepthMap, shadowCoord.xyz).xy;
            float p = step(shadowProjCoord.z, moments.x);
            float variance = max(moments.y - moments.x * moments.x, 0.000002f);
            float d = shadowProjCoord.z - moments.x;
            float pMax = linearStep(0.2f, 1.0f, variance / (variance + d*d));
            result = min(max(p, pMax), 1.0f) * shadowColor;
        #endif
    #endif
#endif

    return result;
}

void main(void)
{
    ivec2 texelCoord = ivec2(gl_FragCoord.xy);
    uint OITIndex = imageLoad(u_OITIndicesImage, texelCoord).r;
    while (OITIndex != 0xFFFFFFFF)
    {
        vec3 baseColor;
        vec3 finalColor;
        float occlusion;
        float roughness;
        float metalness;
        float alpha;
        vec3 normal;
        bvec2 flags;
        float depth;
        uint next;

        unpackWholeOITNode(OITNodes[OITIndex],
                           baseColor,
                           finalColor,
                           occlusion,
                           roughness,
                           metalness,
                           alpha,
                           normal,
                           flags,
                           depth,
                           next);

        vec3 pixelPos = restorePosition(depth);
        
        vec3 lighting = vec3(0.0f);
        if (flags[0])
            lighting = calculateLighting(baseColor, occlusion, roughness, metalness, pixelPos, normal);
        
        vec3 shading = vec3(1.0f);
        if (flags[1])
            shading = calculateShading(pixelPos, alpha < 1.0f - EPS);

        packOITNodeFinalColor(OITNodes[OITIndex], finalColor + lighting * shading);
        OITIndex = next;
    }
    
}
