layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

#ifdef POSITION_ATTRIBUTE
    layout (std430) buffer ssbo_layeredShadowMatricesBuffer { uint shadowNumLayers; uint padding[3]; mat4x4 shadowMatrices[]; };
#endif

#ifdef TEX_COORDS_ATTRIBUTE
    in vec2 v_texCoord[];
    out vec2 f_texCoord;
#endif

#ifdef COLOR_ATTRIBUTE
    in vec4 v_color[];
    out vec4 f_color;
#endif

void main()
{
    for(int layer = 0; layer < shadowNumLayers; ++layer)
    {
        gl_Layer = layer;
        for(int i = 0; i < 3; ++i)
        {
            #ifdef POSITION_ATTRIBUTE
                gl_Position = shadowMatrices[layer] * gl_in[i].gl_Position;
            #endif
            
            #ifdef TEX_COORDS_ATTRIBUTE
                f_texCoord = v_texCoord[i];
            #endif

            #ifdef COLOR_ATTRIBUTE
                f_color = v_color[i];
            #endif
            
            EmitVertex();
        }    
        EndPrimitive();
    }
}