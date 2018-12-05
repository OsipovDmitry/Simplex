#include <algorithm>

#include "framebuffer_p.h"

namespace renderer {

const std::array<GLenum, castFromFramebufferAttachment(FramebufferAttachment::Count)> framebufferAttachmentTable {
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
    return framebufferAttachmentTable[castFromFramebufferAttachment(val)];
}

FramebufferAttachment fromFramebufferGLAttachment(GLenum val)
{
	return castToFramebufferAttachment(std::find(framebufferAttachmentTable.cbegin(), framebufferAttachmentTable.cend(), val) - framebufferAttachmentTable.cbegin());
}

const std::array<GLenum, castFromPrimitiveType(PrimitiveType::Count)> primitiveTypeTable {
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
    return primitiveTypeTable[castFromPrimitiveType(val)];
}

PrimitiveType fromPrimitiveGLType(GLenum val)
{
	return castToPrimitiveType(std::find(primitiveTypeTable.cbegin(), primitiveTypeTable.cend(), val) - primitiveTypeTable.cbegin());
}

const std::array<GLenum, castFromGeometryIndexType(GeometryIndexType::Count)> geometryIndexTypeTable {
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT
};

GLenum toGeometryIndexGLType(GeometryIndexType val)
{
    return geometryIndexTypeTable[castFromGeometryIndexType(val)];
}

GeometryIndexType fromGeometryIndexGLType(GLenum val)
{
	return castToGeometryIndexType(std::find(geometryIndexTypeTable.cbegin(), geometryIndexTypeTable.cend(), val) - geometryIndexTypeTable.cbegin());
}

}
