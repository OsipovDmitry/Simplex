#ifdef POSITION_ATTRIBUTE
    layout (location = POSITION_ATTRIBUTE) in vec2 a_position;
#endif

void main(void)
{
#ifdef POSITION_ATTRIBUTE
    gl_Position = vec4(a_position, 0.0f, 1.0f);
#endif
}
