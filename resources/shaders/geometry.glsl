#include<descriptions.glsl>

#include<math/bounding_box.glsl>

layout (std430) buffer ssbo_GBuffer { GBufferDescription GBuffer; };
layout (std430) buffer ssbo_OITNodesBuffer { OITNodeDescription OITNodes[]; };

uint geometryBufferOITNodesMaxCount()
{
	return GBuffer.OITNodesMaxCount;
}

uint geometryBufferGenerateOITNodeID()
{
	return atomicAdd(GBuffer.OITNodesCount, 1u);
}

void geometryBufferInitializeOITNode(in uint OITNodeID, in uvec4 PBRData, in float depth, in uint nextOITNodeID)
{
	OITNodes[OITNodeID] = makeOITNodeDescription(PBRData, depth, nextOITNodeID);
}

void geometryBufferClearFirstOITNodeID(in ivec2 fragCoords)
{
	layout(r32ui) uimage2DRect image = layout(r32ui) uimage2DRect(GBuffer.OITNodeIDImageHandle);
	
	if (all(lessThan(fragCoords, imageSize(image))))
		imageStore(image, fragCoords, uvec4(0xFFFFFFFFu));
}

uint geometryBufferSetFirstOITNodeID(in ivec2 fragCoords, in uint OITNodeID)
{
	layout(r32ui) uimage2DRect image = layout(r32ui) uimage2DRect(GBuffer.OITNodeIDImageHandle);
	return imageAtomicExchange(image, fragCoords, OITNodeID);
}

void geometryBufferSortOITNodes(in ivec2 fragCoords)
{
	layout(r32ui) uimage2DRect image = layout(r32ui) uimage2DRect(GBuffer.OITNodeIDImageHandle);
	
	if (all(lessThan(fragCoords, imageSize(image))))
	{
		uint sortedOITNodeID = 0xFFFFFFFFu;
		uint currentOITNodeID = imageLoad(image, fragCoords).r;
		float currentOITNodeDepth = 0.0f;
    
		while (currentOITNodeID != 0xFFFFFFFFu)
		{
			currentOITNodeDepth = OITNodes[currentOITNodeID].depth;
    
			uint nextIndex = OITNodes[currentOITNodeID].nextID;
        
			if ((sortedOITNodeID == 0xFFFFFFFFu) || (OITNodes[sortedOITNodeID].depth > currentOITNodeDepth))
			{
				OITNodes[currentOITNodeID].nextID = sortedOITNodeID;
				sortedOITNodeID = currentOITNodeID;
			}
			else
			{
				uint newIndex = sortedOITNodeID;
				while ((OITNodes[newIndex].nextID != 0xFFFFFFFFu) && (OITNodes[OITNodes[newIndex].nextID].depth < currentOITNodeDepth))
					newIndex = OITNodes[newIndex].nextID;

				OITNodes[currentOITNodeID].nextID = OITNodes[newIndex].nextID;
				OITNodes[newIndex].nextID = currentOITNodeID;
			}
        
			currentOITNodeID = nextIndex;
		}

		imageStore(image, fragCoords, uvec4(sortedOITNodeID));
	}
}

bool geometryBufferData(in ivec2 fragCoords, out uvec4 PBRData, out float depth, out uint nextOITNodeID)
{
	PBRData = texelFetch(usampler2DRect(GBuffer.colorTextureHandle), fragCoords);
	depth = texelFetch(sampler2D(GBuffer.depthTextureHandle), fragCoords, 0).r;
	
	layout(r32ui) uimage2DRect image = layout(r32ui) uimage2DRect(GBuffer.OITNodeIDImageHandle);
	nextOITNodeID = imageLoad(image, fragCoords).r;
	
	return true;
}

bool geometryBufferData(in uint OITNodeID, out uvec4 PBRData, out float depth, out uint nextOITNodeID)
{
	if (OITNodeID == 0xFFFFFFFFu)
		return false;

	PBRData = OITNodes[OITNodeID].PBRData;
	depth = OITNodes[OITNodeID].depth;
	nextOITNodeID = OITNodes[OITNodeID].nextID;
	
	return true;
}

float geometryBufferDepth(in ivec2 fragCoords, in uint level)
{
	return texelFetch(sampler2D(GBuffer.depthTextureHandle), fragCoords, int(level)).r;
}

float geometryBufferDepth(in uint OITNodeID)
{
	if (OITNodeID == 0xFFFFFFFFu)
		return 0.0f;
		
	return OITNodes[OITNodeID].depth;
}

float geometryBufferCalculateDepthTextureLevel(in ivec2 fragCoords)
{
	const ivec2 sourceFragCoords = 2 * fragCoords;
	const int sourceLevel = int(GBuffer.generateDepthTextureLevelsPassIndex);
	
	const float d0 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(0, 0), sourceLevel).r;
	const float d1 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(1, 0), sourceLevel).r;
	const float d2 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(0, 1), sourceLevel).r;
	const float d3 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(1, 1), sourceLevel).r;
	
	float minDepth = min(min(d0, d1), min(d2, d3));
	
	const ivec2 sourceLevelSize = textureSize(sampler2D(GBuffer.depthTextureHandle), sourceLevel);
	const bool extraColumn = (sourceLevelSize[0u] & 1) != 0;
	const bool extraRow = (sourceLevelSize[1u] & 1) != 0;
	
	if (extraColumn)
	{
		const float d0 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(2, 0), sourceLevel).r;
		const float d1 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(2, 1), sourceLevel).r;
		minDepth = min(minDepth, min(d0, d1));
	}
	
	if (extraRow)
	{
		const float d0 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(0, 2), sourceLevel).r;
		const float d1 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(1, 2), sourceLevel).r;
		minDepth = min(minDepth, min(d0, d1));
	}
	
	if (extraColumn && extraRow)
	{
		const float d0 = texelFetch(sampler2D(GBuffer.depthTextureHandle), sourceFragCoords + ivec2(2, 2), sourceLevel).r;
		minDepth = min(minDepth, d0);
	}
	
	return minDepth;
}

uint geometryBufferGenerateDepthTextureLevelsPassIndex()
{
	return GBuffer.generateDepthTextureLevelsPassIndex;
}

bool geometryTestBoundingBox(in BoundingBox bbNDC)
{
	const vec2 minUV = clamp(NO2ZO(vec2(boundingBoxMinPoint(bbNDC))), vec2(0.0f), vec2(1.0f));
	const vec2 maxUV = clamp(NO2ZO(vec2(boundingBoxMaxPoint(bbNDC))), vec2(0.0f), vec2(1.0f));
	
	const vec2 boxSizePixels = (maxUV - minUV) * vec2(textureSize(sampler2D(GBuffer.depthTextureHandle), 0));
	const float maxBoxSize = max(boxSizePixels.x, boxSizePixels.y);
	
	const int maxLevel = textureQueryLevels(sampler2D(GBuffer.depthTextureHandle)) - 1;
	const int mipLevel = clamp(int(ceil(log2(maxBoxSize))), 0, maxLevel);
	
	const ivec2 mipSize = textureSize(sampler2D(GBuffer.depthTextureHandle), mipLevel);
	const ivec2 maxCoord = mipSize - ivec2(1);
	
	const ivec2 pixelMin = clamp(ivec2(floor(minUV * vec2(mipSize))), ivec2(0), maxCoord);
	const ivec2 pixelMax = clamp(ivec2(ceil(maxUV * vec2(mipSize))) - ivec2(1), ivec2(0), maxCoord);
	
	const float d0 = texelFetch(sampler2D(GBuffer.depthTextureHandle), ivec2(pixelMin.x, pixelMin.y), mipLevel).r;
	const float d1 = texelFetch(sampler2D(GBuffer.depthTextureHandle), ivec2(pixelMax.x, pixelMin.y), mipLevel).r;
	const float d2 = texelFetch(sampler2D(GBuffer.depthTextureHandle), ivec2(pixelMin.x, pixelMax.y), mipLevel).r;
	const float d3 = texelFetch(sampler2D(GBuffer.depthTextureHandle), ivec2(pixelMax.x, pixelMax.y), mipLevel).r;
	const float minDepth = min(min(d0, d1), min(d2, d3));
	
	return boundingBoxMaxPoint(bbNDC)[2u] >= minDepth;
}