#include<descriptions.glsl>

layout (std430) buffer ssbo_GBuffer { GBufferDescription GBuffer; };
layout (std430) buffer ssbo_OITNodesBuffer { OITNodeDescription OITNodes[]; };

OITNodeDescription makeOITNodeDescription(in uvec4 PBRData, in float depth, in uint nextOITNodeID)
{
	OITNodeDescription desc;
	desc.PBRData = PBRData;
	desc.depth = depth;
	desc.nextID = nextOITNodeID;
	return desc;
}

uvec2 geometryBufferSize()
{
	return GBuffer.size;
}

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
	
	uint sortedOITNodeID = 0xFFFFFFFFu;
    uint currentOITNodeID = imageLoad(image, fragCoords).r;
    float currentOITNodeDepth = 0.0f;
    
    while (currentOITNodeID != 0xFFFFFFFFu)
    {
        currentOITNodeDepth = OITNodes[currentOITNodeID].depth;
    
        uint nextIndex = OITNodes[currentOITNodeID].nextID;
        
        if ((sortedOITNodeID == 0xFFFFFFFFu) || (OITNodes[sortedOITNodeID].depth < currentOITNodeDepth))
        {
            OITNodes[currentOITNodeID].nextID = sortedOITNodeID;
            sortedOITNodeID = currentOITNodeID;
        }
        else
        {
            uint newIndex = sortedOITNodeID;
            while ((OITNodes[newIndex].nextID != 0xFFFFFFFFu) && (OITNodes[OITNodes[newIndex].nextID].depth > currentOITNodeDepth))
                newIndex = OITNodes[newIndex].nextID;

            OITNodes[currentOITNodeID].nextID = OITNodes[newIndex].nextID;
            OITNodes[newIndex].nextID = currentOITNodeID;
        }
        
        currentOITNodeID = nextIndex;
    }

    imageStore(image, fragCoords, uvec4(sortedOITNodeID));
}

void geometryBufferData(in ivec2 fragCoords, out uvec4 PBRData, out float depth, out uint nextOITNodeID)
{
	PBRData = texelFetch(usampler2DRect(GBuffer.colorTextureHandle), fragCoords);
	depth = texelFetch(sampler2DRect(GBuffer.depthStencilTextureHandle), fragCoords).r;
	
	layout(r32ui) uimage2DRect image = layout(r32ui) uimage2DRect(GBuffer.OITNodeIDImageHandle);
	nextOITNodeID = imageLoad(image, fragCoords).r;
}

void geometryBufferData(in uint OITNodeID, out uvec4 PBRData, out float depth, out uint nextOITNodeID)
{
	PBRData = OITNodes[OITNodeID].PBRData;
	depth = OITNodes[OITNodeID].depth;
	nextOITNodeID = OITNodes[OITNodeID].nextID;
}