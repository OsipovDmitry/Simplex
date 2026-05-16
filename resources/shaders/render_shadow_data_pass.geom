layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[6];
} gl_in[];

flat in uint v_layerID[];
flat in uint v_meshID[];
flat in uint v_materialID[];
in float v_linearNormalizedDepth[];
in vec2 v_texCoords[];
in vec4 v_color[];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_ClipDistance[6];
};

flat out uint g_meshID;
flat out uint g_materialID;
out float g_linearNormalizedDepth;
out vec2 g_texCoords;
out vec4 g_color;

void main(void)
{
	gl_Layer = int(v_layerID[0u]);
	g_meshID = v_meshID[0u];
	g_materialID = v_materialID[0u];
	
	for (uint i = 0u; i < 3u; ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		for (uint j = 0u; j < 6u; ++j)
			gl_ClipDistance[j] = gl_in[i].gl_ClipDistance[j];
		g_linearNormalizedDepth = v_linearNormalizedDepth[i];
		g_texCoords = v_texCoords[i];
		g_color = v_color[i];
		EmitVertex();
	}
	
	EndPrimitive();
}
