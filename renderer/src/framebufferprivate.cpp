#include <algorithm>

#include "framebufferprivate.h"

namespace renderer {

const std::array<GLenum, castFromFramebufferAttachment<size_t>(FramebufferAttachment::Count)> framebufferAttachmentTable {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_DEPTH_ATTACHMENT,
	GL_STENCIL_ATTACHMENT,
	GL_DEPTH_STENCIL_ATTACHMENT
};

GLenum toFramebufferGLAttachment(FramebufferAttachment val)
{
	return framebufferAttachmentTable[castFromFramebufferAttachment<size_t>(val)];
}

FramebufferAttachment fromFramebufferGLAttachment(GLenum val)
{
	return castToFramebufferAttachment(std::find(framebufferAttachmentTable.cbegin(), framebufferAttachmentTable.cend(), val) - framebufferAttachmentTable.cbegin());
}

const std::array<GLenum, castFromPrimitiveType<size_t>(PrimitiveType::Count)> primitiveTypeTable {
	GL_POINTS,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_LINES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_TRIANGLES
};

GLenum toPrimitiveGLType(PrimitiveType val)
{
	return primitiveTypeTable[castFromPrimitiveType<size_t>(val)];
}

PrimitiveType fromPrimitiveGLType(GLenum val)
{
	return castToPrimitiveType(std::find(primitiveTypeTable.cbegin(), primitiveTypeTable.cend(), val) - primitiveTypeTable.cbegin());
}

const std::array<GLenum, castFromGeometryIndexType<size_t>(GeometryIndexType::Count)> geometryIndexTypeTable {
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT
};

GLenum toGeometryIndexGLType(GeometryIndexType val)
{
	return geometryIndexTypeTable[castFromGeometryIndexType<size_t>(val)];
}

GeometryIndexType fromGeometryIndexGLType(GLenum val)
{
	return castToGeometryIndexType(std::find(geometryIndexTypeTable.cbegin(), geometryIndexTypeTable.cend(), val) - geometryIndexTypeTable.cbegin());
}

const std::array<GLenum, castFromDepthTestFunc<size_t>(DepthTestFunc::Count)> depthTestFuncTable {
	GL_LEQUAL,
	GL_GEQUAL,
	GL_LESS,
	GL_GREATER,
	GL_EQUAL,
	GL_NOTEQUAL,
	GL_ALWAYS,
	GL_NEVER
};

GLenum toDepthTestGLFunc(DepthTestFunc val)
{
	return depthTestFuncTable[castFromDepthTestFunc<size_t>(val)];
}

DepthTestFunc fromDepthTestGLFunc(GLenum val)
{
	return castToDepthTestFunc(std::find(depthTestFuncTable.cbegin(), depthTestFuncTable.cend(), val) - depthTestFuncTable.cbegin());
}


}
