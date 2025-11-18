struct DrawIndirectCommandsBufferReservedData
{
	uint count;
	uint maxSize;
	uint padding[2u];
};

struct DrawArraysIndirectCommand
{
    uint count;
    uint instanceCount;
    uint first;
    uint baseInstance;
};

struct DrawElementsIndirectCommand
{
    uint count;
    uint instanceCount;
    uint firstIndex;
    int  baseVertex;
    uint baseInstance;
};