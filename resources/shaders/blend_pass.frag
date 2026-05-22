#extension GL_ARB_bindless_texture : enable

#include<camera.glsl>
#include<cluster_node.glsl>
#include<counters.glsl>
#include<geometry.glsl>
#include<ibl.glsl>
#include<light.glsl>
#include<light_node.glsl>
#include<map.glsl>
#include<packing.glsl>
#include<pbr.glsl>
#include<render_info.glsl>
#include<shadow.glsl>
#include<shadow_maps.glsl>
#include<shadow_transforms_data.glsl>

#include<math/constants.glsl>
#include<math/range.glsl>
#include<math/transform.glsl>

out vec4 o_fragColor0;

vec4 blend(in vec4 src, in vec4 dst)
{
    return vec4(src.rgb * src.a + dst.rgb * (1.0f - src.a), src.a + dst.a * (1.0f - src.a));
}

vec3 proccessShadow(
	in vec3 NDC_ZO,
	in float linearNormalizedDepth,
	in uvec3 mapCoords,
	in uint mapSize,
	in float alpha)
{
	const vec2 texelPosSMS = (NDC_ZO.xy * float(mapSize - 1u) + vec2(0.5f)) + vec2(mapCoords.xy);
	const vec3 texCoords = vec3(texelPosSMS.xy / float(renderInfoShadowAtlasSize()), float(mapCoords[2u]));
	
	const TextureHandle varianceTextureHandle = shadowMapsShadowVarianceTextureHandle();
	const vec2 moments = texture(sampler2DArray(varianceTextureHandle), texCoords).rg;
	
	const float variance = max(moments.y - moments.x * moments.x, EPS);
	const float d = linearNormalizedDepth - moments.x;
	const float p_max = smoothstep(renderInfoShadowLightBleedingAmount(), 1.0f, variance / (variance + d * d));
	
	const float p = step(linearNormalizedDepth, moments.x);
	vec3 result = vec3(max(p, p_max));
	
	if (alpha > 1.0f - EPS)
	{
		const TextureHandle colorTextureHandle = shadowMapsShadowColorTextureHandle();
		result *= texture(sampler2DArray(colorTextureHandle), texCoords).rgb;
	}
	
	return result;
}

vec3 proccessAmbientLight(in uint lightID, in vec3 baseColor)
{
	return ambientLightColor(lightID) * baseColor;
}

vec3 proccessImageBasedLight(
	in uint lightID,
	in vec3 baseColor,
	in float metalness,
	in float roughness,
	in vec3 F0,
	in vec3 normal,
	in vec3 texelPos)
{
	vec3 result = vec3(0.0f);
	
	const uint BRDFLutMapID = imageBasedLightBRDFLutMapID(lightID);
	const uint diffuseMapID = imageBasedLightDiffuseMapID(lightID);
	const uint specularMapID = imageBasedLightSpecularMapID(lightID);
	
	if ((BRDFLutMapID != 0xFFFFFFFFu) && (diffuseMapID != 0xFFFFFFFFu) && (specularMapID != 0xFFFFFFFFu))
	{
		const TextureHandle BRDFLutMapTextureHandle = mapTextureHandle(BRDFLutMapID);
		const TextureHandle diffuseMapTextureHandle = mapTextureHandle(diffuseMapID);
		const TextureHandle specularMapTextureHandle = mapTextureHandle(specularMapID);
		if (!isTextureHandleEmpty(BRDFLutMapTextureHandle) &&
			!isTextureHandleEmpty(diffuseMapTextureHandle) &&
			!isTextureHandleEmpty(specularMapTextureHandle))
		{
			const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
			const vec3 N = normalize(transformVector(lightTransformInverted, normal));
			const vec3 V = normalize(transformVector(lightTransformInverted, cameraViewPosition() - texelPos));
			
			result = calculateIBL(
				sampler2D(BRDFLutMapTextureHandle),
				samplerCube(diffuseMapTextureHandle),
				samplerCube(specularMapTextureHandle),
				baseColor,
				metalness,
				roughness,
				F0,
				N,
				V);
				
			result *= imageBasedLightContribution(lightID);
		}
	}
	
	return result;
}

vec3 proccessDirectionalLight(
	in uint lightID,
	in vec3 baseColor,
	in float alpha,
	in float metalness,
	in float roughness,
	in bool isShadowed,
	in vec3 F0,
	in vec3 normal,
	in vec3 texelPosWS)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 N = normalize(transformVector(lightTransformInverted, normal));
	const vec3 V = normalize(transformVector(lightTransformInverted, cameraViewPosition() - texelPosWS));
	const vec3 L = vec3(0.0f, 0.0f, 1.0f);
	
	vec3 result = calculatePBR(
		directionalLightColor(lightID),
		baseColor,
		metalness,
		roughness,
		F0,
		N,
		L,
		V);
		
	if (isShadowed)
	{
		const uint shadowID = lightShadowID(lightID);
		if (shadowID != 0xFFFFFFFFu)
		{
			uint layerID = 0xFFFFFFFFu;
			vec3 texelPosLVS;
			vec3 NDC_ZO;
			
			const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
			const uint layersCount = shadowLayersCount(shadowID);
			for (uint i = 0u; i < layersCount; ++i)
			{
				texelPosLVS = transformPoint(shadowTransformsDataViewTransform(transformsDataOffset, i), texelPosWS);
				const vec4 texelPosLCS = shadowTransformsDataProjectionMatrix(transformsDataOffset, i) * vec4(texelPosLVS, 1.0f);
				NDC_ZO = (texelPosLCS.xyz / texelPosLCS.w) * 0.5f + vec3(0.5f);
				if (all(lessThanEqual(vec3(0.0f), NDC_ZO)) && all(lessThanEqual(NDC_ZO, vec3(1.0f))))
				{
					layerID = i;
					break;
				}
			}
			
			if (layerID != 0xFFFFFFFFu)
			{
				const Range layerZRange = shadowTransformsDataZRange(transformsDataOffset, layerID);
				const float linearNormalizedDepth = (-texelPosLVS[2u] - rangeStart(layerZRange)) / (rangeEnd(layerZRange) - rangeStart(layerZRange));
			
				result *= proccessShadow(
					NDC_ZO,
					linearNormalizedDepth,
					shadowTransformsDataMapCoords(transformsDataOffset, layerID),
					shadowMapSize(shadowID),
					alpha);
			}
		}
	}
	
	return result;
}

vec3 proccessPointLight(
	in uint lightID,
	in vec3 baseColor,
	in float alpha,
	in float metalness,
	in float roughness,
	in bool isShadowed,
	in vec3 F0,
	in vec3 normal,
	in vec3 texelPosWS)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 texelPosLS = transformPoint(lightTransformInverted, texelPosWS);
	const float texelPosLSLen = length(texelPosLS);
	
	const vec3 N = normalize(transformVector(lightTransformInverted, normal));
	const vec3 V = normalize(transformVector(lightTransformInverted, cameraViewPosition() - texelPosWS));
	const vec3 L = (texelPosLSLen > EPS) ? (-texelPosLS / texelPosLSLen) : vec3(0.0f);
	
	vec3 result = calculatePBR(
		pointLightColor(lightID),
		baseColor,
		metalness,
		roughness,
		F0,
		N,
		L,
		V);
		
	const vec2 radiuses = pointLightRadiuses(lightID);
	result *= 1.0f - smoothstep(radiuses[0u], radiuses[1u], texelPosLSLen);
	
	if (isShadowed)
	{
		const uint shadowID = lightShadowID(lightID);
		if (shadowID != 0xFFFFFFFFu)
		{
			uint layerID = 0xFFFFFFFFu;
			
			const vec3 absL = abs(L);
			if (absL.x >= absL.y && absL.x >= absL.z)
			{
				layerID = L.x > 0.0f ? 1u : 0u;
			}
			else if (absL.y >= absL.x && absL.y >= absL.z)
			{
				layerID = L.y > 0.0f ? 3u : 2u;
			}
			else if (absL.z >= absL.x && absL.z >= absL.y)
			{
				layerID = L.z > 0.0f ? 5u : 4u;
			}
			
			if (layerID != 0xFFFFFFFFu)
			{
				const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
				const vec3 texelPosLVS = transformPoint(shadowTransformsDataViewTransform(transformsDataOffset, layerID), texelPosWS);
				const vec4 texelPosLCS = shadowTransformsDataProjectionMatrix(transformsDataOffset, layerID) * vec4(texelPosLVS, 1.0f);
				const vec3 NDC_ZO = (texelPosLCS.xyz / texelPosLCS.w) * 0.5f + vec3(0.5f);
				
				const Range layerZRange = shadowTransformsDataZRange(transformsDataOffset, layerID);
				const float linearNormalizedDepth = (-texelPosLVS[2u] - rangeStart(layerZRange)) / (rangeEnd(layerZRange) - rangeStart(layerZRange));
				
				result *= proccessShadow(
					NDC_ZO,
					linearNormalizedDepth,
					shadowTransformsDataMapCoords(transformsDataOffset, layerID),
					shadowMapSize(shadowID),
					alpha);
			}
		}
	}
	
	return result;
}

vec3 proccessSpotLight(
	in uint lightID,
	in vec3 baseColor,
	in float alpha,
	in float metalness,
	in float roughness,
	in bool isShadowed,
	in vec3 F0,
	in vec3 normal,
	in vec3 texelPosWS)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 texelPosLS = transformPoint(lightTransformInverted, texelPosWS);
	const float texelPosLSLen = length(texelPosLS);
	
	const vec3 N = normalize(transformVector(lightTransformInverted, normal));
	const vec3 V = normalize(transformVector(lightTransformInverted, cameraViewPosition() - texelPosWS));
	const vec3 L = (texelPosLSLen > EPS) ? (-texelPosLS / texelPosLSLen) : vec3(0.0f);
	
	vec3 result = calculatePBR(
		spotLightColor(lightID),
		baseColor,
		metalness,
		roughness,
		F0,
		N,
		L,
		V);
		
	const vec2 radiuses = spotLightRadiuses(lightID);
	const vec2 cosHalfAngles = cos(spotLightHalfAngles(lightID));
	result *=
		(1.0f - smoothstep(radiuses[0u], radiuses[1u], texelPosLSLen)) *
		(1.0f - smoothstep(cosHalfAngles[0u], cosHalfAngles[1u], L.z));
		
	if (isShadowed)
	{
		const uint shadowID = lightShadowID(lightID);
		if (shadowID != 0xFFFFFFFFu)
		{
			const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
			const vec3 texelPosLVS = transformPoint(shadowTransformsDataViewTransform(transformsDataOffset, 0u), texelPosWS);
			const vec4 texelPosLCS = shadowTransformsDataProjectionMatrix(transformsDataOffset, 0u) * vec4(texelPosLVS, 1.0f);
			const vec3 NDC_ZO = (texelPosLCS.xyz / texelPosLCS.w) * 0.5f + vec3(0.5f);
			
			if (all(lessThanEqual(vec3(0.0f), NDC_ZO)) && all(lessThanEqual(NDC_ZO, vec3(1.0f))))
			{
				const Range layerZRange = shadowTransformsDataZRange(transformsDataOffset, 0u);
				const float linearNormalizedDepth = (-texelPosLVS[2u] - rangeStart(layerZRange)) / (rangeEnd(layerZRange) - rangeStart(layerZRange));
				
				result *= proccessShadow(
					NDC_ZO,
					linearNormalizedDepth,
					shadowTransformsDataMapCoords(transformsDataOffset, 0u),
					shadowMapSize(shadowID),
					alpha);
			}
		}
	}
	
	return result;
}

vec4 proccessLighting(in vec3 NDC_ZO, in uvec4 PBRData)
{
	vec3 baseColor;
	vec3 emission;
	float occlusion;
	float roughness;
	float metalness;
	float alpha;
	vec3 normalWS;
	bool isLighted;
	bool isShadowed;
	unpackPBRData(PBRData, baseColor, emission, occlusion, roughness, metalness, alpha, normalWS, isLighted, isShadowed);
	
	vec3 color = emission;
	
	if (isLighted)
	{
		const vec3 texelPosWS = projectPoint(cameraViewProjectionMatrixInverted(), 2.0f * NDC_ZO - vec3(1.0f));
		const vec3 F0 = mix(vec3(renderInfoDielectricSpecular()), baseColor, metalness);
		
		const uint firstLightNodeIDs[2u] = uint[2u](
			countersFirstGlobalLightNodeID(),
			clusterNodeFirstLightNodeID(cameraClusterNodeID(NDC_ZO)));
		
		for (uint i = 0u; i < 2u; ++i)
		{
			uint lightNodeID = firstLightNodeIDs[i];
			while (lightNodeID != 0xFFFFFFFFu)
			{
				const uint lightID = lightNodeLightID(lightNodeID);
				const uint typeID = lightTypeID(lightID);
				
				if (typeID == AmbientLightTypeID)
				{
					color += proccessAmbientLight(lightID, baseColor);
				}
				else if (typeID == ImageBasedLightTypeID)
				{
					color += proccessImageBasedLight(lightID, baseColor, metalness, roughness, F0, normalWS, texelPosWS);
				}
				else if (typeID == DirectionalLightTypeID)
				{
					color += proccessDirectionalLight(lightID, baseColor, alpha, metalness, roughness, isShadowed, F0, normalWS, texelPosWS);
				}
				else if (typeID == PointLightTypeID)
				{
					color += proccessPointLight(lightID, baseColor, alpha, metalness, roughness, isShadowed, F0, normalWS, texelPosWS);
				}
				else if (typeID == SpotLightTypeID)
				{
					color += proccessSpotLight(lightID, baseColor, alpha, metalness, roughness, isShadowed, F0, normalWS, texelPosWS);
				}
				
				lightNodeID = lightNodeNextID(lightNodeID);
			}
		}
	}
	else
	{
		color += baseColor;
	}
	
	return vec4(color /** occlusion*/, alpha);
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
