#include<gammacorrection.glsl>

#ifdef TEX_COORDS_ATTRIBUTE
    in vec2 f_texCoord;
    
    #ifdef HAS_BASE_COLOR_MAP
        uniform sampler2D u_baseColorMap;
    #endif
    
    #ifdef HAS_EMISSION_MAP
        uniform sampler2D u_emissionMap;
    #endif
#endif

#ifdef COLOR_ATTRIBUTE
    in vec4 f_color;
#endif

#ifdef HAS_BASE_COLOR
    uniform vec4 u_baseColor;
#endif

#ifdef HAS_EMISSION
    uniform vec3 u_emission;
#endif

#ifdef SHADING_FILTER_VSM
    layout (location = 0) out vec4 o_fragColor0;
#endif

#ifdef SHADING_MODE_COLOR
    layout (location = 1) out vec4 o_fragColor1;
#endif

#ifdef HAS_ALPHA_CUTOFF
    uniform float u_alphaCutoff;
#endif

void main(void)
{
    vec4 baseColor = vec4(1.0f);

#ifdef COLOR_ATTRIBUTE
    baseColor *= f_color;
#endif

#ifdef HAS_BASE_COLOR
    baseColor *= u_baseColor;
#endif

#if defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_BASE_COLOR_MAP)
    baseColor *= toLinearRGB(texture(u_baseColorMap, f_texCoord));
#endif

#ifdef HAS_ALPHA_CUTOFF
    if (baseColor.a < u_alphaCutoff)
        discard;
#endif

    vec3 emission = vec3(0.0f);

    #ifdef HAS_EMISSION
        emission += u_emission;
    #endif

    #if defined(TEX_COORDS_ATTRIBUTE) && defined(HAS_EMISSION_MAP)
        emission += toLinearRGB(texture(u_emissionMapMap, v_texCoord)).rgb;
    #endif

    vec4 color = baseColor + vec4(emission, 0.0f);

#ifdef SHADING_FILTER_VSM
    float depth = gl_FragCoord.z;
    float dx = dFdx(depth);
    float dy = dFdy(depth);
    o_fragColor0 = vec4(depth, depth * depth + 0.25f * (dx * dx + dy * dy), 0.0f, 0.0f);
#endif
    
#ifdef SHADING_MODE_COLOR
    vec3 c1 = mix(vec3(1.0f), color.rgb, color.a);
    vec3 c2 = mix(color.rgb, vec3(0.0f), color.a);
    
    o_fragColor1 = vec4(mix(c1, c2, color.a), 1.0f);
#endif
}
