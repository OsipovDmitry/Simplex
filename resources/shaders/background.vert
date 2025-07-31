#ifdef POSITION_ATTRIBUTE
    layout (location = POSITION_ATTRIBUTE) in vec2 a_position;
    
    uniform mat4 u_viewProjectionMatrixInverse;
    uniform vec3 u_viewPosition;
    
    out vec3 v_texCoord;
#endif

void main(void)
{
#ifdef POSITION_ATTRIBUTE
    gl_Position = vec4(a_position, 0.0f, 1.0f);

    vec4 texCoord = u_viewProjectionMatrixInverse * vec4(a_position, 1.0f, 1.0f);
    v_texCoord = texCoord.xyz / texCoord.w;
    v_texCoord -= u_viewPosition;
#endif
}
