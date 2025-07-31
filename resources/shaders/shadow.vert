#ifdef POSITION_ATTRIBUTE
    layout (location = POSITION_ATTRIBUTE) in vec3 a_position;
    uniform mat4 u_modelMatrix;

    #if defined(BONES_IDS_ATTRIBUTE) && defined(BONES_WEIGHTS_ATTRIBUTE)
        layout (location = BONES_IDS_ATTRIBUTE) in uvec4 a_bonesIDs;
        layout (location = BONES_WEIGHTS_ATTRIBUTE) in vec4 a_bonesWeights;
        layout (std430) buffer ssbo_bonesBuffer { mat3x4 bones[]; };
    #endif
#endif

#ifdef TEX_COORDS_ATTRIBUTE
    layout (location = TEX_COORDS_ATTRIBUTE) in vec2 a_texCoord;
    out vec2 v_texCoord;
#endif
#ifdef COLOR_ATTRIBUTE
    layout (location = COLOR_ATTRIBUTE) in vec4 a_color;
    out vec4 v_color;
#endif

void main(void)
{
#ifdef POSITION_ATTRIBUTE
    vec4 localPosition = vec4(a_position, 1.0f);

    #if defined(BONES_IDS_ATTRIBUTE) && defined(BONES_WEIGHTS_ATTRIBUTE)
        mat3x4 bonesMatrix =
            bones[a_bonesIDs[0]] * a_bonesWeights[0] +
            bones[a_bonesIDs[1]] * a_bonesWeights[1] +
            bones[a_bonesIDs[2]] * a_bonesWeights[2] +
            bones[a_bonesIDs[3]] * a_bonesWeights[3];
        localPosition = vec4(localPosition * bonesMatrix, 1.0f);
    #endif

    gl_Position = u_modelMatrix * localPosition;
#endif

#ifdef TEX_COORDS_ATTRIBUTE
    v_texCoord = a_texCoord;
#endif

#ifdef COLOR_ATTRIBUTE
    v_color = a_color;
#endif
}
