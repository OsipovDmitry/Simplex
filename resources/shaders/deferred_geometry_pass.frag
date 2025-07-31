#ifdef IS_TRANSPARENT
    layout (early_fragment_tests) in;
#endif

#include<gammacorrection.glsl>
#include<normal.glsl>
#include<oit_node.glsl>

#ifdef HAS_ALPHA_CUTOFF
    uniform float u_alphaCutoff;
#endif

uniform bool u_isLightedFlag;
uniform bool u_isShadowedFlag;

#ifdef TEX_COORDS_ATTRIBUTE
    in vec2 v_texCoord;
    
    #ifdef HAS_BASE_COLOR_MAP
        uniform sampler2D u_baseColorMap;
    #endif
    
    #ifdef HAS_EMISSION_MAP
        uniform sampler2D u_emissionMap;
    #endif

    #if defined(HAS_OCCLUSION_MAP) || defined(HAS_ROUGHNESS_MAP) || defined(HAS_METALNESS_MAP)
        uniform uvec3 u_ORMSwizzleMask;
    #endif
    
    #ifdef HAS_OCCLUSION_MAP
        uniform sampler2D u_occlusionMap;
    #endif

    #ifdef HAS_ROUGHNESS_MAP
        uniform sampler2D u_roughnessMap;
    #endif

    #ifdef HAS_METALNESS_MAP
        uniform sampler2D u_metalnessMap;
    #endif

    #if defined(HAS_NORMAL_MAP) && defined(NORMAL_ATTRIBUTE) && defined(TANGENT_ATTRIBUTE)
        uniform sampler2D u_normalMap;
    #endif
#endif

#ifdef NORMAL_ATTRIBUTE
    in vec3 v_normal;

    #ifdef TANGENT_ATTRIBUTE
        in vec3 v_tangent;
        in vec3 v_binormal;
    #endif
#endif

#ifdef COLOR_ATTRIBUTE
    in vec4 v_color;
#endif

#ifdef HAS_BASE_COLOR
    uniform vec4 u_baseColor;
#endif

#ifdef HAS_EMISSION
    uniform vec3 u_emission;
#endif

#ifdef HAS_ROUGHNESS
    uniform float u_roughness;
#endif

#ifdef HAS_METALNESS
    uniform float u_metalness;
#endif

layout (std430) buffer coherent ssbo_OITNodesBuffer { uint OITMaxNumNodes; uint OITNumNodes; OITNode OITNodes[]; };
layout (r32ui) uniform uimage2DRect u_OITIndicesImage;

void main(void)
{
    vec4 baseColor = vec4(1.0f);

    #ifdef COLOR_ATTRIBUTE
        baseColor *= v_color;
    #endif

    #ifdef HAS_BASE_COLOR
        baseColor *= u_baseColor;
    #endif

    #if defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_BASE_COLOR_MAP)
        baseColor *= toLinearRGB(texture(u_baseColorMap, v_texCoord));
    #endif
    
    float alpha = baseColor.a;
    
    #ifdef HAS_ALPHA_CUTOFF
        if (alpha < u_alphaCutoff)
            discard;
    #endif

    vec3 emission = vec3(0.0f);

    #if defined (HAS_EMISSION) || (defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_EMISSION_MAP))
        emission = vec3(1.0f);
    
        #if defined (HAS_EMISSION)
            emission *= u_emission;
        #endif

        #if defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_EMISSION_MAP)
            emission *= toLinearRGB(texture(u_emissionMap, v_texCoord)).rgb;
        #endif
    #endif
    
    vec3 finalColor = emission;
    if (!u_isLightedFlag)
    {
        finalColor += baseColor.rgb;
        baseColor.rgb = vec3(0.0f);
    }

    float occlusion = 1.0f;

    #if defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_OCCLUSION_MAP)
        occlusion *= texture(u_occlusionMap, v_texCoord)[u_ORMSwizzleMask[0]];
    #endif

    float roughness = 1.0f;

    #ifdef HAS_ROUGHNESS
        roughness *= u_roughness;
    #endif

    #if defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_ROUGHNESS_MAP)
        roughness *= texture(u_roughnessMap, v_texCoord)[u_ORMSwizzleMask[1]];
    #endif

    float metalness = 1.0f;

    #ifdef HAS_METALNESS
        metalness *= u_metalness;
    #endif

    #if defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_METALNESS_MAP)
        metalness *= texture(u_metalnessMap, v_texCoord)[u_ORMSwizzleMask[2]];
    #endif

    vec3 normal = vec3(0.0f);

    #ifdef NORMAL_ATTRIBUTE
        normal = normalize(v_normal);

        #if defined(HAS_NORMAL_MAP) && defined(TEX_COORDS_ATTRIBUTE) && defined(TANGENT_ATTRIBUTE)
            vec3 tangent = normalize(v_tangent);
            vec3 binormal = normalize(v_binormal);
            normal = normalize(mat3(tangent, binormal, normal) * unpackNormal(texture(u_normalMap, v_texCoord).xyz));
        #endif
    #endif

    #ifdef IS_TRANSPARENT
        // don't revert normal for transparent geometry
    #else
        if (!gl_FrontFacing)
            normal = -normal;
    #endif

    ivec2 texelCoord = ivec2(gl_FragCoord.xy);
    uint OITIndex = 0xFFFFFFFF;
    uint prevOITIndex = 0xFFFFFFFF;

    #ifdef IS_TRANSPARENT
        OITIndex = atomicAdd(OITNumNodes, 1u);
        prevOITIndex = imageAtomicExchange(u_OITIndicesImage, texelCoord, OITIndex);
    #else
        alpha = 1.0f;
        float prevDepth = 1.0f;

        OITIndex = imageLoad(u_OITIndicesImage, texelCoord).r;
        if (OITIndex != 0xFFFFFFFF)
            prevDepth = OITNodes[OITIndex].depth;

        if (gl_FragCoord.z > prevDepth)
            discard;
            
        if (OITIndex == 0xFFFFFFFF)
            OITIndex = atomicAdd(OITNumNodes, 1u);
            
        imageStore(u_OITIndicesImage, texelCoord, uvec4(OITIndex));
    #endif
    
    if (OITIndex < OITMaxNumNodes)
    {
        packWholeOITNode(OITNodes[OITIndex],
                         baseColor.rgb,
                         finalColor,
                         occlusion,
                         roughness,
                         metalness,
                         alpha,
                         normal,
                         bvec2(u_isLightedFlag, u_isShadowedFlag),
                         gl_FragCoord.z,
                         prevOITIndex);
    }
}
