#ifdef POSITION_ATTRIBUTE
    in vec3 v_texCoord;
#endif

#ifdef HAS_BACKGROUND_COLOR_MAP
    uniform samplerCube u_backgroundColorMap;
#endif

#ifdef HAS_BACKGROUND_COLOR
    uniform vec3 u_backgroundColor;
#endif

#ifdef HAS_BACKGROUND_ROUGHNESS
    uniform float u_backgroundRoughness;
#endif

out vec4 o_fragColor0;

void main(void)
{
    vec3 color = vec3(1.0f);
    
    #ifdef HAS_BACKGROUND_COLOR
        color *= u_backgroundColor;
    #endif
    
    #if defined(POSITION_ATTRIBUTE) && defined(HAS_BACKGROUND_COLOR_MAP)
        float roughness = 0.0f;
        
        #ifdef HAS_BACKGROUND_ROUGHNESS
            roughness = u_backgroundRoughness;
        #endif
        color *= textureLod(u_backgroundColorMap, v_texCoord, roughness).rgb;
    #endif
    
    o_fragColor0 = vec4(color, 1.0f);
}