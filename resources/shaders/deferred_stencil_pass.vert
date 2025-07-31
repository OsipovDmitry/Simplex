#ifdef POSITION_ATTRIBUTE
    layout (location = POSITION_ATTRIBUTE) in vec3 a_position;
    
    #if defined(POINT_LIGHT_TYPE) || defined(SPOT_LIGHT_TYPE)
        uniform mat4 u_modelViewProjectionMatrix;
    #endif
#endif

void main(void)
{
#ifdef POSITION_ATTRIBUTE
    vec4 pos = vec4(a_position, 1.0f);

    #if defined(POINT_LIGHT_TYPE) || defined(SPOT_LIGHT_TYPE)
        pos = u_modelViewProjectionMatrix * pos;
    #endif
    
    gl_Position = pos;
#endif
}
