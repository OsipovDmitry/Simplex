#extension GL_ARB_bindless_texture : enable

#include<descriptions.glsl>

layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };
layout (std430) readonly buffer ssbo_materialMapsBuffer { uvec2 materialMaps[]; };
layout (std430) readonly buffer ssbo_materialsBuffer { MaterialDescription materials[]; };

flat in uint v_meshOffset;
flat in uint v_materialOffset;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_binormal;
in vec2 v_texCoords;
in vec4 v_color;

out vec4 o_fragColor0;
out vec4 o_fragColor1;
out vec4 o_fragColor2;

void main(void)
{
	o_fragColor0 = vec4(v_normal, 1.0f);
	o_fragColor1 = vec4(v_tangent, 1.0f);
	o_fragColor2 = vec4(v_color);
}
