#include<descriptions.glsl>

uniform mat4x4 u_viewProjectionMatrix;

layout (std430) readonly buffer ssbo_positionsBuffer { vec3 positions[]; };
layout (std430) readonly buffer ssbo_normalsBuffer { vec3 normals[]; };
layout (std430) readonly buffer ssbo_texCoordsBuffer { vec2 texCoords[]; };
layout (std430) readonly buffer ssbo_bonesBuffer { BonesDescription bones[]; };
layout (std430) readonly buffer ssbo_tangentsBuffer { vec4 tangents[]; };
layout (std430) readonly buffer ssbo_colorsBuffer { vec4 colors[]; };
layout (std430) readonly buffer ssbo_indicesBuffer { uint indices[]; };
layout (std430) readonly buffer ssbo_meshesBuffer { MeshDescription meshes[]; };
layout (std430) readonly buffer ssbo_drawablesBuffer { DrawableDescription drawables[]; };
layout (std430) readonly buffer ssbo_drawDataBuffer {
	DrawDataBufferReservedData drawDataBufferReservedData;
	DrawDataDescription drawData[];
};

flat out uint v_meshOffset;
flat out uint v_materialOffset;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_binormal;
out vec2 v_texCoords;
out vec4 v_color;

void main(void)
{
	if (gl_DrawID < drawDataBufferReservedData.count)
	{
		DrawDataDescription drawDataDescription = drawData[gl_DrawID];
		
		DrawableDescription drawableDescription = drawables[drawDataDrawableOffset(drawDataDescription)];
		
		v_meshOffset = drawableDescription.meshOffset;
		v_materialOffset = drawableDescription.materialOffset;
		
		MeshDescription meshDescription = meshes[v_meshOffset];
		
		uint indexOffset = meshDescription.indexOffset + gl_VertexID;
		uint indexID = indices[indexOffset];
		
		gl_Position = vec4(vec3(0.0f), 1.0f);
		if (meshDescription.positionOffset != 0xFFFFFFFFu)
		{
			gl_Position =
				u_viewProjectionMatrix *
				drawDataModelMatrix(drawDataDescription) *
				vec4(positions[meshDescription.positionOffset + indexID], 1.0f);
		}
		
		if (meshDescription.normalOffset != 0xFFFFFFFFu)
		{
			mat3x3 normalMatrix = drawDataNormalMatrix(drawDataDescription);
			
			v_normal = normalize(normals[meshDescription.normalOffset + indexID]);
			v_normal = normalize(normalMatrix * v_normal);
			
			if (meshDescription.tangentOffset != 0xFFFFFFFFu)
			{
				vec4 tangent = tangents[meshDescription.tangentOffset + indexID];
				v_tangent = normalize(tangent.xyz);
				v_tangent = normalize(normalMatrix * v_tangent);

				v_binormal = normalize(cross(v_normal, v_tangent) * tangent.w);
			}
		}
		
		if (meshDescription.texCoordsOffset != 0xFFFFFFFFu)
		{
			v_texCoords = texCoords[meshDescription.texCoordsOffset + indexID];
		}
		
		if (meshDescription.colorOffset != 0xFFFFFFFFu)
		{
			v_color = colors[meshDescription.colorOffset + indexID];
		}
	}
}
