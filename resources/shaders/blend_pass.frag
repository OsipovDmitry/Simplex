#extension GL_ARB_bindless_texture : enable

#include<math/transform.glsl>

#include<camera.glsl>
#include<cluster_node.glsl>
#include<geometry.glsl>
#include<ibl.glsl>
#include<light.glsl>
#include<light_node.glsl>
#include<material_map.glsl>
#include<packing.glsl>
#include<pbr.glsl>
#include<scene_info.glsl>

out vec4 o_fragColor0;

// TODO: move this constant to some SSBO
const float DielecticSpecular = 0.04f;

vec4 blend(in vec4 src, in vec4 dst)
{
    return vec4(src.rgb * src.a + dst.rgb * (1.0f - src.a), src.a + dst.a * (1.0f - src.a));
}

vec4 proccessLighting(in vec3 NDC_ZO, in uvec4 PBRData)
{
	const vec3 texelPosWS = projectPoint(cameraViewProjectionMatrixInverted(), 2.0f * NDC_ZO - vec3(1.0f));

	vec3 baseColor;
	vec3 emission;
	float occlusion;
	float roughness;
	float metalness;
	float alpha;
	vec3 normal;
	bool isLighted;
	bool isShadowed;
	unpackPBRData(PBRData, baseColor, emission, occlusion, roughness, metalness, alpha, normal, isLighted, isShadowed);
	
	const uint firstLightNodeIDs[2u] = uint[2u](
		sceneInfoFirstGlobalLightNodeID(),
		clusterNodeFirstLightNodeID(cameraClusterNodeID(NDC_ZO)));
	
	vec3 color = emission;
	
	if (isLighted)
	{
		const vec3 F0 = mix(vec3(DielecticSpecular), baseColor, metalness);
		
		for (uint i = 0u; i < 2u; ++i)
		{
			uint lightNodeID = firstLightNodeIDs[i];
			while (lightNodeID != 0xFFFFFFFFu)
			{
				const uint lightID = lightNodeLightID(lightNodeID);
				const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
				const uint typeID = lightTypeID(lightID);
				
				const vec3 N = normalize(transformVector(lightTransformInverted, normal));
				const vec3 V = normalize(transformVector(lightTransformInverted, cameraViewPosition() - texelPosWS));
				
				float attenuation = 1.0f;
				vec3 shadow = vec3(1.0f);
				vec3 light = vec3(0.0f);
				
				if (typeID == ImageBasedLightTypeID)
				{
					const uint BRDFLutMapID = imageBasedLightBRDFLutMapID(lightID);
					const uint diffuseMapID = imageBasedLightDiffuseMapID(lightID);
					const uint specularMapID = imageBasedLightSpecularMapID(lightID);
					if ((BRDFLutMapID != 0xFFFFFFFFu) && (diffuseMapID != 0xFFFFFFFFu) && (specularMapID != 0xFFFFFFFFu))
					{
						const TextureHandle BRDFLutMapTextureHandle = materialMapTextureHandle(BRDFLutMapID);
						const TextureHandle diffuseMapTextureHandle = materialMapTextureHandle(diffuseMapID);
						const TextureHandle specularMapTextureHandle = materialMapTextureHandle(specularMapID);
						if (!isTextureHandleEmpty(BRDFLutMapTextureHandle) &&
							!isTextureHandleEmpty(diffuseMapTextureHandle) &&
							!isTextureHandleEmpty(specularMapTextureHandle))
						{
							attenuation = imageBasedLightContribution(lightID);
							light = calculateIBL(
								sampler2D(BRDFLutMapTextureHandle),
								samplerCube(diffuseMapTextureHandle),
								samplerCube(specularMapTextureHandle),
								baseColor,
								metalness,
								roughness,
								F0,
								N,
								V);
						}
					}
				}
				else
				{
					vec3 L = vec3(0.0f);
					vec3 lightColor = vec3(0.0f);
					
					if (typeID == DirectionalLightTypeID)
					{
						L = vec3(0.0f, 0.0f, 1.0f);
						lightColor = directionalLightColor(lightID);
					}
					else
					{
						L = -transformPoint(lightTransformInverted, texelPosWS);
						
						const float LLen = length(L);
						if (LLen > EPS)
							L /= LLen;
						
						if (typeID == PointLightTypeID)
						{
							lightColor = pointLightColor(lightID);
							
							const vec2 radiuses = pointLightRadiuses(lightID); 
							attenuation = 1.0f - smoothstep(radiuses[0u], radiuses[1u], LLen);
								#if defined(SPOT_LIGHT_TYPE)
									float cosAngle = dot(L, normalize(u_modelZDirection));
									attenuation *= 1.0f - smoothstep(u_lightCosHalfAngles.x, u_lightCosHalfAngles.y, cosAngle);
								#endif
						}
						else if (typeID == SpotLightTypeID)
						{
							lightColor = spotLightColor(lightID);
							
							const vec2 radiuses = spotLightRadiuses(lightID);
							
							const vec2 cosHalfAngles = cos(spotLightHalfAngles(lightID));
							const float cosAngle = L.z; //dot(L, vec3(0.0f, 0.0f, 1.0f));
							
							attenuation =
								(1.0f - smoothstep(radiuses[0u], radiuses[1u], LLen)) *
								(1.0f - smoothstep(cosHalfAngles[0u], cosHalfAngles[1u], cosAngle));
						}
						
					}
					
					light = calculatePBR(
						baseColor,
						metalness,
						roughness,
						F0,
						lightColor,
						N,
						L,
						V);
				}
				
				color += attenuation * light * shadow;
				lightNodeID = lightNodeNextID(lightNodeID);
			}
		}
	}
	else
	{
		color += baseColor;
	}
	
	return vec4(color, alpha);
}

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);
	const vec2 NDC_XY_ZO = vec2(gl_FragCoord.xy) / vec2(geometryBufferSize() - uvec2(1u));
	
	uvec4 PBRData;
	float depth;
	uint OITNodeID;
	geometryBufferData(fragCoords, PBRData, depth, OITNodeID);
	
	vec4 color = proccessLighting(vec3(NDC_XY_ZO, depth), PBRData);
	while (OITNodeID != 0xFFFFFFFFu)
	{
		geometryBufferData(OITNodeID, PBRData, depth, OITNodeID);
		color = blend(proccessLighting(vec3(NDC_XY_ZO, depth), PBRData), color);
	}
	
	o_fragColor0 = color;
}
