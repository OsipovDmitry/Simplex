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

vec3 proccessAmbientLight(in uint lightID, in vec3 baseColor)
{
	return coloredLightColor(lightID) * baseColor;
}

vec3 proccessDirectionalLight(
	in uint lightID,
	in vec3 baseColor,
	in float metalness,
	in float roughness,
	in bool isShadowed,
	in vec3 F0,
	in vec3 normal,
	in vec3 view,
	in vec3 texelPosWS,
	in bool isTexelTransparent)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 N = normalize(transformVector(lightTransformInverted, normal));
	const vec3 V = normalize(transformVector(lightTransformInverted, view));
	const vec3 L = vec3(0.0f, 0.0f, 1.0f);
	
	vec3 result = calculatePBR(
		coloredLightColor(lightID),
		baseColor,
		metalness,
		roughness,
		F0,
		N,
		L,
		V);
		
	if (isShadowed)
	{
		const uint shadowID = shadowedLightShadowID(lightID);
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
				NDC_ZO = projectPoint(
					shadowTransformsDataProjectionMatrix(transformsDataOffset, i),
					texelPosLVS) * 0.5f + vec3(0.5f);
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
			
				result *= shadowMapsProccessMomentsShadow(
					NDC_ZO.xy,
					linearNormalizedDepth,
					shadowTransformsDataMapCoords(transformsDataOffset, layerID),
					shadowMapSize(shadowID),
					0.0f,
					0.000001f,
					isTexelTransparent);
			}
		}
	}
	
	return result;
}

vec3 proccessPointLight(
	in uint lightID,
	in vec3 baseColor,
	in float metalness,
	in float roughness,
	in bool isShadowed,
	in vec3 F0,
	in vec3 normal,
	in vec3 view,
	in vec3 texelPosWS,
	in bool isTexelTransparent)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 texelPosLS = transformPoint(lightTransformInverted, texelPosWS);
	const float texelPosLSLen = length(texelPosLS);
	
	const vec3 N = normalize(transformVector(lightTransformInverted, normal));
	const vec3 V = normalize(transformVector(lightTransformInverted, view));
	const vec3 L = (texelPosLSLen > EPS) ? (-texelPosLS / texelPosLSLen) : vec3(0.0f);
	
	vec3 result = calculatePBR(
		coloredLightColor(lightID),
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
		const uint shadowID = shadowedLightShadowID(lightID);
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
				const vec3 NDC_ZO = projectPoint(
					shadowTransformsDataProjectionMatrix(transformsDataOffset, layerID),
					texelPosLVS) * 0.5f + vec3(0.5f);
				
				const Range layerZRange = shadowTransformsDataZRange(transformsDataOffset, layerID);
				const float linearNormalizedDepth = (-texelPosLVS[2u] - rangeStart(layerZRange)) / (rangeEnd(layerZRange) - rangeStart(layerZRange));
				
				result *= shadowMapsProccessMomentsShadow(
					NDC_ZO.xy,
					linearNormalizedDepth,
					shadowTransformsDataMapCoords(transformsDataOffset, layerID),
					shadowMapSize(shadowID),
					0.0f,
					0.000001f,
					isTexelTransparent);
			}
		}
	}
	
	return result;
}

vec3 proccessSpotLight(
	in uint lightID,
	in vec3 baseColor,
	in float metalness,
	in float roughness,
	in bool isShadowed,
	in vec3 F0,
	in vec3 normal,
	in vec3 view,
	in vec3 texelPosWS,
	in bool isTexelTransparent)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 texelPosLS = transformPoint(lightTransformInverted, texelPosWS);
	const float texelPosLSLen = length(texelPosLS);
	
	const vec3 N = normalize(transformVector(lightTransformInverted, normal));
	const vec3 V = normalize(transformVector(lightTransformInverted, view));
	const vec3 L = (texelPosLSLen > EPS) ? (-texelPosLS / texelPosLSLen) : vec3(0.0f);
	
	vec3 result = calculatePBR(
		coloredLightColor(lightID),
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
		const uint shadowID = shadowedLightShadowID(lightID);
		if (shadowID != 0xFFFFFFFFu)
		{
			const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
			const vec3 texelPosLVS = transformPoint(shadowTransformsDataViewTransform(transformsDataOffset, 0u), texelPosWS);
			const vec3 NDC_ZO = projectPoint(
				shadowTransformsDataProjectionMatrix(transformsDataOffset, 0u),
				texelPosLVS) * 0.5f + vec3(0.5f);
			
			if (all(lessThanEqual(vec3(0.0f), NDC_ZO)) && all(lessThanEqual(NDC_ZO, vec3(1.0f))))
			{
				const Range layerZRange = shadowTransformsDataZRange(transformsDataOffset, 0u);
				const float linearNormalizedDepth = (-texelPosLVS[2u] - rangeStart(layerZRange)) / (rangeEnd(layerZRange) - rangeStart(layerZRange));
				
				result *= shadowMapsProccessMomentsShadow(
					NDC_ZO.xy,
					linearNormalizedDepth,
					shadowTransformsDataMapCoords(transformsDataOffset, 0u),
					shadowMapSize(shadowID),
					0.0f,
					0.000001f,
					isTexelTransparent);
			}
		}
	}
	
	return result;
}

vec3 proccessImageBasedLight(
	in uint lightID,
	in vec3 baseColor,
	in float metalness,
	in float roughness,
	in vec3 F0,
	in vec3 normal,
	in vec3 view,
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
			const vec3 V = normalize(transformVector(lightTransformInverted, view));
			
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

vec4 proccessLighting(in vec3 NDC_ZO, in uvec4 PBRData, in bool isTexelTransparent)
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
		const vec3 viewWS = normalize(cameraViewPosition() - texelPosWS);
		
		const uint firstLightNodeIDs[2u] = uint[2u](
			countersFirstGlobalLightNodeID(),
			clusterNodeFirstLightNodeID(cameraClusterNodeID(NDC_ZO)));
		
		for (uint i = 0u; i < 2u; ++i)
		{
			for (uint lightNodeID = firstLightNodeIDs[i]; lightNodeID != 0xFFFFFFFFu; lightNodeID = lightNodeNextID(lightNodeID))
			{
				const uint lightID = lightNodeLightID(lightNodeID);
				const uint typeID = lightTypeID(lightID);
				
				if (!IsLightTypeDefined(typeID))
					continue;
				
				if (typeID == AmbientLightTypeID)
				{
					color += proccessAmbientLight(lightID, baseColor);
				}
				else if (typeID == DirectionalLightTypeID)
				{
					color += proccessDirectionalLight(lightID, baseColor, metalness, roughness, isShadowed, F0, normalWS, viewWS, texelPosWS, isTexelTransparent);
				}
				else if (typeID == PointLightTypeID)
				{
					color += proccessPointLight(lightID, baseColor, metalness, roughness, isShadowed, F0, normalWS, viewWS, texelPosWS, isTexelTransparent);
				}
				else if (typeID == SpotLightTypeID)
				{
					color += proccessSpotLight(lightID, baseColor, metalness, roughness, isShadowed, F0, normalWS, viewWS, texelPosWS, isTexelTransparent);
				}
				else if (typeID == ImageBasedLightTypeID)
				{
					color += proccessImageBasedLight(lightID, baseColor, metalness, roughness, F0, normalWS, viewWS, texelPosWS);
				}
			}
		}
	}
	else
	{
		color += baseColor;
	}
	
	return vec4(color /** occlusion*/, alpha);
}

vec3 proccessDirectionalScattering(in uint lightID, in vec3 texelPosWS)
{
	vec3 result = coloredLightColor(lightID);
		
	const uint shadowID = shadowedLightShadowID(lightID);
	if (shadowID != 0xFFFFFFFFu)
	{
		uint layerID = 0xFFFFFFFFu;
		vec3 NDC_ZO;
		
		const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
		const uint layersCount = shadowLayersCount(shadowID);
		for (uint i = 0u; i < layersCount; ++i)
		{
			NDC_ZO = projectPoint(shadowTransformsDataViewProjectionMatrix(transformsDataOffset, i), texelPosWS) * 0.5f + vec3(0.5f);
			if (all(lessThanEqual(vec3(0.0f), NDC_ZO)) && all(lessThanEqual(NDC_ZO, vec3(1.0f))))
			{
				layerID = i;
				break;
			}
		}
		
		if (layerID != 0xFFFFFFFFu)
		{
			result *= shadowMapsProcessDiscreteShadow(
				NDC_ZO,
				shadowTransformsDataMapCoords(transformsDataOffset, layerID),
				shadowMapSize(shadowID),
				false);
		}
	}
	
	return result;
}

vec3 proccessPointScattering(in uint lightID, in vec3 texelPosWS)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 texelPosLS = transformPoint(lightTransformInverted, texelPosWS);
	const float texelPosLSLen = length(texelPosLS);
	
	const vec3 L = (texelPosLSLen > EPS) ? (-texelPosLS / texelPosLSLen) : vec3(0.0f);
	
	vec3 result = coloredLightColor(lightID);
		
	const vec2 radiuses = pointLightRadiuses(lightID);
	result *= 1.0f - smoothstep(radiuses[0u], radiuses[1u], texelPosLSLen);
	
	const uint shadowID = shadowedLightShadowID(lightID);
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
			const vec3 NDC_ZO = projectPoint(
				shadowTransformsDataViewProjectionMatrix(transformsDataOffset, layerID),
				texelPosWS) * 0.5f + vec3(0.5f);
			
			result *= shadowMapsProcessDiscreteShadow(
				NDC_ZO,
				shadowTransformsDataMapCoords(transformsDataOffset, layerID),
				shadowMapSize(shadowID),
				false);
		}
	}
	
	return result;
}

vec3 proccessSpotScattering(in uint lightID, in vec3 texelPosWS)
{
	const Transform lightTransformInverted = transformInverted(lightTransform(lightID));
	
	const vec3 texelPosLS = transformPoint(lightTransformInverted, texelPosWS);
	const float texelPosLSLen = length(texelPosLS);
	
	const vec3 L = (texelPosLSLen > EPS) ? (-texelPosLS / texelPosLSLen) : vec3(0.0f);
	
	vec3 result = coloredLightColor(lightID);
		
	const vec2 radiuses = spotLightRadiuses(lightID);
	const vec2 cosHalfAngles = cos(spotLightHalfAngles(lightID));
	result *=
		(1.0f - smoothstep(radiuses[0u], radiuses[1u], texelPosLSLen)) *
		(1.0f - smoothstep(cosHalfAngles[0u], cosHalfAngles[1u], L.z));
		
	const uint shadowID = shadowedLightShadowID(lightID);
	if (shadowID != 0xFFFFFFFFu)
	{
		const uint transformsDataOffset = shadowTransformsDataOffset(shadowID);
		const vec3 NDC_ZO = projectPoint(
			shadowTransformsDataViewProjectionMatrix(transformsDataOffset, 0u),
			texelPosWS) * 0.5f + vec3(0.5f);
		
		if (all(lessThanEqual(vec3(0.0f), NDC_ZO)) && all(lessThanEqual(NDC_ZO, vec3(1.0f))))
		{
			result *= shadowMapsProcessDiscreteShadow(
				NDC_ZO,
				shadowTransformsDataMapCoords(transformsDataOffset, 0u),
				shadowMapSize(shadowID),
				false);
		}
	}
	
	return result;
}

vec3 proccessScattering(in vec2 NDC_XY_ZO, in float depthFrom, in float depthTo)
{
	if (depthFrom <= depthTo)
		return vec3(0.0f);
		
	const float STEP = 0.1f;
	const float FACTOR = 0.005f;

	vec3 color = vec3(0.0f);
	
	const vec3 pointFromWS = projectPoint(cameraViewProjectionMatrixInverted(), 2.0f * vec3(NDC_XY_ZO, depthFrom) - vec3(1.0f));
	const vec3 pointToWS = projectPoint(cameraViewProjectionMatrixInverted(), 2.0f * vec3(NDC_XY_ZO, depthTo) - vec3(1.0f));
	const float fromToWSLen = length(pointToWS - pointFromWS);
	
	float prevRayDepth = 0.0f;
	do
	{
		const float nextRayDepth = min(prevRayDepth + STEP, fromToWSLen);
		const float contribution = nextRayDepth - prevRayDepth;
		prevRayDepth = nextRayDepth;
		
		const vec3 texelPosWS = mix(pointFromWS, pointToWS, nextRayDepth / fromToWSLen);
		const vec3 NDC_ZO = projectPoint(cameraViewProjectionMatrix(), texelPosWS) * 0.5f + vec3(0.5f);
		
		const uint firstLightNodeIDs[2u] = uint[2u](
			countersFirstGlobalLightNodeID(),
			clusterNodeFirstLightNodeID(cameraClusterNodeID(NDC_ZO)));
		
		for (uint i = 0u; i < 2u; ++i)
		{
			for (uint lightNodeID = firstLightNodeIDs[i]; lightNodeID != 0xFFFFFFFFu; lightNodeID = lightNodeNextID(lightNodeID))
			{
				const uint lightID = lightNodeLightID(lightNodeID);
				const uint typeID = lightTypeID(lightID);
				
				if (!IsLightTypeShadowed(typeID))
					continue;
					
				if (!shadowedLightIsVolumetricScatteringEnabled(lightID))
					continue;
				
				if (typeID == DirectionalLightTypeID)
				{
					color += proccessDirectionalScattering(lightID, texelPosWS) * contribution;
				}
				else if (typeID == PointLightTypeID)
				{
					color += proccessPointScattering(lightID, texelPosWS) * contribution;
				}
				else if (typeID == SpotLightTypeID)
				{
					color += proccessSpotScattering(lightID, texelPosWS) * contribution;
				}
			}
		}
	} while (prevRayDepth + EPS < fromToWSLen);
	
	return color * FACTOR;
}

void main(void)
{
	const ivec2 fragCoords = ivec2(gl_FragCoord.xy);
	const vec2 NDC_XY_ZO = vec2(gl_FragCoord.xy) / vec2(geometryBufferSize() - uvec2(1u));
	
	uvec4 PBRData;
	float depth;
	uint OITNodeID;
	
	geometryBufferData(fragCoords, PBRData, depth, OITNodeID);
	vec4 color = proccessLighting(vec3(NDC_XY_ZO, depth), PBRData, false);
	color.rgb += proccessScattering(NDC_XY_ZO, depth, geometryBufferDepth(OITNodeID));
	
	while (geometryBufferData(OITNodeID, PBRData, depth, OITNodeID))
	{
		const vec4 fragColor = proccessLighting(vec3(NDC_XY_ZO, depth), PBRData, true);
		color = blend(fragColor, color);
		color.rgb += proccessScattering(NDC_XY_ZO, depth, geometryBufferDepth(OITNodeID));
	}

	o_fragColor0 = color;
}
