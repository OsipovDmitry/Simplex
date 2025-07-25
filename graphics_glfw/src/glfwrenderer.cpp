#include <array>
#include <functional>

//#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/glm/gtx/texture.hpp>
#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/image.h>
#include <utils/shader.h>

#include <core/renderinfo.h>
#include <core/drawable.h>
#include <core/uniform.h>

#include <graphics_glfw/glfwwidget.h>

#include "glfwwidgetprivate.h"
#include "glfwrenderer.h"
#include "mvpstateset.h"

namespace simplex
{
namespace graphics_glfw
{

// Conversions

GLenum Conversions::PrimitiveType2GL(utils::PrimitiveType value)
{
    static std::array<GLenum, utils::numElementsPrimitiveType()> s_table {
        GL_POINTS,
        GL_LINES,
        GL_LINE_STRIP,
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN,
    };

    return s_table[utils::castFromPrimitiveType(value)];
}

GLenum Conversions::VertexComponentType2GL(utils::VertexComponentType value)
{
    static std::array<GLenum, utils::numElementsVertexComponentType()> s_table {
        GL_FLOAT,
        GL_DOUBLE,
        GL_BYTE,
        GL_UNSIGNED_BYTE,
        GL_SHORT,
        GL_UNSIGNED_SHORT,
        GL_INT,
        GL_UNSIGNED_INT
    };

    return s_table[utils::castFromVertexComponentType(value)];
}

GLenum Conversions::DrawElementsIndexType2GL(utils::DrawElementsIndexType value)
{
    static std::array<GLenum, utils::numElementsDrawElementsIndexType()> s_table {
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_INT
    };

    return s_table[utils::castFromDrawElementsIndexType(value)];
}

GLenum Conversions::PixelInternalFormat2GL(core::graphics::PixelInternalFormat value)
{
    static std::array<GLenum, core::graphics::numElementsPixelInternalFormat()> s_table {
        GL_NONE,
        GL_R8, GL_R8_SNORM, GL_R16, GL_R16_SNORM,
        GL_RG8, GL_RG8_SNORM, GL_RG16, GL_RG16_SNORM,
        GL_R3_G3_B2, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB8_SNORM, GL_RGB10, GL_RGB12, GL_RGB16, GL_RGB16_SNORM, GL_RGBA2,
        GL_RGBA4, GL_RGB5_A1, GL_RGBA8, GL_RGBA8_SNORM, GL_RGB10_A2, GL_RGB10_A2UI, GL_RGBA12, GL_RGBA16, GL_RGBA16_SNORM,
        GL_SRGB8, GL_SRGB8_ALPHA8,
        GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F,
        GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F,
        GL_R11F_G11F_B10F, GL_RGB9_E5,
        GL_R8I, GL_R8UI, GL_R16I, GL_R16UI, GL_R32I, GL_R32UI,
        GL_RG8I, GL_RG8UI, GL_RG16I, GL_RG16UI, GL_RG32I, GL_RG32UI,
        GL_RGB8I, GL_RGB8UI, GL_RGB16I, GL_RGB16UI, GL_RGB32I, GL_RGB32UI,
        GL_RGBA8I, GL_RGBA8UI, GL_RGBA16I, GL_RGBA16UI, GL_RGBA32I, GL_RGBA32UI,
        GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32F, GL_STENCIL_INDEX8, GL_DEPTH24_STENCIL8, GL_DEPTH32F_STENCIL8
    };

    return s_table[core::graphics::castFromPixelInternalFormat(value)];
}

core::graphics::PixelInternalFormat Conversions::GL2PixelInternalFormat(GLenum value)
{
    static std::unordered_map<GLenum, core::graphics::PixelInternalFormat> s_table {
        { GL_R8, core::graphics::PixelInternalFormat::R8 },
        { GL_R8_SNORM, core::graphics::PixelInternalFormat::R8_SNORM },
        { GL_R16, core::graphics::PixelInternalFormat::R16 },
        { GL_R16_SNORM, core::graphics::PixelInternalFormat::R16_SNORM },
        { GL_RG8, core::graphics::PixelInternalFormat::RG8 },
        { GL_RG8_SNORM, core::graphics::PixelInternalFormat::RG8_SNORM },
        { GL_RG16, core::graphics::PixelInternalFormat::RG16 },
        { GL_RG16_SNORM, core::graphics::PixelInternalFormat::RG16_SNORM },
        { GL_R3_G3_B2, core::graphics::PixelInternalFormat::R3_G3_B2 },
        { GL_RGB4, core::graphics::PixelInternalFormat::RGB4 },
        { GL_RGB5, core::graphics::PixelInternalFormat::RGB5 },
        { GL_RGB8, core::graphics::PixelInternalFormat::RGB8 },
        { GL_RGB8_SNORM, core::graphics::PixelInternalFormat::RGB8_SNORM },
        { GL_RGB10, core::graphics::PixelInternalFormat::RGB10 },
        { GL_RGB12, core::graphics::PixelInternalFormat::RGB12 },
        { GL_RGB16, core::graphics::PixelInternalFormat::RGB16 },
        { GL_RGB16_SNORM, core::graphics::PixelInternalFormat::RGB16_SNORM },
        { GL_RGBA2, core::graphics::PixelInternalFormat::RGBA2 },
        { GL_RGBA4, core::graphics::PixelInternalFormat::RGBA4 },
        { GL_RGB5_A1, core::graphics::PixelInternalFormat::RGB5_A1 },
        { GL_RGBA8, core::graphics::PixelInternalFormat::RGBA8 },
        { GL_RGBA8_SNORM, core::graphics::PixelInternalFormat::RGBA8_SNORM },
        { GL_RGB10_A2, core::graphics::PixelInternalFormat::RGB10_A2 },
        { GL_RGB10_A2UI, core::graphics::PixelInternalFormat::RGB10_A2UI },
        { GL_RGBA12, core::graphics::PixelInternalFormat::RGBA12 },
        { GL_RGBA16, core::graphics::PixelInternalFormat::RGBA16 },
        { GL_RGBA16_SNORM, core::graphics::PixelInternalFormat::RGBA16_SNORM },
        { GL_SRGB8, core::graphics::PixelInternalFormat::SRGB8 },
        { GL_SRGB8_ALPHA8, core::graphics::PixelInternalFormat::SRGB8_ALPHA8 },
        { GL_R16F, core::graphics::PixelInternalFormat::R16F },
        { GL_RG16F, core::graphics::PixelInternalFormat::RG16F },
        { GL_RGB16F, core::graphics::PixelInternalFormat::RGB16F },
        { GL_RGBA16F, core::graphics::PixelInternalFormat::RGBA16F },
        { GL_R32F, core::graphics::PixelInternalFormat::R32F },
        { GL_RG32F, core::graphics::PixelInternalFormat::RG32F },
        { GL_RGB32F, core::graphics::PixelInternalFormat::RGB32F },
        { GL_RGBA32F, core::graphics::PixelInternalFormat::RGBA32F },
        { GL_R11F_G11F_B10F, core::graphics::PixelInternalFormat::R11F_G11F_B10F },
        { GL_RGB9_E5, core::graphics::PixelInternalFormat::RGB9_E5 },
        { GL_R8I, core::graphics::PixelInternalFormat::R8I },
        { GL_R8UI, core::graphics::PixelInternalFormat::R8UI },
        { GL_R16I, core::graphics::PixelInternalFormat::R16I },
        { GL_R16UI, core::graphics::PixelInternalFormat::R16UI },
        { GL_R32I, core::graphics::PixelInternalFormat::R32I },
        { GL_R32UI, core::graphics::PixelInternalFormat::R32UI },
        { GL_RG8I, core::graphics::PixelInternalFormat::RG8I },
        { GL_RG8UI, core::graphics::PixelInternalFormat::RG8UI },
        { GL_RG16I, core::graphics::PixelInternalFormat::RG16I },
        { GL_RG16UI, core::graphics::PixelInternalFormat::RG16UI },
        { GL_RG32I, core::graphics::PixelInternalFormat::RG32I },
        { GL_RG32UI, core::graphics::PixelInternalFormat::RG32UI },
        { GL_RGB8I, core::graphics::PixelInternalFormat::RGB8I },
        { GL_RGB8UI, core::graphics::PixelInternalFormat::RGB8UI },
        { GL_RGB16I, core::graphics::PixelInternalFormat::RGB16I },
        { GL_RGB16UI, core::graphics::PixelInternalFormat::RGB16UI },
        { GL_RGB32I, core::graphics::PixelInternalFormat::RGB32I },
        { GL_RGB32UI, core::graphics::PixelInternalFormat::RGB32UI },
        { GL_RGBA8I, core::graphics::PixelInternalFormat::RGBA8I },
        { GL_RGBA8UI, core::graphics::PixelInternalFormat::RGBA8UI },
        { GL_RGBA16I, core::graphics::PixelInternalFormat::RGBA16I },
        { GL_RGBA16UI, core::graphics::PixelInternalFormat::RGBA16UI },
        { GL_RGBA32I, core::graphics::PixelInternalFormat::RGBA32I },
        { GL_RGBA32UI, core::graphics::PixelInternalFormat::RGBA32UI },
        { GL_DEPTH_COMPONENT16, core::graphics::PixelInternalFormat::Depth16 },
        { GL_DEPTH_COMPONENT24, core::graphics::PixelInternalFormat::Depth24 },
        { GL_DEPTH_COMPONENT32F, core::graphics::PixelInternalFormat::Depth32F },
        { GL_STENCIL_INDEX8, core::graphics::PixelInternalFormat::Stencil8 },
        { GL_DEPTH24_STENCIL8, core::graphics::PixelInternalFormat::Depth24Stencil8 },
        { GL_DEPTH32F_STENCIL8, core::graphics::PixelInternalFormat::Dept32FStencil8 },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::PixelInternalFormat::Undefined : it->second;
}

GLenum Conversions::PixelNumComponents2GL(uint32_t value)
{
    GLenum result = GL_NONE;
    switch (value)
    {
    case 1: { result = GL_RED; break; }
    case 2: { result = GL_RG; break; }
    case 3: { result = GL_RGB; break; }
    case 4: { result = GL_RGBA; break; }
    default: break;
    }

    return result;
}

GLenum Conversions::PixelComponentType2GL(utils::PixelComponentType value)
{
    static std::array<GLenum, utils::numElementsPixelComponentType()> s_table {
        GL_FLOAT,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT
    };

    return s_table[utils::castFromPixelComponentType(value)];
}

GLenum Conversions::FrameBufferAttachment2GL(core::graphics::FrameBufferAttachment value)
{
    static std::array<GLenum, core::graphics::numElementsFrameBufferAttachment()> s_table {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_DEPTH_ATTACHMENT,
        GL_STENCIL_ATTACHMENT,
        GL_DEPTH_STENCIL_ATTACHMENT
    };

    return s_table[core::graphics::castFromFrameBufferAttachment(value)];
}

GLenum Conversions::TextureType2GL(core::graphics::TextureType value)
{
    static std::array<GLenum, core::graphics::numElementsTextureType()> s_table {
        GL_TEXTURE_1D,
        GL_TEXTURE_2D,
        GL_TEXTURE_3D,
        GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_1D_ARRAY,
        GL_TEXTURE_2D_ARRAY,
        GL_TEXTURE_CUBE_MAP_ARRAY,
        GL_TEXTURE_RECTANGLE
    };

    return s_table[core::graphics::castFromTextureType(value)];
}

GLenum Conversions::TextureWrapMode2GL(core::graphics::TextureWrapMode value)
{
    static std::array<GLenum, core::graphics::numElementsTextureWrapMode()> s_table {
        GL_CLAMP_TO_EDGE,
        GL_CLAMP_TO_BORDER,
        GL_MIRRORED_REPEAT,
        GL_REPEAT,
        GL_MIRROR_CLAMP_TO_EDGE
    };

    return s_table[core::graphics::castFromTextureWrapMode(value)];
}

GLenum Conversions::TextureSwizzle2GL(core::graphics::TextureSwizzle value)
{
    static std::array<GLenum, core::graphics::numElementsTextureSwizzle()> s_table {
        GL_RED,
        GL_GREEN,
        GL_BLUE,
        GL_ALPHA,
        GL_ZERO,
        GL_ONE
    };

    return s_table[core::graphics::castFromTextureSwizzle(value)];
}

GLbitfield Conversions::BufferMapAccess2GL(core::graphics::IBuffer::MapAccess value)
{
    static std::array<GLbitfield, core::graphics::IBuffer::numElementsMapAccess()> s_table {
        GL_MAP_READ_BIT,
        GL_MAP_WRITE_BIT,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
    };

    return s_table[core::graphics::IBuffer::castFromMapAccess(value)];
}

GLenum Conversions::ImageDataAccess2GL(core::graphics::IImage::DataAccess value)
{
    static std::array<GLbitfield, core::graphics::IImage::numElementsDataAccess()> s_table {
        GL_READ_ONLY,
        GL_WRITE_ONLY,
        GL_READ_WRITE
    };

    return s_table[core::graphics::IImage::castFromDataAccess(value)];
}

uint16_t Conversions::GL2VertexNumComponents(GLenum value)
{
    static std::unordered_map<GLenum, uint16_t> s_table {
        { GL_FLOAT, 1u },
        { GL_FLOAT_VEC2, 2u },
        { GL_FLOAT_VEC3, 3u },
        { GL_FLOAT_VEC4, 4u },
        { GL_DOUBLE, 1u },
        { GL_DOUBLE_VEC2, 2u },
        { GL_DOUBLE_VEC3, 3u },
        { GL_DOUBLE_VEC4, 4u },
        { GL_BYTE, 1u },
        { GL_UNSIGNED_BYTE, 1u },
        { GL_SHORT, 1u },
        { GL_UNSIGNED_SHORT, 1u },
        { GL_INT, 1u },
        { GL_INT_VEC2, 2u },
        { GL_INT_VEC3, 3u},
        { GL_INT_VEC4, 4u },
        { GL_UNSIGNED_INT, 1u },
        { GL_UNSIGNED_INT_VEC2, 2u },
        { GL_UNSIGNED_INT_VEC3, 3u },
        { GL_UNSIGNED_INT_VEC4, 4u }
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? 0u : it->second;
}

utils::VertexComponentType Conversions::GL2VertexComponentType(GLenum value)
{
    static std::unordered_map<GLenum, utils::VertexComponentType> s_table {
        { GL_FLOAT, utils::VertexComponentType::Single },
        { GL_FLOAT_VEC2, utils::VertexComponentType::Single },
        { GL_FLOAT_VEC3, utils::VertexComponentType::Single },
        { GL_FLOAT_VEC4, utils::VertexComponentType::Single },
        { GL_DOUBLE, utils::VertexComponentType::Double },
        { GL_DOUBLE_VEC2, utils::VertexComponentType::Double },
        { GL_DOUBLE_VEC3, utils::VertexComponentType::Double },
        { GL_DOUBLE_VEC4, utils::VertexComponentType::Double },
        { GL_BYTE, utils::VertexComponentType::Int8 },
        { GL_UNSIGNED_BYTE, utils::VertexComponentType::Uint8 },
        { GL_SHORT, utils::VertexComponentType::Int16 },
        { GL_UNSIGNED_SHORT, utils::VertexComponentType::Uint16 },
        { GL_INT, utils::VertexComponentType::Int32 },
        { GL_INT_VEC2, utils::VertexComponentType::Int32 },
        { GL_INT_VEC3, utils::VertexComponentType::Int32},
        { GL_INT_VEC4, utils::VertexComponentType::Int32 },
        { GL_UNSIGNED_INT, utils::VertexComponentType::Uint32 },
        { GL_UNSIGNED_INT_VEC2, utils::VertexComponentType::Uint32 },
        { GL_UNSIGNED_INT_VEC3, utils::VertexComponentType::Uint32 },
        { GL_UNSIGNED_INT_VEC4, utils::VertexComponentType::Uint32 }
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? utils::VertexComponentType::Count : it->second;
}

GLenum Conversions::UniformType2GL(core::graphics::UniformType value)
{
    static std::array<GLenum, core::graphics::numElementsUniformType()> s_table {
        GL_NONE,
        GL_FLOAT,
        GL_FLOAT_VEC2,
        GL_FLOAT_VEC3,
        GL_FLOAT_VEC4,
        GL_FLOAT_MAT2,
        GL_FLOAT_MAT3,
        GL_FLOAT_MAT4,
        GL_DOUBLE,
        GL_DOUBLE_VEC2,
        GL_DOUBLE_VEC3,
        GL_DOUBLE_VEC4,
        GL_DOUBLE_MAT2,
        GL_DOUBLE_MAT3,
        GL_DOUBLE_MAT4,
        GL_INT,
        GL_INT_VEC2,
        GL_INT_VEC3,
        GL_INT_VEC4,
        GL_UNSIGNED_INT,
        GL_UNSIGNED_INT_VEC2,
        GL_UNSIGNED_INT_VEC3,
        GL_UNSIGNED_INT_VEC4,
        GL_SAMPLER_1D,
        GL_SAMPLER_2D,
        GL_SAMPLER_3D,
        GL_SAMPLER_CUBE,
        GL_SAMPLER_1D_ARRAY,
        GL_SAMPLER_2D_ARRAY,
        GL_SAMPLER_CUBE_MAP_ARRAY,
        GL_SAMPLER_2D_RECT,
        GL_IMAGE_1D,
        GL_IMAGE_2D,
        GL_IMAGE_3D,
        GL_IMAGE_CUBE,
        GL_IMAGE_1D_ARRAY,
        GL_IMAGE_2D_ARRAY,
        GL_IMAGE_CUBE_MAP_ARRAY,
        GL_IMAGE_2D_RECT,
        GL_UNSIGNED_INT_ATOMIC_COUNTER
    };

    return s_table[core::graphics::castFromUniformType(value)];
}

core::graphics::UniformType Conversions::GL2UniformType(GLenum value)
{
    static std::unordered_map<GLenum, core::graphics::UniformType> s_table {
        { GL_BOOL, core::graphics::UniformType::Bool },
        { GL_FLOAT, core::graphics::UniformType::Single },
        { GL_FLOAT_VEC2, core::graphics::UniformType::SingleVec2 },
        { GL_FLOAT_VEC3, core::graphics::UniformType::SingleVec3 },
        { GL_FLOAT_VEC4, core::graphics::UniformType::SingleVec4 },
        { GL_FLOAT_MAT2, core::graphics::UniformType::SingleMat2 },
        { GL_FLOAT_MAT3, core::graphics::UniformType::SingleMat3 },
        { GL_FLOAT_MAT4, core::graphics::UniformType::SingleMat4 },
        { GL_DOUBLE, core::graphics::UniformType::Double },
        { GL_DOUBLE_VEC2, core::graphics::UniformType::DoubleVec2 },
        { GL_DOUBLE_VEC3, core::graphics::UniformType::DoubleVec3 },
        { GL_DOUBLE_VEC4, core::graphics::UniformType::DoubleVec4 },
        { GL_DOUBLE_MAT2, core::graphics::UniformType::DoubleMat2 },
        { GL_DOUBLE_MAT3, core::graphics::UniformType::DoubleMat3 },
        { GL_DOUBLE_MAT4, core::graphics::UniformType::DoubleMat4 },
        { GL_INT, core::graphics::UniformType::Int32 },
        { GL_INT_VEC2, core::graphics::UniformType::Int32Vec2 },
        { GL_INT_VEC3, core::graphics::UniformType::Int32Vec3 },
        { GL_INT_VEC4, core::graphics::UniformType::Int32Vec4 },
        { GL_UNSIGNED_INT, core::graphics::UniformType::Uint32 },
        { GL_UNSIGNED_INT_VEC2, core::graphics::UniformType::Uint32Vec2 },
        { GL_UNSIGNED_INT_VEC3, core::graphics::UniformType::Uint32Vec3 },
        { GL_UNSIGNED_INT_VEC4, core::graphics::UniformType::Uint32Vec4 },
        { GL_SAMPLER_1D, core::graphics::UniformType::Sampler1D },
        { GL_SAMPLER_2D, core::graphics::UniformType::Sampler2D },
        { GL_SAMPLER_3D, core::graphics::UniformType::Sampler3D },
        { GL_SAMPLER_CUBE, core::graphics::UniformType::SamplerCube },
        { GL_SAMPLER_1D_ARRAY, core::graphics::UniformType::Sampler1DArray },
        { GL_SAMPLER_2D_ARRAY, core::graphics::UniformType::Sampler2DArray },
        { GL_SAMPLER_CUBE_MAP_ARRAY, core::graphics::UniformType::SamplerCubeArray },
        { GL_SAMPLER_2D_RECT, core::graphics::UniformType::SamplerRect },
        { GL_INT_SAMPLER_1D, core::graphics::UniformType::Sampler1D },
        { GL_INT_SAMPLER_2D, core::graphics::UniformType::Sampler2D },
        { GL_INT_SAMPLER_3D, core::graphics::UniformType::Sampler3D },
        { GL_INT_SAMPLER_CUBE, core::graphics::UniformType::SamplerCube },
        { GL_INT_SAMPLER_1D_ARRAY, core::graphics::UniformType::Sampler1DArray },
        { GL_INT_SAMPLER_2D_ARRAY, core::graphics::UniformType::Sampler2DArray },
        { GL_INT_SAMPLER_CUBE_MAP_ARRAY, core::graphics::UniformType::SamplerCubeArray },
        { GL_INT_SAMPLER_2D_RECT, core::graphics::UniformType::SamplerRect },
        { GL_UNSIGNED_INT_SAMPLER_1D, core::graphics::UniformType::Sampler1D },
        { GL_UNSIGNED_INT_SAMPLER_2D, core::graphics::UniformType::Sampler2D },
        { GL_UNSIGNED_INT_SAMPLER_3D, core::graphics::UniformType::Sampler3D },
        { GL_UNSIGNED_INT_SAMPLER_CUBE, core::graphics::UniformType::SamplerCube },
        { GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, core::graphics::UniformType::Sampler1DArray },
        { GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, core::graphics::UniformType::Sampler2DArray },
        { GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY, core::graphics::UniformType::SamplerCubeArray },
        { GL_UNSIGNED_INT_SAMPLER_2D_RECT, core::graphics::UniformType::SamplerRect },
        { GL_IMAGE_1D, core::graphics::UniformType::Image1D },
        { GL_IMAGE_2D, core::graphics::UniformType::Image2D },
        { GL_IMAGE_3D, core::graphics::UniformType::Image3D },
        { GL_IMAGE_CUBE, core::graphics::UniformType::ImageCube },
        { GL_IMAGE_1D_ARRAY, core::graphics::UniformType::Image1DArray },
        { GL_IMAGE_2D_ARRAY, core::graphics::UniformType::Image2DArray },
        { GL_IMAGE_CUBE_MAP_ARRAY, core::graphics::UniformType::ImageCubeArray },
        { GL_IMAGE_2D_RECT, core::graphics::UniformType::ImageRect },
        { GL_INT_IMAGE_1D, core::graphics::UniformType::Image1D },
        { GL_INT_IMAGE_2D, core::graphics::UniformType::Image2D },
        { GL_INT_IMAGE_3D, core::graphics::UniformType::Image3D },
        { GL_INT_IMAGE_CUBE, core::graphics::UniformType::ImageCube },
        { GL_INT_IMAGE_1D_ARRAY, core::graphics::UniformType::Image1DArray },
        { GL_INT_IMAGE_2D_ARRAY, core::graphics::UniformType::Image2DArray },
        { GL_INT_IMAGE_CUBE_MAP_ARRAY, core::graphics::UniformType::ImageCubeArray },
        { GL_INT_IMAGE_2D_RECT, core::graphics::UniformType::ImageRect },
        { GL_UNSIGNED_INT_IMAGE_1D, core::graphics::UniformType::Image1D },
        { GL_UNSIGNED_INT_IMAGE_2D, core::graphics::UniformType::Image2D },
        { GL_UNSIGNED_INT_IMAGE_3D, core::graphics::UniformType::Image3D },
        { GL_UNSIGNED_INT_IMAGE_CUBE, core::graphics::UniformType::ImageCube },
        { GL_UNSIGNED_INT_IMAGE_1D_ARRAY, core::graphics::UniformType::Image1DArray },
        { GL_UNSIGNED_INT_IMAGE_2D_ARRAY, core::graphics::UniformType::Image2DArray },
        { GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY, core::graphics::UniformType::ImageCubeArray },
        { GL_UNSIGNED_INT_IMAGE_2D_RECT, core::graphics::UniformType::ImageRect },
        { GL_UNSIGNED_INT_ATOMIC_COUNTER, core::graphics::UniformType::AtomicCounterUint },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::UniformType::Undefined : it->second;
}

GLenum Conversions::FaceType2GL(core::graphics::FaceType value)
{
    static std::array<GLenum, core::graphics::numElementsFaceType()> s_table {
        GL_FRONT,
        GL_BACK,
        GL_FRONT_AND_BACK
    };
    return s_table[core::graphics::castFromFaceType(value)];
}

GLenum Conversions::ComparingFunc2GL(core::graphics::ComparingFunc value)
{
    static std::array<GLenum, core::graphics::numElementsComparingFunc()> s_table {
        GL_NEVER,
        GL_LESS,
        GL_EQUAL,
        GL_LEQUAL,
        GL_GREATER,
        GL_NOTEQUAL,
        GL_GEQUAL,
        GL_ALWAYS
    };
    return s_table[core::graphics::castFromComparingFunc(value)];
}

GLenum Conversions::StencilOperation2GL(core::graphics::StencilOperation value)
{
    static std::array<GLenum, core::graphics::numElementsStencilOperation()> s_table {
        GL_KEEP,
        GL_ZERO,
        GL_REPLACE,
        GL_INCR,
        GL_INCR_WRAP,
        GL_DECR,
        GL_DECR_WRAP,
        GL_INVERT
    };
    return s_table[core::graphics::castFromStencilOperation(value)];
}

GLenum Conversions::BlendEquetion2GL(core::graphics::BlendEquation value)
{
    static std::array<GLenum, core::graphics::numElementsBlendEquation()> s_table {
        GL_FUNC_ADD,
        GL_FUNC_SUBTRACT,
        GL_FUNC_REVERSE_SUBTRACT,
        GL_MIN,
        GL_MAX
    };
    return s_table[core::graphics::castFromBlendEquation(value)];
}

GLenum Conversions::BlendFactor2GL(core::graphics::BlendFactor value)
{
    static std::array<GLenum, core::graphics::numElementsBlendFactor()> s_table {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA
    };
    return s_table[core::graphics::castFromBlendFactor(value)];
}

// BufferBase_4_5::MappedData_4_5

BufferBase_4_5::MappedData_4_5::MappedData_4_5(const std::weak_ptr<const BufferBase_4_5>& mappedBuffer, uint8_t *data)
    : m_mappedBuffer(mappedBuffer)
    , m_data(data)
{
    SAVE_CURRENT_CONTEXT
}

BufferBase_4_5::MappedData_4_5::~MappedData_4_5()
{
    CHECK_CURRENT_CONTEXT
    if (!m_mappedBuffer.expired())
    {
        auto bufferBase_4_5 = m_mappedBuffer.lock();
        glUnmapNamedBuffer(bufferBase_4_5->id());
        bufferBase_4_5->m_isMapped = false;
    }
}

const uint8_t *Buffer_4_5::MappedData_4_5::get() const
{
    CHECK_CURRENT_CONTEXT
    return const_cast<MappedData_4_5*>(this)->get();
}

uint8_t *Buffer_4_5::MappedData_4_5::get()
{
    CHECK_CURRENT_CONTEXT
    return m_mappedBuffer.expired() ? nullptr : m_data;
}

// BufferBase_4_5

BufferBase_4_5::~BufferBase_4_5()
{
    CHECK_CURRENT_CONTEXT
    glDeleteBuffers(1, &m_id);
}

GLuint BufferBase_4_5::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

size_t BufferBase_4_5::fullSize() const
{
    CHECK_CURRENT_CONTEXT
    GLint result = 0;
    glGetNamedBufferParameteriv(m_id, GL_BUFFER_SIZE, &result);
    return static_cast<size_t>(result);
}

void BufferBase_4_5::setFullSize(size_t newSize)
{
    CHECK_CURRENT_CONTEXT

    size_t oldSize = fullSize();
    if (oldSize == newSize)
        return;

    GLuint newID;
    glCreateBuffers(1, &newID);
    glNamedBufferStorage(newID, newSize, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    glCopyNamedBufferSubData(m_id, newID, 0u, 0u, glm::min(oldSize, newSize));
    glDeleteBuffers(1, &m_id);
    m_id = newID;
}

BufferBase_4_5::BufferBase_4_5(uint64_t size, const void* data)
{
    SAVE_CURRENT_CONTEXT
        glCreateBuffers(1, &m_id);
    glNamedBufferStorage(m_id, size, data, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
}

std::unique_ptr<BufferBase_4_5::MappedData_4_5> BufferBase_4_5::mapData(core::graphics::IBuffer::MapAccess access, size_t offset, size_t size)
{
    CHECK_CURRENT_CONTEXT
    if (m_isMapped)
    {
        LOG_ERROR << "Buffer is already mapped";
        return nullptr;
    }

    if (size == 0)
        size = fullSize() - offset;

    if (offset + size > fullSize())
    {
        LOG_ERROR << "The size of mapped data is out of range";
        return nullptr;
    }

    m_isMapped = true;

    return std::make_unique<MappedData_4_5>(
                weak_from_this(),
                static_cast<uint8_t*>(glMapNamedBufferRange(m_id,
                    static_cast<GLintptr>(offset),
                    static_cast<GLsizei>(size),
                    Conversions::BufferMapAccess2GL(access))));
}

std::unique_ptr<const BufferBase_4_5::MappedData_4_5> BufferBase_4_5::mapData(core::graphics::IBuffer::MapAccess access, size_t offset, size_t size) const
{
    CHECK_CURRENT_CONTEXT
    return const_cast<BufferBase_4_5*>(this)->mapData(access, offset, size);
}

// Buffer_4_5

Buffer_4_5::Buffer_4_5(uint64_t size, const void* data)
    : BufferBase_4_5(size, data)
{
    SAVE_CURRENT_CONTEXT
}

Buffer_4_5::~Buffer_4_5() = default;

size_t Buffer_4_5::size() const
{
    CHECK_CURRENT_CONTEXT
    return fullSize();
}

void Buffer_4_5::resize(size_t size)
{
    CHECK_CURRENT_CONTEXT
    setFullSize(size);
}

std::unique_ptr<const core::graphics::IBuffer::MappedData> Buffer_4_5::map(MapAccess access, size_t offset, size_t size) const
{
    CHECK_CURRENT_CONTEXT
    return mapData(access, offset, size);
}

std::unique_ptr<core::graphics::IBuffer::MappedData> Buffer_4_5::map(MapAccess access, size_t offset, size_t size)
{
    CHECK_CURRENT_CONTEXT
    return mapData(access, offset, size);
}

std::shared_ptr<Buffer_4_5> Buffer_4_5::create(size_t size, const void *data)
{
    return std::make_shared<Buffer_4_5>(size, data);
}

// DynamicBuffer_4_5

DynamicBuffer_4_5::DynamicBuffer_4_5(uint64_t size, const void* data)
    : BufferBase_4_5(size, data)
    , m_size(size)

{
    SAVE_CURRENT_CONTEXT
}

DynamicBuffer_4_5::~DynamicBuffer_4_5() = default;

size_t DynamicBuffer_4_5::capacity() const
{
    CHECK_CURRENT_CONTEXT
    return fullSize();
}

void DynamicBuffer_4_5::reserve(size_t size)
{
    CHECK_CURRENT_CONTEXT
    if (size > capacity())
        setFullSize(size);
}

void DynamicBuffer_4_5::pushBack(const void* data, size_t size)
{
    CHECK_CURRENT_CONTEXT
    expand(m_size + size);
    std::memcpy(map(IBuffer::MapAccess::WriteOnly, m_size, size)->get(), data, size);
    m_size += size;
}

size_t DynamicBuffer_4_5::size() const
{
    CHECK_CURRENT_CONTEXT
    return m_size;
}

void DynamicBuffer_4_5::resize(size_t size)
{
    CHECK_CURRENT_CONTEXT
    expand(size);
    m_size = size;
}

std::unique_ptr<const core::graphics::IBuffer::MappedData> DynamicBuffer_4_5::map(MapAccess access, size_t offset, size_t size) const
{
    CHECK_CURRENT_CONTEXT
    return mapData(access, offset, size);
}

std::unique_ptr<core::graphics::IBuffer::MappedData> DynamicBuffer_4_5::map(MapAccess access, size_t offset, size_t size)
{
    CHECK_CURRENT_CONTEXT
    return mapData(access, offset, size);
}

std::shared_ptr<DynamicBuffer_4_5> DynamicBuffer_4_5::create(size_t size, const void* data)
{
    return std::make_shared<DynamicBuffer_4_5>(size, data);
}


void DynamicBuffer_4_5::expand(size_t requiredSize)
{
    CHECK_CURRENT_CONTEXT
    if (requiredSize > capacity())
        reserve(glm::max(requiredSize, capacity() * 2u));
}

// VertexArray_4_5

VertexArray_4_5::VertexArray_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateVertexArrays(1, &m_id);
}

VertexArray_4_5::~VertexArray_4_5()
{
    CHECK_CURRENT_CONTEXT
    glDeleteVertexArrays(1, &m_id);
}

GLuint VertexArray_4_5::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

uint32_t VertexArray_4_5::attachVertexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer, size_t offset, uint32_t stride)
{
    CHECK_CURRENT_CONTEXT
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    if (!buffer_4_5)
        LOG_CRITICAL << "Buffer can't be nullptr";

    auto bindingIndex = static_cast<uint32_t>(-1);

    auto tuple = std::make_tuple(buffer_4_5, offset, stride);

    if (auto it = std::find(m_vertexBuffers.begin(), m_vertexBuffers.end(), tuple); it != m_vertexBuffers.end())
        return static_cast<uint32_t>(it - m_vertexBuffers.begin());

    auto it = std::find_if(m_vertexBuffers.begin(), m_vertexBuffers.end(), [](const auto &v){ return std::get<0>(v) == nullptr;});
    bindingIndex = static_cast<uint32_t>(it - m_vertexBuffers.begin());
    if (it == m_vertexBuffers.end())
        m_vertexBuffers.resize(m_vertexBuffers.size() + 1);
    m_vertexBuffers[bindingIndex] = tuple;

    glVertexArrayVertexBuffer(m_id, static_cast<GLuint>(bindingIndex), buffer_4_5->id(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));

    return bindingIndex;
}

void VertexArray_4_5::detachVertexBuffer(uint32_t bindingIndex)
{
    CHECK_CURRENT_CONTEXT
    m_vertexBuffers[bindingIndex] = std::make_tuple(nullptr, static_cast<size_t>(0), static_cast<uint32_t>(0));

    for (const auto &[attrib, tuple]: m_vertexDeclarations)
        if (std::get<0>(tuple) == bindingIndex)
            undeclareVertexAttribute(attrib);

    glVertexArrayVertexBuffer(m_id, static_cast<GLuint>(bindingIndex), 0, 0, 0);

}

std::shared_ptr<const core::graphics::IBuffer> VertexArray_4_5::vertexBuffer(uint32_t bindingIndex) const
{
    CHECK_CURRENT_CONTEXT
    return std::get<0>(m_vertexBuffers[bindingIndex]);
}

size_t VertexArray_4_5::vertexBufferOffset(uint32_t bindingIndex) const
{
    CHECK_CURRENT_CONTEXT
    return std::get<1>(m_vertexBuffers[bindingIndex]);
}

uint32_t VertexArray_4_5::vertexBufferStride(uint32_t bindingIndex) const
{
    CHECK_CURRENT_CONTEXT
    return std::get<2>(m_vertexBuffers[bindingIndex]);
}

void VertexArray_4_5::declareVertexAttribute(utils::VertexAttribute attrib,
                                             uint32_t bindingIndex,
                                             uint32_t numComponents,
                                             utils::VertexComponentType type,
                                             uint32_t relativeOffset)
{
    CHECK_CURRENT_CONTEXT
    if (numComponents < 1 || numComponents > 4)
        LOG_CRITICAL << "Num components must be [1..4]";

    m_vertexDeclarations[attrib] = std::make_tuple(bindingIndex, numComponents, type, relativeOffset);

    auto loc = static_cast<GLuint>(attrib);

    glVertexArrayAttribBinding(m_id, loc, static_cast<GLuint>(bindingIndex));
    glEnableVertexArrayAttrib(m_id, loc);

    switch (type)
    {
    case utils::VertexComponentType::Single: {
        glVertexArrayAttribFormat(m_id,
                                            loc,
                                            static_cast<GLint>(numComponents),
                                            Conversions::VertexComponentType2GL(type),
                                            GL_FALSE,
                                            static_cast<GLuint>(relativeOffset));
        break;
    }
    case utils::VertexComponentType::Double: {
        glVertexArrayAttribLFormat(m_id,
                                             loc,
                                             static_cast<GLint>(numComponents),
                                             Conversions::VertexComponentType2GL(type),
                                             static_cast<GLuint>(relativeOffset));
        break;
    }
    case utils::VertexComponentType::Int8:
    case utils::VertexComponentType::Uint8:
    case utils::VertexComponentType::Int16:
    case utils::VertexComponentType::Uint16:
    case utils::VertexComponentType::Int32:
    case utils::VertexComponentType::Uint32: {
        glVertexArrayAttribIFormat(m_id,
                                             loc,
                                             static_cast<GLint>(numComponents),
                                             Conversions::VertexComponentType2GL(type),
                                             static_cast<GLuint>(relativeOffset));
        break;
    }
    default: {
        break;
    }
    }
}

void VertexArray_4_5::undeclareVertexAttribute(utils::VertexAttribute attrib)
{
    CHECK_CURRENT_CONTEXT
    glDisableVertexArrayAttrib(m_id, static_cast<GLuint>(attrib));

    m_vertexDeclarations.erase(attrib);
}

uint32_t VertexArray_4_5::vertexAttributeBindingIndex(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<0>(it->second) : static_cast<uint32_t>(-1);
}

uint32_t VertexArray_4_5::vertexAttributeNumComponents(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<1>(it->second) : 0u;
}

utils::VertexComponentType VertexArray_4_5::vertexAttributeComponentType(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<2>(it->second) : utils::VertexComponentType::Count;
}

uint32_t VertexArray_4_5::vertexAttributeRelativeOffset(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<3>(it->second) : 0u;
}

void VertexArray_4_5::attachIndexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer)
{
    CHECK_CURRENT_CONTEXT
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    if (!buffer_4_5)
        LOG_CRITICAL << "Buffer can't be nullptr";

    m_indexBuffer = buffer_4_5;

    glVertexArrayElementBuffer(m_id, buffer_4_5->id());
}

void VertexArray_4_5::detachIndexBuffer()
{
    CHECK_CURRENT_CONTEXT
    m_indexBuffer = nullptr;

    glVertexArrayElementBuffer(m_id, 0);
}

std::shared_ptr<const core::graphics::IBuffer> VertexArray_4_5::indexBuffer() const
{
    CHECK_CURRENT_CONTEXT
    return m_indexBuffer;
}

void VertexArray_4_5::addPrimitiveSet(std::shared_ptr<utils::PrimitiveSet> primitiveSet)
{
    CHECK_CURRENT_CONTEXT
    if (!primitiveSet)
        LOG_CRITICAL << "Primitive set can't be nullptr";

    m_primitiveSets.insert(primitiveSet);
}

void VertexArray_4_5::removePrimitiveSet(std::shared_ptr<utils::PrimitiveSet> primitiveSet)
{
    CHECK_CURRENT_CONTEXT
    m_primitiveSets.erase(primitiveSet);
}

const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> &VertexArray_4_5::primitiveSets() const
{
    CHECK_CURRENT_CONTEXT
    return m_primitiveSets;
}

std::shared_ptr<VertexArray_4_5> VertexArray_4_5::create(const std::shared_ptr<const utils::Mesh> & mesh, bool uniteVertexBuffers)
{
    auto vertexArray = std::make_shared<VertexArray_4_5>();

    if (mesh)
    {
        size_t numVertices = mesh->vertexBuffers().empty() ? 0u : mesh->vertexBuffers().begin()->second->numVertices();

        if (uniteVertexBuffers)
        {
            size_t totalSize = 0u;
            size_t stride = 0u;
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                if (numVertices != buffer->numVertices())
                    LOG_CRITICAL << "Buffers have different size";

                totalSize += buffer->sizeInBytes();
                stride += buffer->numComponents() * utils::sizeOfVertexComponentType(buffer->componentType());
            }

            auto buffer = Buffer_4_5::create(totalSize);
            auto bindingIndex = vertexArray->attachVertexBuffer(buffer, 0u, stride);

            size_t relativeOffset = 0u;
            auto bufferData = buffer->map(core::graphics::IBuffer::MapAccess::WriteOnly);
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                size_t vertexSize = buffer->numComponents() * utils::sizeOfVertexComponentType(buffer->componentType());
                for (size_t i = 0; i < buffer->numVertices(); ++i)
                    std::memcpy(static_cast<uint8_t*>(bufferData->get()) + stride * i + relativeOffset, buffer->vertex(i), vertexSize);
                vertexArray->declareVertexAttribute(attrib, bindingIndex, buffer->numComponents(), buffer->componentType(), relativeOffset);
                relativeOffset += vertexSize;
            }
        }
        else
        {
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                if (numVertices != buffer->numVertices())
                    LOG_CRITICAL << "Buffers have different size";

                auto bindingIndex = vertexArray->attachVertexBuffer(Buffer_4_5::create(buffer->sizeInBytes(), buffer->data()),
                                                                    0u,
                                                                    buffer->numComponents() * utils::sizeOfVertexComponentType(buffer->componentType()));
                vertexArray->declareVertexAttribute(attrib, bindingIndex, buffer->numComponents(), buffer->componentType(), 0u);
            }
        }

        size_t indexBufferTotalSize = 0u;

        for (auto &primitiveSet : mesh->primitiveSets())
        {
            if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
            {
                vertexArray->addPrimitiveSet(std::make_shared<utils::DrawArrays>(drawArrays->primitiveType(),
                                                                                 drawArrays->first(),
                                                                                 drawArrays->count()));
            }
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
                if (auto drawElementsBuffer = drawElements->asDrawElementsBuffer(); drawElementsBuffer)
                {
                    vertexArray->addPrimitiveSet(std::make_shared<utils::DrawElements>(drawElementsBuffer->primitiveType(),
                                                                                       drawElementsBuffer->count(),
                                                                                       drawElementsBuffer->indexType(),
                                                                                       indexBufferTotalSize,
                                                                                       drawElementsBuffer->baseVertex()));
                    indexBufferTotalSize += drawElementsBuffer->sizeInBytes();
                }
        }

        if (indexBufferTotalSize)
        {
            auto buffer = Buffer_4_5::create(indexBufferTotalSize);
            vertexArray->attachIndexBuffer(buffer);

            auto bufferData = buffer->map(core::graphics::IBuffer::MapAccess::WriteOnly);
            size_t offset = 0;

            for (auto &primitiveSet : mesh->primitiveSets())
            {
                if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
                    if (auto drawElementsBuffer = drawElements->asDrawElementsBuffer(); drawElementsBuffer)
                    {
                        std::memcpy(static_cast<uint8_t*>(bufferData->get()) + offset, drawElementsBuffer->data(), drawElementsBuffer->sizeInBytes());
                        offset += drawElementsBuffer->sizeInBytes();
                    }
            }
        }
    }

    return vertexArray;
}

// TextureBase_4_5

TextureBase_4_5::TextureBase_4_5()
    : m_id(0u)
{
    SAVE_CURRENT_CONTEXT
}

TextureBase_4_5::~TextureBase_4_5()
{
    CHECK_CURRENT_CONTEXT
    glDeleteTextures(1, &m_id);
}

GLuint TextureBase_4_5::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

GLenum TextureBase_4_5::GLinternalFormat() const
{
    CHECK_CURRENT_CONTEXT
    GLint result;

    glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_INTERNAL_FORMAT, &result);

    return static_cast<GLenum>(result);
}

glm::uvec2 TextureBase_4_5::size() const
{
    CHECK_CURRENT_CONTEXT
    return mipmapSize(0u);
}

core::graphics::PixelInternalFormat TextureBase_4_5::internalFormat() const
{
    CHECK_CURRENT_CONTEXT
    return Conversions::GL2PixelInternalFormat(GLinternalFormat());
}

bool TextureBase_4_5::hasAlpha() const
{
    CHECK_CURRENT_CONTEXT
    GLint result;

    glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_ALPHA_SIZE, &result);

    return result != 0;
}

bool TextureBase_4_5::hasDepth() const
{
    CHECK_CURRENT_CONTEXT
    GLint result;

    glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_DEPTH_SIZE, &result);

    return result != 0;
}

glm::uvec3 TextureBase_4_5::mipmapSize(uint32_t level) const
{
    CHECK_CURRENT_CONTEXT
    GLint w, h, d;

    glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_WIDTH, &w);
    glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_HEIGHT, &h);
    glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_DEPTH, &d);

    return glm::uvec3(w, h, d);
}

uint32_t TextureBase_4_5::numMipmapLevels() const
{
    CHECK_CURRENT_CONTEXT
    GLint result;

    glGetTextureParameteriv(m_id, GL_TEXTURE_IMMUTABLE_LEVELS, &result);

    return static_cast<uint32_t>(result);
}

uint32_t TextureBase_4_5::numFaces() const
{
    CHECK_CURRENT_CONTEXT
    return 1u;
}

void TextureBase_4_5::subImage(uint32_t level,
                               const glm::uvec3 &offset,
                               const glm::uvec3 &size,
                               uint32_t numComponents,
                               utils::PixelComponentType type,
                               size_t bufSize,
                               void *data) const
{
    glGetTextureSubImage(m_id,
                                   static_cast<GLint>(level),
                                   static_cast<GLint>(offset.x),
                                   static_cast<GLint>(offset.y),
                                   static_cast<GLint>(offset.z),
                                   static_cast<GLsizei>(size.x),
                                   static_cast<GLsizei>(size.y),
                                   static_cast<GLsizei>(size.z),
                                   hasDepth() ? GL_DEPTH_COMPONENT : Conversions::PixelNumComponents2GL(numComponents),
                                   Conversions::PixelComponentType2GL(type),
                                   static_cast<GLsizei>(bufSize),
                                   data);
}

void TextureBase_4_5::generateMipmaps()
{
    CHECK_CURRENT_CONTEXT
    glGenerateTextureMipmap(m_id);
}

void TextureBase_4_5::setBorderColor(const glm::vec4 &value)
{
    CHECK_CURRENT_CONTEXT
    glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
}

void TextureBase_4_5::setWrapMode(core::graphics::TextureWrapMode value)
{
    CHECK_CURRENT_CONTEXT
    auto glValue = static_cast<GLint>(Conversions::TextureWrapMode2GL(value));

    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, glValue);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, glValue);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, glValue);
}

void TextureBase_4_5::setFilterMode(core::graphics::TextureFilterMode value)
{
    CHECK_CURRENT_CONTEXT
    switch (value)
    {
    case core::graphics::TextureFilterMode::Point: {
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    }
    case core::graphics::TextureFilterMode::Linear: {
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    }
    case core::graphics::TextureFilterMode::Bilinear: {
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    }
    case core::graphics::TextureFilterMode::Trilinear: {
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
    default: break;
    }
}

void TextureBase_4_5::setSwizzleMask(const core::graphics::TextureSwizzleMask &value)
{
    CHECK_CURRENT_CONTEXT
    const std::array<GLint, 4u> glValues {
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[0u])),
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[1u])),
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[2u])),
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[3u])),
    };

    glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_RGBA, glValues.data());
}

core::graphics::PTexture TextureBase_4_5::copy() const
{
    CHECK_CURRENT_CONTEXT
    auto result = std::dynamic_pointer_cast<TextureBase_4_5>(copyEmpty());
    if (!result)
        return nullptr;

    auto textureType = Conversions::TextureType2GL(type());
    auto textureNumFaces = numFaces();
    auto textureNumLevels = numMipmapLevels();

    for (uint32_t level = 0u; level < textureNumLevels; ++level)
    {
        auto size = mipmapSize(level);
        size.z *= textureNumFaces;

        glCopyImageSubData(m_id,
                                     textureType,
                                     static_cast<GLint>(level),
                                     0, 0, 0,
                                     result->m_id,
                                     textureType,
                                     static_cast<GLint>(level),
                                     0, 0, 0,
                                     static_cast<GLsizei>(size.x), static_cast<GLsizei>(size.y), static_cast<GLsizei>(size.z));
    }

    return result;
}

// Texture1D_4_5

Texture1D_4_5::Texture1D_4_5(uint32_t width, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_1D, 1, &m_id);
    glTextureStorage1D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width));
}

Texture1D_4_5::~Texture1D_4_5() = default;

core::graphics::TextureType Texture1D_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::Type1D;
}

void Texture1D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage1D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x),
                                  static_cast<GLint>(size.x),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture Texture1D_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, internalFormat(), numMipmapLevels());
}

std::shared_ptr<Texture1D_4_5> Texture1D_4_5::createEmpty(uint32_t width, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
{
    if (width == 0u)
        LOG_CRITICAL << "Width can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(width));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<Texture1D_4_5>(width, internalFormat, numLevels);
    result->setWrapMode(core::graphics::TextureWrapMode::Repeat);
    result->setFilterMode(numLevels > 1u ? core::graphics::TextureFilterMode::Trilinear : core::graphics::TextureFilterMode::Linear);

    return result;
}

std::shared_ptr<Texture1D_4_5> Texture1D_4_5::create(
    const std::shared_ptr<const utils::Image>& image,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels,
    bool genMipmaps)
{
    if (!image)
        LOG_CRITICAL << "Image can't be nullptr";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createEmpty(image->width(), internalFormat, numLevels);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), 0u, 0u), image->numComponents(), image->type(), image->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

// Texture2D_4_5

Texture2D_4_5::Texture2D_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    glTextureStorage2D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height));
}

Texture2D_4_5::~Texture2D_4_5() = default;

core::graphics::TextureType Texture2D_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::Type2D;
}

void Texture2D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture Texture2D_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, internalFormat(), numMipmapLevels());
}

std::shared_ptr<Texture2D_4_5> Texture2D_4_5::createEmpty(uint32_t width,
    uint32_t height,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels)
{
    if (width * height == 0u)
        LOG_CRITICAL << "Width and height can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec2(width, height)));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<Texture2D_4_5>(width, height, internalFormat, numLevels);
    result->setWrapMode(core::graphics::TextureWrapMode::Repeat);
    result->setFilterMode(numLevels > 1u ? core::graphics::TextureFilterMode::Trilinear : core::graphics::TextureFilterMode::Linear);

    return result;
}

std::shared_ptr<Texture2D_4_5> Texture2D_4_5::create(const std::shared_ptr<const utils::Image>& image,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels,
    bool genMipmaps)
{
    if (!image)
        LOG_CRITICAL << "Image can't be nullptr";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createEmpty(image->width(), image->height(), internalFormat, numLevels);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), image->height(), 0u), image->numComponents(), image->type(), image->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

// Texture3D_4_5

Texture3D_4_5::Texture3D_4_5(uint32_t width, uint32_t height, uint32_t depth, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_3D, 1, &m_id);
    glTextureStorage3D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height),
                                 static_cast<GLsizei>(depth));
}

Texture3D_4_5::~Texture3D_4_5() = default;

core::graphics::TextureType Texture3D_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::Type3D;
}

void Texture3D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture Texture3D_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, baseSize.z, internalFormat(), numMipmapLevels());
}

std::shared_ptr<Texture3D_4_5> Texture3D_4_5::createEmpty(uint32_t width,
    uint32_t height,
    uint32_t depth,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels)
{
    if (width * height * depth == 0u)
        LOG_CRITICAL << "Width, height and depth can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec3(width, height, depth)));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<Texture3D_4_5>(width, height, depth, internalFormat, numLevels);
    result->setWrapMode(core::graphics::TextureWrapMode::Repeat);
    result->setFilterMode(numLevels > 1u ? core::graphics::TextureFilterMode::Trilinear : core::graphics::TextureFilterMode::Linear);

    return result;
}

std::shared_ptr<Texture3D_4_5> Texture3D_4_5::create(const std::vector<std::shared_ptr<const utils::Image>> &images,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels,
    bool genMipmaps)
{
    if (images.empty())
        LOG_CRITICAL << "Images count of 3D texture can't be 0";

    for (const auto &image : images)
        if (!image)
            LOG_CRITICAL << "Image can't be nullptr";

    auto width = images[0u]->width();
    auto height = images[0u]->height();
    auto numComponents = images[0u]->numComponents();
    auto type = images[0u]->type();

    for (const auto &image : images)
        if ((width != image->width()) ||
            (height != image->height()) ||
            (numComponents != image->numComponents()) ||
            (type != image->type()))
            LOG_CRITICAL << "All the layers of 3D teture must be the the same size, components count and component pixel type";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(numComponents, type);

    auto result = createEmpty(width, height, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0; layer < static_cast<uint32_t>(images.size()); ++layer)
        result->setSubImage(0u, glm::uvec3(0u, 0u, layer), glm::uvec3(width, height, 1u), numComponents, type, images[layer]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

// TextureCube_4_5

TextureCube_4_5::TextureCube_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
    glTextureStorage2D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height));
}

TextureCube_4_5::~TextureCube_4_5() = default;

core::graphics::TextureType TextureCube_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::TypeCube;
}

uint32_t TextureCube_4_5::numFaces() const
{
    CHECK_CURRENT_CONTEXT
    return 6u;
}

void TextureCube_4_5::setSubImage(uint32_t level,const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT
    if (offset.z + size.z > 6u)
        LOG_CRITICAL << "Number of cubemap faces must be 6";

    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture TextureCube_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, internalFormat(), numMipmapLevels());
}

std::shared_ptr<TextureCube_4_5> TextureCube_4_5::createEmpty(uint32_t width,
    uint32_t height,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels)
{
    if (width * height == 0u)
        LOG_CRITICAL << "Width and height can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec2(width, height)));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<TextureCube_4_5>(width, height, internalFormat, numLevels);
    result->setFilterMode(numLevels > 1u ? core::graphics::TextureFilterMode::Trilinear : core::graphics::TextureFilterMode::Linear);

    return result;
}

std::shared_ptr<TextureCube_4_5> TextureCube_4_5::create(const std::vector<std::shared_ptr<const utils::Image>>& images,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels,
    bool genMipmaps)
{
    if (images.size() != 6u)
        LOG_CRITICAL << "Images count of cubemap texture must be 6";

    for (const auto &image : images)
        if (!image)
            LOG_CRITICAL << "Image can't be nullptr";

    auto width = images[0u]->width();
    auto height = images[0u]->height();
    auto numComponents = images[0u]->numComponents();
    auto type = images[0u]->type();

    for (const auto &image : images)
        if ((width != image->width()) ||
            (height != image->height()) ||
            (numComponents != image->numComponents()) ||
            (type != image->type()))
            LOG_CRITICAL << "All the faces of cubemap must be the the same size, components count and component pixel type";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(numComponents, type);

    auto result = createEmpty(width, height, internalFormat, numLevels);
    for (uint32_t face = 0; face < static_cast<uint32_t>(images.size()); ++face)
        result->setSubImage(0u, glm::uvec3(0u, 0u, face), glm::uvec3(width, height, 1u), numComponents, type, images[face]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

// Texture1DArray_4_5

Texture1DArray_4_5::Texture1DArray_4_5(uint32_t width, uint32_t numLayers, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_1D_ARRAY, 1, &m_id);
    glTextureStorage2D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(numLayers));
}

Texture1DArray_4_5::~Texture1DArray_4_5() = default;

core::graphics::TextureType Texture1DArray_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::Type1DArray;
}

void Texture1DArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture Texture1DArray_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, internalFormat(), numMipmapLevels());
}

std::shared_ptr<Texture1DArray_4_5> Texture1DArray_4_5::createEmpty(uint32_t width,
    uint32_t numLayers,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels)
{
    if (width * numLayers == 0u)
        LOG_CRITICAL << "Width and layers count of 1D array texture can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(width));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<Texture1DArray_4_5>(width, numLayers, internalFormat, numLevels);
    result->setWrapMode(core::graphics::TextureWrapMode::Repeat);
    result->setFilterMode(numLevels > 1u ? core::graphics::TextureFilterMode::Trilinear : core::graphics::TextureFilterMode::Linear);

    return result;
}

std::shared_ptr<Texture1DArray_4_5> Texture1DArray_4_5::create(const std::vector<std::shared_ptr<const utils::Image>>& images,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels,
    bool genMipmaps)
{
    if (images.empty())
        LOG_CRITICAL << "Layers count of 1D texture array can't be 0";

    for (const auto &image : images)
        if (!image)
            LOG_CRITICAL << "Image can't be nullptr";

    auto width = images[0u]->width();
    auto numComponents = images[0u]->numComponents();
    auto type = images[0u]->type();

    for (const auto &image : images)
        if ((width != image->width()) ||
            (numComponents != image->numComponents()) ||
            (type != image->type()))
            LOG_CRITICAL << "All the layers of 1D texture array must be the the same size, components count and component pixel type";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(numComponents, type);

    auto result = createEmpty(width, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0u; layer < static_cast<uint32_t>(images.size()); ++layer)
        result->setSubImage(0u, glm::uvec3(0u, layer, 0u), glm::uvec3(width, 1u, 0u), numComponents, type, images[layer]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

// Texture2DArray_4_5

Texture2DArray_4_5::Texture2DArray_4_5(uint32_t width, uint32_t height, uint32_t numLayers, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_id);
    glTextureStorage3D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height),
                                 static_cast<GLsizei>(numLayers));
}

Texture2DArray_4_5::~Texture2DArray_4_5() = default;

core::graphics::TextureType Texture2DArray_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::Type2DArray;
}

void Texture2DArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture Texture2DArray_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, baseSize.z, internalFormat(), numMipmapLevels());
}

std::shared_ptr<Texture2DArray_4_5> Texture2DArray_4_5::createEmpty(uint32_t width,
    uint32_t height,
    uint32_t numLayers,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels)
{
    if (width * height * numLayers == 0u)
        LOG_CRITICAL << "Width, height and layers count of 2D array texture can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec2(width, height)));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<Texture2DArray_4_5>(width, height, numLayers, internalFormat, numLevels);
    result->setWrapMode(core::graphics::TextureWrapMode::Repeat);
    result->setFilterMode(numLevels > 1u ? core::graphics::TextureFilterMode::Trilinear : core::graphics::TextureFilterMode::Linear);

    return result;
}

std::shared_ptr<Texture2DArray_4_5> Texture2DArray_4_5::create(const std::vector<std::shared_ptr<const utils::Image>>& images,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels,
    bool genMipmaps)
{
    if (images.empty())
        LOG_CRITICAL << "Layers count of 2D texture array can't be 0";

    for (const auto &image : images)
        if (!image)
            LOG_CRITICAL << "Image can't be nullptr";

    auto width = images[0u]->width();
    auto height = images[0u]->height();
    auto numComponents = images[0u]->numComponents();
    auto type = images[0u]->type();

    for (const auto &image : images)
        if ((width != image->width()) ||
            (height != image->height()) ||
            (numComponents != image->numComponents()) ||
            (type != image->type()))
            LOG_CRITICAL << "All the layers of 2D texture array must be the the same size, components count and component pixel type";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(numComponents, type);

    auto result = createEmpty(width, height, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0u; layer < static_cast<uint32_t>(images.size()); ++layer)
        result->setSubImage(0u, glm::uvec3(0u, 0u, layer), glm::uvec3(width, height, 1u), numComponents, type, images[layer]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

// TextureCubeArray_4_5

TextureCubeArray_4_5::TextureCubeArray_4_5(uint32_t width, uint32_t height, uint32_t numLayers, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &m_id);
    glTextureStorage3D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height),
                                 static_cast<GLsizei>(6u * numLayers));
}

TextureCubeArray_4_5::~TextureCubeArray_4_5() = default;

core::graphics::TextureType TextureCubeArray_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::TypeCubeArray;
}

uint32_t TextureCubeArray_4_5::numFaces() const
{
    CHECK_CURRENT_CONTEXT
    return 6u;
}

void TextureCubeArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture TextureCubeArray_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, baseSize.z, internalFormat(), numMipmapLevels());
}

std::shared_ptr<TextureCubeArray_4_5> TextureCubeArray_4_5::createEmpty(uint32_t width,
    uint32_t height,
    uint32_t numLayers,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels)
{
    if (width * height * numLayers == 0u)
        LOG_CRITICAL << "Width, height and layers count of cubemap array texture can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec2(width, height)));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<TextureCubeArray_4_5>(width, height, numLayers, internalFormat, numLevels);
    result->setFilterMode(numLevels > 1u ? core::graphics::TextureFilterMode::Trilinear : core::graphics::TextureFilterMode::Linear);

    return result;
}

std::shared_ptr<TextureCubeArray_4_5> TextureCubeArray_4_5::create(const std::vector<std::vector<std::shared_ptr<const utils::Image>>>& images,
    core::graphics::PixelInternalFormat internalFormat,
    uint32_t numLevels,
    bool genMipmaps)
{
    if (images.empty())
        LOG_CRITICAL << "Layers count of cubemap texture array can't be 0";

    for (const auto &layerImages : images)
    {
        if (layerImages.size() != 6u)
            LOG_CRITICAL << "Images count of cubemap texture array must be 6";
        for (const auto &image : layerImages)
            if (!image)
                LOG_CRITICAL << "Image can't be nullptr";
    }

    auto width = images[0u][0u]->width();
    auto height = images[0u][0u]->height();
    auto numComponents = images[0u][0u]->numComponents();
    auto type = images[0u][0u]->type();

    for (const auto &layerImages : images)
        for (const auto &image : layerImages)
            if ((width != image->width()) ||
                (height != image->height()) ||
                (numComponents != image->numComponents()) ||
                (type != image->type()))
                LOG_CRITICAL << "All the layers of cubemap texture array must be the the same size, components count and component pixel type";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(numComponents, type);

    auto result = createEmpty(width, height, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0u; layer < static_cast<uint32_t>(images.size()); ++layer)
        for (uint32_t face = 0; face < static_cast<uint32_t>(images.size()); ++face)
            result->setSubImage(0u, glm::uvec3(0u, 0u, 6u * layer + face), glm::uvec3(width, height, 1u), numComponents, type, images[layer][face]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

// TextureRect_4_5

TextureRect_4_5::TextureRect_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat)
    : TextureBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    glCreateTextures(GL_TEXTURE_RECTANGLE, 1, &m_id);
    glTextureStorage2D(m_id,
                                 1u,
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height));
}

TextureRect_4_5::~TextureRect_4_5() = default;

core::graphics::TextureType TextureRect_4_5::type() const
{
    CHECK_CURRENT_CONTEXT
    return core::graphics::TextureType::TypeRect;
}

void TextureRect_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT
    if (level != 0)
        LOG_CRITICAL << "Level must be 0 for TextureRect";

    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Count)
        LOG_CRITICAL << "Undefined pixel component type";

    glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

core::graphics::PTexture TextureRect_4_5::copyEmpty() const
{
    CHECK_CURRENT_CONTEXT
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, internalFormat());
}

std::shared_ptr<TextureRect_4_5> TextureRect_4_5::createEmpty(uint32_t width,
    uint32_t height,
    core::graphics::PixelInternalFormat internalFormat)
{
    if (width * height == 0u)
        LOG_CRITICAL << "Width and height can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        LOG_CRITICAL << "Undefined pixel internal format";

    auto result = std::make_shared<TextureRect_4_5>(width, height, internalFormat);
    result->setWrapMode(core::graphics::TextureWrapMode::ClampToEdge);
    result->setFilterMode(core::graphics::TextureFilterMode::Point);

    return result;
}

std::shared_ptr<TextureRect_4_5> TextureRect_4_5::create(const std::shared_ptr<const utils::Image>& image,
    core::graphics::PixelInternalFormat internalFormat)
{
    if (!image)
        LOG_CRITICAL << "Image can't be nullptr";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createEmpty(image->width(), image->height(), internalFormat);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), image->height(), 0u), image->numComponents(), image->type(), image->data());

    return result;
}

// TextureHandle_4_5

TextureHandle_4_5::TextureHandle_4_5(const core::graphics::PConstTexture& texture)
    : m_texture(texture)
{
    SAVE_CURRENT_CONTEXT

    if (auto texture_4_5 = std::dynamic_pointer_cast<const TextureBase_4_5>(m_texture); texture_4_5)
    {
        m_id = glGetTextureHandleARB(texture_4_5->id());
    }
    else
        LOG_CRITICAL << "Texture can't be nullptr";
}

TextureHandle_4_5::~TextureHandle_4_5() = default;

core::graphics::TextureHandle TextureHandle_4_5::handle() const
{
    return m_id;
}

core::graphics::PConstTexture TextureHandle_4_5::texture() const
{
    CHECK_CURRENT_CONTEXT
    return m_texture;
}

void TextureHandle_4_5::makeResident()
{
    CHECK_CURRENT_CONTEXT
    glMakeTextureHandleResidentARB(m_id);
}

void TextureHandle_4_5::doneResident()
{
    CHECK_CURRENT_CONTEXT
    glMakeTextureHandleNonResidentARB(m_id);
}

std::shared_ptr<TextureHandle_4_5> TextureHandle_4_5::create(const core::graphics::PConstTexture& texture)
{
    return std::make_shared<TextureHandle_4_5>(texture);
}

// Image_4_5

Image_4_5::Image_4_5(DataAccess access, const core::graphics::PConstTexture &texture, uint32_t level)
    : m_texture(nullptr)
    , m_level(0u)
    , m_format(core::graphics::PixelInternalFormat::Undefined)
    , m_access(DataAccess::ReadWrite)
{
    SAVE_CURRENT_CONTEXT
    setAccess(access);

    if (texture)
        setTexture(texture, level);
}

Image_4_5::~Image_4_5() = default;

core::graphics::IImage::DataAccess Image_4_5::access() const
{
    CHECK_CURRENT_CONTEXT
    return m_access;
}

void Image_4_5::setAccess(DataAccess value)
{
    CHECK_CURRENT_CONTEXT
    m_access = value;
}

uint32_t Image_4_5::mipmapLevel() const
{
    CHECK_CURRENT_CONTEXT
    return m_level;
}

core::graphics::PixelInternalFormat Image_4_5::format() const
{
    CHECK_CURRENT_CONTEXT
    return m_format;
}

core::graphics::PConstTexture Image_4_5::texture() const
{
    CHECK_CURRENT_CONTEXT
    return m_texture;
}

void Image_4_5::setTexture(const core::graphics::PConstTexture &texture, uint32_t level)
{
    CHECK_CURRENT_CONTEXT
    if (!texture)
        LOG_CRITICAL << "Texture can't be nullptr";

    if (level >= texture->numMipmapLevels())
        LOG_CRITICAL << "Level must be less than texture levels count";

    const auto &supportedFormats = supportedImageFormats();

    const auto internalFormat = texture->internalFormat();
    if (supportedFormats.find(internalFormat) == supportedFormats.end())
        LOG_CRITICAL << "Unsupported image format";

    m_texture = texture;
    m_format = internalFormat;
    m_level = level;
}

const core::graphics::SupportedImageFormats& Image_4_5::supportedImageFormats()
{
    static const core::graphics::SupportedImageFormats s_supportdFormats{
        { core::graphics::PixelInternalFormat::RGBA32F, "rgba32f" },
        { core::graphics::PixelInternalFormat::RGBA16F, "rgba16f" },
        { core::graphics::PixelInternalFormat::RG32F, "rg32f" },
        { core::graphics::PixelInternalFormat::RG16F, "rg16f" },
        { core::graphics::PixelInternalFormat::R11F_G11F_B10F, "r11f_g11f_b10f" },
        { core::graphics::PixelInternalFormat::R32F, "r32f" },
        { core::graphics::PixelInternalFormat::R16F, "r16f" },
        { core::graphics::PixelInternalFormat::RGBA32UI, "rgba32ui" },
        { core::graphics::PixelInternalFormat::RGBA16UI, "rgba16ui" },
        { core::graphics::PixelInternalFormat::RGB10_A2UI, "rgb10a2ui" },
        { core::graphics::PixelInternalFormat::RGBA8UI, "rgba8ui" },
        { core::graphics::PixelInternalFormat::RG32UI, "rg32ui" },
        { core::graphics::PixelInternalFormat::RG16UI, "rg16ui" },
        { core::graphics::PixelInternalFormat::RG8UI, "rg8ui" },
        { core::graphics::PixelInternalFormat::R32UI, "r32ui" },
        { core::graphics::PixelInternalFormat::R16UI, "r18ui" },
        { core::graphics::PixelInternalFormat::R8UI, "r8ui" },
        { core::graphics::PixelInternalFormat::RGBA32I, "rgba32i" },
        { core::graphics::PixelInternalFormat::RGBA16I, "rgba16i" },
        { core::graphics::PixelInternalFormat::RGBA8I, "rgba8i" },
        { core::graphics::PixelInternalFormat::RG32I, "rg32i" },
        { core::graphics::PixelInternalFormat::RG16I, "rg16i" },
        { core::graphics::PixelInternalFormat::RG8I, "rg8i" },
        { core::graphics::PixelInternalFormat::R32I, "rg32i" },
        { core::graphics::PixelInternalFormat::R16I, "rg16i" },
        { core::graphics::PixelInternalFormat::R8I, "r8i" },
        { core::graphics::PixelInternalFormat::RGBA16, "rgba16" },
        { core::graphics::PixelInternalFormat::RGB10_A2, "rgb10a2" },
        { core::graphics::PixelInternalFormat::RGBA8, "rgba8" },
        { core::graphics::PixelInternalFormat::RG16, "rg16" },
        { core::graphics::PixelInternalFormat::RG8, "rg8" },
        { core::graphics::PixelInternalFormat::R16, "r16" },
        { core::graphics::PixelInternalFormat::R8, "r8" },
        { core::graphics::PixelInternalFormat::RGBA16_SNORM, "rgba16_snorm" },
        { core::graphics::PixelInternalFormat::RGBA8_SNORM, "rgba8_snorm" },
        { core::graphics::PixelInternalFormat::RG16_SNORM, "rg16_snorm" },
        { core::graphics::PixelInternalFormat::RG8_SNORM, "rg8_snorm" },
        { core::graphics::PixelInternalFormat::R16_SNORM, "r16_snorm" },
        { core::graphics::PixelInternalFormat::R8_SNORM, "r8_snorm" }
    };

    return s_supportdFormats;
}

std::shared_ptr<Image_4_5> Image_4_5::create(core::graphics::IImage::DataAccess access,
    const core::graphics::PConstTexture& texture,
    uint32_t level)
{
    return std::make_shared<Image_4_5>(access, texture, level);
}

// RenderBuffer_4_5

RenderBuffer_4_5::RenderBuffer_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat)
{
    SAVE_CURRENT_CONTEXT
    glCreateRenderbuffers(1, &m_id);
    glNamedRenderbufferStorage(m_id,
                                         Conversions::PixelInternalFormat2GL(internalFormat),
                                         static_cast<GLsizei>(width),
                                         static_cast<GLsizei>(height));
}

RenderBuffer_4_5::~RenderBuffer_4_5()
{
    CHECK_CURRENT_CONTEXT
    glDeleteRenderbuffers(1, &m_id);
}

GLuint RenderBuffer_4_5::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

glm::uvec2 RenderBuffer_4_5::size() const
{
    CHECK_CURRENT_CONTEXT
    GLint width, height;
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_WIDTH, &width);
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_HEIGHT, &height);

    return glm::uvec2(width, height);
}

core::graphics::PixelInternalFormat RenderBuffer_4_5::internalFormat() const
{
    CHECK_CURRENT_CONTEXT
    GLint result;
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_INTERNAL_FORMAT, &result);

    return Conversions::GL2PixelInternalFormat(static_cast<GLenum>(result));
}

bool RenderBuffer_4_5::hasAlpha() const
{
    CHECK_CURRENT_CONTEXT
    GLint result;
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_ALPHA_SIZE, &result);

    return result != 0;
}

bool RenderBuffer_4_5::hasDepth() const
{
    CHECK_CURRENT_CONTEXT
    GLint result;
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_DEPTH_SIZE, &result);

    return result != 0;
}

std::shared_ptr<RenderBuffer_4_5> RenderBuffer_4_5::create(uint32_t width,
    uint32_t height,
    core::graphics::PixelInternalFormat internalFormat)
{
    return std::make_shared<RenderBuffer_4_5>(width, height, internalFormat);
}

// FrameBuffer_4_5

FrameBufferBase_4_5::FrameBufferBase_4_5(GLuint id)
    : m_id(id)
{
    SAVE_CURRENT_CONTEXT
    for (uint32_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
        setClearColor(i, glm::vec4(.5f, .5f, 1.f, 1.f));

    setClearDepthStencil(1.f, 0x00u);

    setFaceCulling(false);

    setColorMasks(true);

    setDepthTest(true);
    setDepthMask(true);

    setStencilTest(false);

    setBlending(false);
    setBlendConstantColor(glm::vec3(1.f));
    setBlendConstantAlpha(1.f);
    for (uint32_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    {
        setBlendEquation(i, core::graphics::BlendEquation::Add, core::graphics::BlendEquation::Add);
        setBlendFactor(i,
                       core::graphics::BlendFactor::Zero,
                       core::graphics::BlendFactor::One,
                       core::graphics::BlendFactor::Zero,
                       core::graphics::BlendFactor::One);
    }
}

FrameBufferBase_4_5::~FrameBufferBase_4_5() = default;

GLuint FrameBufferBase_4_5::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

void FrameBufferBase_4_5::clear()
{
    CHECK_CURRENT_CONTEXT
    bool depthStensilWasCleared = false;
    for (auto attachment : m_clearMask)
    {
        if (core::graphics::IsFrameBufferColorAttachment(attachment) && m_attachments.count(attachment))
        {
            auto drawBuffer = core::graphics::FrameBufferColorAttachmentIndex(attachment);
            const auto &clearColor = m_clearColor[drawBuffer];
            switch (clearColor.first)
            {
            case core::graphics::FrameBufferClearColorType::Single: {
                glClearNamedFramebufferfv(m_id,
                                                    GL_COLOR,
                                                    static_cast<GLint>(drawBuffer),
                                                    glm::value_ptr(clearColor.second.floatColor));
                break;
            }
            case core::graphics::FrameBufferClearColorType::Int32: {
                glClearNamedFramebufferiv(m_id,
                                                    GL_COLOR,
                                                    static_cast<GLint>(drawBuffer),
                                                    glm::value_ptr(clearColor.second.intColor));
                break;
            }
            case core::graphics::FrameBufferClearColorType::Uint32: {
                glClearNamedFramebufferuiv(m_id,
                                                     GL_COLOR,
                                                     static_cast<GLint>(drawBuffer),
                                                     glm::value_ptr(clearColor.second.uintColor));
                break;
            }
            default:
                break;
            }
            continue;
        }

        if ((attachment == core::graphics::FrameBufferAttachment::Depth) ||
            (attachment == core::graphics::FrameBufferAttachment::DepthStencil))
        {
            if (m_attachments.count(core::graphics::FrameBufferAttachment::DepthStencil))
            {
                if (!depthStensilWasCleared)
                {
                    glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);
                }
                depthStensilWasCleared = true;
            }
            else if (m_attachments.count(core::graphics::FrameBufferAttachment::Depth))
            {
                glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &m_clearDepth);
            }
            continue;
        }

        if ((attachment == core::graphics::FrameBufferAttachment::Stencil) ||
            (attachment == core::graphics::FrameBufferAttachment::DepthStencil))
        {
            if (m_attachments.count(core::graphics::FrameBufferAttachment::DepthStencil))
            {
                if (!depthStensilWasCleared)
                {
                    glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);
                }
                depthStensilWasCleared = true;
            }
            else if (m_attachments.count(core::graphics::FrameBufferAttachment::Stencil))
            {
                glClearNamedFramebufferiv(m_id, GL_STENCIL, 0, &m_clearStencil);
            }
            continue;
        }
    }
}

bool FrameBufferBase_4_5::isComplete() const
{
    CHECK_CURRENT_CONTEXT
    return glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

const core::graphics::IFrameBuffer::Attachments &FrameBufferBase_4_5::attachments() const
{
    CHECK_CURRENT_CONTEXT
    return m_attachments;
}

bool FrameBufferBase_4_5::attachment(core::graphics::FrameBufferAttachment value, AttachmentInfo &result) const
{
    CHECK_CURRENT_CONTEXT
    auto it = m_attachments.find(value);
    if (it == m_attachments.end())
        return false;

    result = it->second;
    return true;
}

const core::graphics::FrameBufferClearColor &FrameBufferBase_4_5::clearColor(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    return m_clearColor[index];
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::vec4 &value)
{
    CHECK_CURRENT_CONTEXT
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.floatColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Single, clearColorValue};
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::i32vec4 &value)
{
    CHECK_CURRENT_CONTEXT
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.intColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Int32, clearColorValue };
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::u32vec4 &value)
{
    CHECK_CURRENT_CONTEXT
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.uintColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Uint32, clearColorValue };
}

float FrameBufferBase_4_5::clearDepth() const
{
    CHECK_CURRENT_CONTEXT
    return m_clearDepth;
}

int32_t FrameBufferBase_4_5::clearStencil() const
{
    CHECK_CURRENT_CONTEXT
    return m_clearStencil;
}

void FrameBufferBase_4_5::setClearDepthStencil(float depth, uint8_t stencil)
{
    CHECK_CURRENT_CONTEXT
    m_clearDepth = depth;
    m_clearStencil = stencil;
}

const std::unordered_set<core::graphics::FrameBufferAttachment> &FrameBufferBase_4_5::clearMask() const
{
    CHECK_CURRENT_CONTEXT
    return m_clearMask;
}

void FrameBufferBase_4_5::setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment> &value)
{
    CHECK_CURRENT_CONTEXT
    m_clearMask = value;
}

void FrameBufferBase_4_5::setDrawBuffers(const std::vector<core::graphics::FrameBufferAttachment> &attachments)
{
    CHECK_CURRENT_CONTEXT
    std::vector<GLenum> buffers;
    for (const auto &attachment : attachments)
    {
        if (attachment == core::graphics::FrameBufferAttachment::Count)
            buffers.push_back(GL_NONE);
        else
        {
            if (!core::graphics::IsFrameBufferColorAttachment(attachment))
                LOG_CRITICAL << "Not color attachment can be draw buffer";

            buffers.push_back(GL_COLOR_ATTACHMENT0 + core::graphics::FrameBufferColorAttachmentIndex(attachment));
        }
    }

    glNamedFramebufferDrawBuffers(m_id,
                                            static_cast<GLsizei>(buffers.size()),
                                            buffers.data());
}

bool FrameBufferBase_4_5::faceCulling() const
{
    CHECK_CURRENT_CONTEXT
    return m_faceCulling;
}

core::graphics::FaceType FrameBufferBase_4_5::cullFaceType() const
{
    CHECK_CURRENT_CONTEXT
    return m_cullFaceType;
}

void FrameBufferBase_4_5::setFaceCulling(bool value, core::graphics::FaceType type)
{
    CHECK_CURRENT_CONTEXT
    m_faceCulling = value;
    m_cullFaceType = type;
}

bool FrameBufferBase_4_5::colorMask(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    return m_colorMasks[index];
}

void FrameBufferBase_4_5::setColorMask(uint32_t index, bool value)
{
    CHECK_CURRENT_CONTEXT
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    m_colorMasks[index] = value;
}

void FrameBufferBase_4_5::setColorMasks(bool value)
{
    CHECK_CURRENT_CONTEXT
    for (auto &mask : m_colorMasks)
        mask = value;
}

bool FrameBufferBase_4_5::depthTest() const
{
    CHECK_CURRENT_CONTEXT
    return m_depthTest;
}

core::graphics::ComparingFunc FrameBufferBase_4_5::depthFunc() const
{
    CHECK_CURRENT_CONTEXT
    return m_depthFunc;
}

void FrameBufferBase_4_5::setDepthTest(bool value, core::graphics::ComparingFunc func)
{
    CHECK_CURRENT_CONTEXT
    m_depthTest = value;
    m_depthFunc = func;
}

bool FrameBufferBase_4_5::depthMask() const
{
    CHECK_CURRENT_CONTEXT
    return m_depthMask;
}

void FrameBufferBase_4_5::setDepthMask(bool value)
{
    CHECK_CURRENT_CONTEXT
    m_depthMask = value;
}

bool FrameBufferBase_4_5::stencilTest() const
{
    CHECK_CURRENT_CONTEXT
    return m_stencilTest;
}

void FrameBufferBase_4_5::setStencilTest(bool value)
{
    CHECK_CURRENT_CONTEXT
    m_stencilTest = value;
}

core::graphics::ComparingFunc FrameBufferBase_4_5::stencilComparingFunc(core::graphics::FaceType value) const
{
    CHECK_CURRENT_CONTEXT
    auto result = core::graphics::ComparingFunc::Always;

    switch (value)
    {
    case core::graphics::FaceType::Front:
        result = m_stencilComparingFuncFrontFace;
        break;
    case core::graphics::FaceType::Back:
        result = m_stencilComparingFuncBackFace;
        break;
    default:
        LOG_CRITICAL << "Face type must be Front or Back";
        break;
    }

    return result;
}

uint8_t FrameBufferBase_4_5::stencilReferenceValue(core::graphics::FaceType value) const
{
    CHECK_CURRENT_CONTEXT
    uint8_t result = 0x00u;

    switch (value)
    {
    case core::graphics::FaceType::Front:
        result = m_stencilRefFrontFace;
        break;
    case core::graphics::FaceType::Back:
        result = m_stencilRefBackFace;
        break;
    default:
        LOG_CRITICAL << "Face type must be Front or Back";
        break;
    }

    return result;
}

uint8_t FrameBufferBase_4_5::stencilMaskValue(core::graphics::FaceType value) const
{
    CHECK_CURRENT_CONTEXT
    uint8_t result = 0x00u;

    switch (value)
    {
    case core::graphics::FaceType::Front:
        result = m_stencilMaskFrontFace;
        break;
    case core::graphics::FaceType::Back:
        result = m_stencilMaskBackFace;
        break;
    default:
        LOG_CRITICAL << "Face type must be Front or Back";
        break;
    }

    return result;
}

void FrameBufferBase_4_5::setStencilFunc(core::graphics::FaceType face, core::graphics::ComparingFunc func, uint8_t ref, uint8_t mask)
{
    CHECK_CURRENT_CONTEXT
    switch (face)
    {
    case core::graphics::FaceType::Front:
        m_stencilComparingFuncFrontFace = func;
        m_stencilRefFrontFace = ref;
        m_stencilMaskFrontFace = mask;
        break;
    case core::graphics::FaceType::Back:
        m_stencilComparingFuncBackFace = func;
        m_stencilRefBackFace = ref;
        m_stencilMaskBackFace = mask;
        break;
    case core::graphics::FaceType::FrontAndBack:
        m_stencilComparingFuncFrontFace = m_stencilComparingFuncBackFace = func;
        m_stencilRefFrontFace = m_stencilRefBackFace = ref;
        m_stencilMaskFrontFace = m_stencilMaskBackFace = mask;
        break;
    default:
        LOG_CRITICAL << "Face type must be Front, Back or FrontAndBack";
        break;
    }
}

const core::graphics::StencilOperations &FrameBufferBase_4_5::stencilOperations(core::graphics::FaceType value) const
{
    CHECK_CURRENT_CONTEXT
    static core::graphics::StencilOperations errorResult;

    switch (value)
    {
    case core::graphics::FaceType::Front:
        return m_stencilOperationsFrontFace;
    case core::graphics::FaceType::Back:
        return m_stencilOperationsBackFace;
    default:
        LOG_CRITICAL << "Face type must be Front or Back";
        break;
    }

    return errorResult;
}

void FrameBufferBase_4_5::setStencilOperations(core::graphics::FaceType face, const core::graphics::StencilOperations &value)
{
    CHECK_CURRENT_CONTEXT
    switch (face)
    {
    case core::graphics::FaceType::Front:
        m_stencilOperationsFrontFace = value;
        break;
    case core::graphics::FaceType::Back:
        m_stencilOperationsBackFace = value;
        break;
    case core::graphics::FaceType::FrontAndBack:
        m_stencilOperationsFrontFace = m_stencilOperationsBackFace = value;
        break;
    default:
        LOG_CRITICAL << "Face type must be Front, Back or FrontAndBack";
        break;
    }
}

bool FrameBufferBase_4_5::blending() const
{
    CHECK_CURRENT_CONTEXT
    return m_blending;
}

void FrameBufferBase_4_5::setBlending(bool value)
{
    CHECK_CURRENT_CONTEXT
    m_blending = value;
}

core::graphics::BlendEquation FrameBufferBase_4_5::blendColorEquation(uint32_t index)
{
    CHECK_CURRENT_CONTEXT
    return m_blendColorEquation[index];
}

core::graphics::BlendEquation FrameBufferBase_4_5::blendAlphaEquation(uint32_t index)
{
    CHECK_CURRENT_CONTEXT
    return m_blendAlphaEquation[index];
}

void FrameBufferBase_4_5::setBlendEquation(uint32_t index, core::graphics::BlendEquation colorValue, core::graphics::BlendEquation alphaValue)
{
    CHECK_CURRENT_CONTEXT
    m_blendColorEquation[index] = colorValue;
    m_blendAlphaEquation[index] = alphaValue;
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendColorSourceFactor(uint32_t index)
{
    CHECK_CURRENT_CONTEXT
    return m_blendColorSourceFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendAlphaSourceFactor(uint32_t index)
{
    CHECK_CURRENT_CONTEXT
    return m_blendAlphaSourceFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendColorDestinationFactor(uint32_t index)
{
    CHECK_CURRENT_CONTEXT
    return m_blendColorDestFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendAlphaDestinationFactor(uint32_t index)
{
    CHECK_CURRENT_CONTEXT
    return m_blendAlphaDestFactor[index];
}

void FrameBufferBase_4_5::setBlendFactor(uint32_t index,
                                         core::graphics::BlendFactor colorSourceValue,
                                         core::graphics::BlendFactor colorDestValue,
                                         core::graphics::BlendFactor alphaSourceValue,
                                         core::graphics::BlendFactor alphaDestValue)
{
    CHECK_CURRENT_CONTEXT
    m_blendColorSourceFactor[index] = colorSourceValue;
    m_blendColorDestFactor[index] = colorDestValue;
    m_blendAlphaSourceFactor[index] = alphaSourceValue;
    m_blendAlphaDestFactor[index] = alphaDestValue;
}

glm::vec3 FrameBufferBase_4_5::blendConstantColor() const
{
    CHECK_CURRENT_CONTEXT
    return m_blendConstColor;
}

void FrameBufferBase_4_5::setBlendConstantColor(const glm::vec3 &value)
{
    CHECK_CURRENT_CONTEXT
    m_blendConstColor = value;
}

float FrameBufferBase_4_5::blendConstantAlpha() const
{
    CHECK_CURRENT_CONTEXT
    return m_blendConstAlpha;
}

void FrameBufferBase_4_5::setBlendConstantAlpha(float value)
{
    CHECK_CURRENT_CONTEXT
    m_blendConstAlpha = value;
}

// FrameBuffer_4_5

FrameBuffer_4_5::FrameBuffer_4_5()
    : FrameBufferBase_4_5(0u)
{
    SAVE_CURRENT_CONTEXT
    glCreateFramebuffers(1, &m_id);
}

FrameBuffer_4_5::~FrameBuffer_4_5()
{
    CHECK_CURRENT_CONTEXT
    glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer_4_5::attach(core::graphics::FrameBufferAttachment key,
                             std::shared_ptr<const core::graphics::ISurface> surface,
                             uint32_t level)
{
    CHECK_CURRENT_CONTEXT
    detach(key);

    if (auto texture_4_5 = std::dynamic_pointer_cast<const TextureBase_4_5>(surface); texture_4_5)
    {
        glNamedFramebufferTexture(m_id, Conversions::FrameBufferAttachment2GL(key), texture_4_5->id(), static_cast<GLint>(level));
        m_attachments[key] = { surface, level, 0u };
    }
    else if (auto renderBuffer_4_5 = std::dynamic_pointer_cast<const RenderBuffer_4_5>(surface); renderBuffer_4_5)
    {
        glNamedFramebufferRenderbuffer(m_id,
                                                 Conversions::FrameBufferAttachment2GL(key),
                                                 GL_RENDERBUFFER,
                                                 renderBuffer_4_5->id());
        m_attachments[key] = { surface, level, 0u };
    }
    else
        LOG_CRITICAL << "Attachment can't be nullptr";
}

void FrameBuffer_4_5::attachLayer(core::graphics::FrameBufferAttachment key,
                                  std::shared_ptr<const core::graphics::ITexture> texture,
                                  uint32_t level,
                                  uint32_t layer)
{
    CHECK_CURRENT_CONTEXT
    detach(key);

    if (auto texture_4_5 = std::dynamic_pointer_cast<const TextureBase_4_5>(texture); texture_4_5)
    {
        glNamedFramebufferTextureLayer(m_id,
                                                 Conversions::FrameBufferAttachment2GL(key),
                                                 texture_4_5->id(),
                                                 static_cast<GLint>(level),
                                                 static_cast<GLint>(layer));
        m_attachments[key] = { texture, level, layer };
    }
    else
        LOG_CRITICAL << "Attachment can't be nullptr";
}

void FrameBuffer_4_5::detach(core::graphics::FrameBufferAttachment key)
{
    CHECK_CURRENT_CONTEXT
    if (auto it = m_attachments.find(key); it != m_attachments.end())
    {
        glNamedFramebufferRenderbuffer(m_id,
                                                 Conversions::FrameBufferAttachment2GL(key),
                                                 GL_RENDERBUFFER,
                                                 0u);
        m_attachments.erase(it);
    }
}

std::shared_ptr<FrameBuffer_4_5> FrameBuffer_4_5::create()
{
    return std::make_shared<FrameBuffer_4_5>();
}

// DefaultFrameBuffer_4_5

DefaultFrameBuffer_4_5::DefaultFrameBuffer_4_5(GLuint defaultFbo)
    : FrameBufferBase_4_5(defaultFbo)
{
    SAVE_CURRENT_CONTEXT

    //GLint objType = GL_NONE;
    //glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    //if (objType != GL_NONE)
    //    m_attachments[core::graphics::FrameBufferAttachment::DepthStencil] = {nullptr, 0, 0};
    //else
    //{
    //    objType = GL_NONE;
    //    glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    //    if (objType != GL_NONE)
    //        m_attachments[core::graphics::FrameBufferAttachment::Depth] = {nullptr, 0, 0};

    //    objType = GL_NONE;
    //    glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    //    if (objType != GL_NONE)
    //        m_attachments[core::graphics::FrameBufferAttachment::Stencil] = {nullptr, 0, 0};
    //}

    //std::vector<GLenum> drawBuffers;
    //for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    //{
    //    objType = GL_NONE;
    //    glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    //    if (objType != GL_NONE)
    //    {
    //        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
    //        m_attachments[core::graphics::FrameBufferColorAttachment(i)] = {nullptr, 0, 0};
    //    }
    //}
    //glNamedFramebufferDrawBuffers(defaultFbo, static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());

    m_attachments[core::graphics::FrameBufferAttachment::Depth] = {nullptr, 0, 0};
    m_attachments[core::graphics::FrameBufferAttachment::Stencil] = {nullptr, 0, 0};
    m_attachments[core::graphics::FrameBufferColorAttachment(0)] = {nullptr, 0, 0};
}

DefaultFrameBuffer_4_5::~DefaultFrameBuffer_4_5() = default;

void DefaultFrameBuffer_4_5::attach(core::graphics::FrameBufferAttachment, std::shared_ptr<const core::graphics::ISurface>, uint32_t)
{
    CHECK_CURRENT_CONTEXT
}

void DefaultFrameBuffer_4_5::attachLayer(core::graphics::FrameBufferAttachment, std::shared_ptr<const core::graphics::ITexture>, uint32_t, uint32_t)
{
    CHECK_CURRENT_CONTEXT
}

void DefaultFrameBuffer_4_5::detach(core::graphics::FrameBufferAttachment)
{
    CHECK_CURRENT_CONTEXT
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::vec4&)
{
    CHECK_CURRENT_CONTEXT
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::i32vec4&)
{
    CHECK_CURRENT_CONTEXT
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::u32vec4&)
{
    CHECK_CURRENT_CONTEXT
}

void DefaultFrameBuffer_4_5::setClearDepthStencil(float, uint8_t)
{
    CHECK_CURRENT_CONTEXT
}

void DefaultFrameBuffer_4_5::setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment>&)
{
    CHECK_CURRENT_CONTEXT
}

std::shared_ptr<DefaultFrameBuffer_4_5> DefaultFrameBuffer_4_5::create(GLuint id)
{
    return std::make_shared<DefaultFrameBuffer_4_5>(id);
}

// ProgramBase_4_5

ProgramBase_4_5::ProgramBase_4_5()
{
    SAVE_CURRENT_CONTEXT
    m_id = glCreateProgram();
}

ProgramBase_4_5::~ProgramBase_4_5()
{
    CHECK_CURRENT_CONTEXT
    GLint count;
    glGetProgramiv(m_id, GL_ATTACHED_SHADERS, &count);

    std::vector<GLuint> shaders(static_cast<size_t>(count));
    GLsizei resCount;

    glGetAttachedShaders(m_id, count, &resCount, shaders.data());

    for (GLsizei i = 0; i < resCount; ++i)
    {
        GLuint shaderId = shaders[static_cast<size_t>(i)];
        glDetachShader(m_id, shaderId);
        glDeleteShader(shaderId);
    }

    glDeleteProgram(m_id);
}

bool ProgramBase_4_5::compileAndLink(const std::unordered_map<GLenum, std::reference_wrapper<const std::string>>& shadersData)
{
    CHECK_CURRENT_CONTEXT
    static const std::unordered_map<GLenum, std::string> s_shaderTypesTable{
        {GL_COMPUTE_SHADER, "Compute"},
        {GL_VERTEX_SHADER, "Vertex"},
        {GL_TESS_CONTROL_SHADER, "TessControl"},
        {GL_TESS_EVALUATION_SHADER, "TessEvaluation"},
        {GL_GEOMETRY_SHADER, "Geometry"},
        {GL_FRAGMENT_SHADER, "Fragment"}
    };

    if (!m_id)
        LOG_CRITICAL << "Program can't be nullptr";

    std::vector<GLuint> shadersIds;
    bool isOk = true;

    for (auto const& [type, shader] : shadersData)
    {
        const char* data = shader.get().c_str();

        auto id = glCreateShader(type);
        glShaderSource(id, 1, &data, nullptr);
        glCompileShader(id);
        GLint compiled;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1)
            {
                char* infoLog = static_cast<char*>(std::malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
                glGetShaderInfoLog(id, infoLen, nullptr, infoLog);
                LOG_ERROR << s_shaderTypesTable.at(type) << " shader compile error: " << infoLog;
                std::free(infoLog);

                LOG_INFO << s_shaderTypesTable.at(type) << " shader:";
                LOG_INFO << data;
            }
            isOk = false;
        }
        shadersIds.push_back(id);
    }

    if (isOk)
    {
        for (const auto& shaderId : shadersIds)
            glAttachShader(m_id, shaderId);
    }

    if (isOk)
    {
        std::string logString;
        if (!preBuild(logString))
        {
            isOk = false;
            LOG_ERROR << "Program pre build: " << logString;
        }
    }

    if (isOk)
    {
        glLinkProgram(m_id);
        GLint linked;
        glGetProgramiv(m_id, GL_LINK_STATUS, &linked);
        if (!linked) {
            GLint infoLen = 0;
            glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* infoLog = static_cast<char*>(std::malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
                glGetProgramInfoLog(m_id, infoLen, nullptr, infoLog);
                LOG_ERROR << "Program link error: " << infoLog;
                std::free(infoLog);
            }
            isOk = false;
        }
    }

    if (isOk)
    {
        std::string logString;
        if (!postBuild(logString))
        {
            isOk = false;
            LOG_ERROR << "Program post build: " << logString;
        }
    }

    if (!isOk)
    {
        for (const auto& shaderId : shadersIds)
            if (glIsShader(shaderId) == GL_TRUE)
                glDeleteShader(shaderId);
    }

    return isOk;
}

GLuint ProgramBase_4_5::id() const
{
    CHECK_CURRENT_CONTEXT
    return m_id;
}

bool ProgramBase_4_5::preBuild(std::string &)
{
    CHECK_CURRENT_CONTEXT
    return true;
}

bool ProgramBase_4_5::postBuild(std::string &)
{
    CHECK_CURRENT_CONTEXT
    auto currentRenderer = core::graphics::RendererBase::current();
    if (!currentRenderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    GLint numUniforms;
    glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    m_uniformsInfo.reserve(static_cast<size_t>(numUniforms));

    glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_MAX_NAME_LENGTH, &m_uniformNameMaxLength);
    std::vector<GLchar> uniformName(static_cast<size_t>(m_uniformNameMaxLength));

    for (GLuint uniformIndex = 0; uniformIndex < static_cast<GLuint>(numUniforms); ++uniformIndex)
    {
        static const std::array<GLenum, 3> uniformProperties {GL_BLOCK_INDEX, GL_TYPE, GL_LOCATION};
        std::array<GLint, 3> uniformValues {};

        glGetProgramResourceiv(m_id,
                                         GL_UNIFORM,
                                         uniformIndex,
                                         uniformProperties.size(),
                                         uniformProperties.data(),
                                         uniformValues.size(),
                                         nullptr,
                                         uniformValues.data());

        if (uniformValues[0] != -1) // Skip any uniforms that are in a block
            continue;

        if (uniformValues[1] == GL_UNSIGNED_INT_ATOMIC_COUNTER) // get atomic counter binding point instead of location
        {
            GLint atomicCounterBufferIndex;
            glGetActiveUniformsiv(m_id,
                                            1u,
                                            &uniformIndex,
                                            GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX,
                                            &atomicCounterBufferIndex);

            glGetActiveAtomicCounterBufferiv(m_id,
                                                       static_cast<GLuint>(atomicCounterBufferIndex),
                                                       GL_ATOMIC_COUNTER_BUFFER_BINDING,
                                                       uniformValues.data() + 2u);
        }

        glGetProgramResourceName(m_id,
                                           GL_UNIFORM,
                                           uniformIndex,
                                           m_uniformNameMaxLength,
                                           nullptr,
                                           uniformName.data());

        m_uniformsInfo.push_back({ currentRenderer->uniformIdByName(uniformName.data()),
                                   static_cast<uint16_t>(uniformIndex),
                                   uniformValues[2u],
                                   Conversions::GL2UniformType(static_cast<GLenum>(uniformValues[1u])) });
    }

    GLint numSSBOs;
    glGetProgramInterfaceiv(m_id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numSSBOs);
    m_SSBOsInfo.reserve(static_cast<size_t>(numSSBOs));

    glGetProgramInterfaceiv(m_id, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &m_SSBONameMaxLength);
    std::vector<GLchar> SSBOName(static_cast<size_t>(m_SSBONameMaxLength));

    glGetProgramInterfaceiv(m_id, GL_BUFFER_VARIABLE, GL_MAX_NAME_LENGTH, &m_bufferVariableNameMaxLength);

    for (GLuint SSBOIndex = 0; SSBOIndex < static_cast<GLuint>(numSSBOs); ++SSBOIndex)
    {
        static const std::array<GLenum, 1> numVariablesProperty {GL_NUM_ACTIVE_VARIABLES};
        GLint numVariables;

        glGetProgramResourceiv(m_id,
                                         GL_SHADER_STORAGE_BLOCK,
                                         SSBOIndex,
                                         numVariablesProperty.size(),
                                         numVariablesProperty.data(),
                                         1u,
                                         nullptr,
                                         &numVariables);

        static const std::array<GLenum, 1> variablesIndicesProperty {GL_ACTIVE_VARIABLES};
        std::vector<GLint> variablesIndices(static_cast<GLuint>(numVariables));

        glGetProgramResourceiv(m_id,
                                         GL_SHADER_STORAGE_BLOCK,
                                         SSBOIndex,
                                         variablesIndicesProperty.size(),
                                         variablesIndicesProperty.data(),
                                         static_cast<GLsizei>(variablesIndices.size()),
                                         nullptr,
                                         variablesIndices.data());

        std::vector<core::graphics::SSBOVariableInfo> variables;
        variables.reserve(static_cast<size_t>(numVariables));

        for (auto varIndex : variablesIndices)
        {
            static const std::array<GLenum, 2u> variableProperties {GL_TYPE, GL_OFFSET};
            std::array<GLint, 2u> uniformValues {};

            glGetProgramResourceiv(m_id,
                                             GL_BUFFER_VARIABLE,
                                             static_cast<GLuint>(varIndex),
                                             variableProperties.size(),
                                             variableProperties.data(),
                                             static_cast<GLsizei>(uniformValues.size()),
                                             nullptr,
                                             uniformValues.data());

            variables.push_back({ static_cast<uint16_t>(varIndex),
//                                  Conversions::GL2SSBOVariableType(static_cast<GLenum>(uniformValues[0u])),
                                  static_cast<uint16_t>(uniformValues[1u]) });
        }


        glGetProgramResourceName(m_id,
                                           GL_SHADER_STORAGE_BLOCK,
                                           SSBOIndex,
                                           m_SSBONameMaxLength,
                                           nullptr,
                                           SSBOName.data());

        m_SSBOsInfo.push_back({ currentRenderer->SSBOIdByName(SSBOName.data()),
                                static_cast<uint16_t>(SSBOIndex),
                                std::move(variables) });
    }

    return true;
}

int32_t ProgramBase_4_5::uniformLocationByName(const std::string &value) const
{
    CHECK_CURRENT_CONTEXT
    return glGetProgramResourceLocation(m_id, GL_UNIFORM, value.data());
}

const std::vector<core::graphics::UniformInfo> &ProgramBase_4_5::uniformsInfo() const
{
    CHECK_CURRENT_CONTEXT
    return m_uniformsInfo;
}

const std::vector<core::graphics::SSBOInfo> &ProgramBase_4_5::SSBOsInfo() const
{
    CHECK_CURRENT_CONTEXT
    return m_SSBOsInfo;
}

std::string ProgramBase_4_5::uniformNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT
    std::vector<GLchar> name(static_cast<size_t>(m_uniformNameMaxLength));
    glGetProgramResourceName(m_id,
                                       GL_UNIFORM,
                                       static_cast<GLuint>(index),
                                       m_uniformNameMaxLength,
                                       nullptr,
                                       name.data());

    return name.data();
}

std::string ProgramBase_4_5::SSBOVariableNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT
    std::vector<GLchar> name(static_cast<size_t>(m_uniformNameMaxLength));
    glGetProgramResourceName(m_id,
                                       GL_BUFFER_VARIABLE,
                                       static_cast<GLuint>(index),
                                       m_SSBONameMaxLength,
                                       nullptr,
                                       name.data());

    return name.data();
}

std::string ProgramBase_4_5::SSBONameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT
    std::vector<GLchar> name(static_cast<size_t>(m_SSBONameMaxLength));
    glGetProgramResourceName(m_id,
                                       GL_SHADER_STORAGE_BLOCK,
                                       static_cast<GLuint>(index),
                                       m_bufferVariableNameMaxLength,
                                       nullptr,
                                       name.data());

    return name.data();
}

// RenderProgram_4_5

RenderProgram_4_5::RenderProgram_4_5()
    : ProgramBase_4_5()
{
    SAVE_CURRENT_CONTEXT
}

RenderProgram_4_5::~RenderProgram_4_5() = default;


bool RenderProgram_4_5::postBuild(std::string &log)
{
    CHECK_CURRENT_CONTEXT
    auto currentRenderer = core::graphics::RendererBase::current();
    if (!currentRenderer)
        LOG_CRITICAL << "Graphics renderer can't be nullptr";

    GLint numAttributes;
    glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttributes);
    m_attributesInfo.reserve(static_cast<size_t>(numAttributes));

    glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_MAX_NAME_LENGTH, &m_attributeNameMaxLength);
    std::vector<GLchar> attributeName(static_cast<size_t>(m_attributeNameMaxLength));

    for (GLuint i = 0; i < static_cast<GLuint>(numAttributes); ++i)
    {
        static const std::array<GLenum, 2> properties {GL_TYPE, GL_LOCATION};
        static std::array<GLint, 2> values {};

        glGetProgramResourceiv(m_id,
                                         GL_PROGRAM_INPUT,
                                         i,
                                         properties.size(),
                                         properties.data(),
                                         values.size(),
                                         nullptr,
                                         values.data());

        glGetProgramResourceName(m_id,
                                           GL_PROGRAM_INPUT,
                                           i,
                                           m_attributeNameMaxLength,
                                           nullptr,
                                           attributeName.data());

        m_attributesInfo.push_back({ currentRenderer->vertexAttributeByName(attributeName.data()),
                                     static_cast<uint16_t>(i),
                                     values[1],
                                     Conversions::GL2VertexNumComponents(static_cast<GLenum>(values[0])),
                                     Conversions::GL2VertexComponentType(static_cast<GLenum>(values[0])) });
    }

    return ProgramBase_4_5::postBuild(log);
}

int32_t RenderProgram_4_5::attributeLocationByName(const std::string &value) const
{
    CHECK_CURRENT_CONTEXT
    return glGetProgramResourceLocation(m_id, GL_PROGRAM_INPUT, value.data());
}

const std::vector<core::graphics::AttributeInfo> &RenderProgram_4_5::attributesInfo() const
{
    CHECK_CURRENT_CONTEXT
    return m_attributesInfo;
}

std::string RenderProgram_4_5::attributeNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT
    std::vector<GLchar> name(static_cast<size_t>(m_attributeNameMaxLength));
    glGetProgramResourceName(m_id,
                                       GL_PROGRAM_INPUT,
                                       static_cast<GLuint>(index),
                                       m_attributeNameMaxLength,
                                       nullptr,
                                       name.data());

    return name.data();
}

std::shared_ptr<RenderProgram_4_5> RenderProgram_4_5::create(const std::shared_ptr<utils::Shader>& vertexShader,
    const std::shared_ptr<utils::Shader>& fragmentShader)
{
    if (!vertexShader)
        LOG_CRITICAL << "Vertex shader can't be nullptr";

    if (!fragmentShader)
        LOG_CRITICAL << "Fragment shader can't be nullptr";

    auto renderProgram = std::make_shared<RenderProgram_4_5>();
    return renderProgram->compileAndLink(
        {{GL_VERTEX_SHADER, std::cref(vertexShader->data())},
        {GL_FRAGMENT_SHADER, std::cref(fragmentShader->data())}}) ?
                renderProgram :
                nullptr;
}

std::shared_ptr<RenderProgram_4_5> RenderProgram_4_5::create(const std::shared_ptr<utils::Shader>& vertexShader,
    const std::shared_ptr<utils::Shader>& geometryShader,
    const std::shared_ptr<utils::Shader>& fragmentShader)
{
    if (!vertexShader)
        LOG_CRITICAL << "Vertex shader can't be nullptr";

    if (!geometryShader)
        LOG_CRITICAL << "Geometry shader can't be nullptr";

    if (!fragmentShader)
        LOG_CRITICAL << "Fragment shader can't be nullptr";

    auto renderProgram = std::make_shared<RenderProgram_4_5>();
    return renderProgram->compileAndLink(
                         {{GL_VERTEX_SHADER, std::cref(vertexShader->data())},
                          {GL_GEOMETRY_SHADER, std::cref(geometryShader->data())},
                          {GL_FRAGMENT_SHADER, std::cref(fragmentShader->data())}}) ?
                renderProgram :
                nullptr;
}

// ComputeProgram_4_5

ComputeProgram_4_5::ComputeProgram_4_5()
    : ProgramBase_4_5()
{
    SAVE_CURRENT_CONTEXT
}

ComputeProgram_4_5::~ComputeProgram_4_5() = default;


bool ComputeProgram_4_5::postBuild(std::string &log)
{
    CHECK_CURRENT_CONTEXT
    std::array<GLint, 3> params;
    glGetProgramiv(m_id, GL_COMPUTE_WORK_GROUP_SIZE, params.data());
    m_workGroupSize = glm::uvec3(static_cast<uint32_t>(params[0]),
                                 static_cast<uint32_t>(params[1]),
                                 static_cast<uint32_t>(params[2]));

    return ProgramBase_4_5::postBuild(log);
}

glm::uvec3 ComputeProgram_4_5::workGroupSize() const
{
    CHECK_CURRENT_CONTEXT
    return m_workGroupSize;
}

std::shared_ptr<ComputeProgram_4_5> ComputeProgram_4_5::create(const std::shared_ptr<utils::Shader>& computeShader)
{
    if (!computeShader)
        LOG_CRITICAL << "Compute shader can't be nullptr";

    auto computerProgram = std::make_shared<ComputeProgram_4_5>();
    return computerProgram->compileAndLink({std::make_pair(GL_COMPUTE_SHADER, std::cref(computeShader->data()))}) ?
                computerProgram :
                nullptr;
}

// GLFWRenderer

GLFWRenderer::GLFWRenderer(const std::string &name, const std::weak_ptr<GLFWWidget> &widget)
    : core::graphics::RendererBase(name)
    , m_widget(widget)
    , m_screenSize(0u, 0u)
{
    LOG_INFO << "Graphics renderer \"" << GLFWRenderer::name() << "\" has been created";
}

GLFWRenderer::~GLFWRenderer()
{
    LOG_INFO << "Graphics renderer \"" << GLFWRenderer::name() << "\" has been destroyed";
}

std::shared_ptr<core::graphics::IGraphicsWidget> GLFWRenderer::widget()
{
    // No CHECK_THIS_CONTEXT because of recurion calls
    return m_widget.expired() ? nullptr : m_widget.lock();
}

std::shared_ptr<const core::graphics::IGraphicsWidget> GLFWRenderer::widget() const
{
    // No CHECK_THIS_CONTEXT because of recurion calls
    return const_cast<GLFWRenderer*>(this)->widget();
}

void GLFWRenderer::blitFrameBuffer(std::shared_ptr<const core::graphics::IFrameBuffer> src,
                                            std::shared_ptr<core::graphics::IFrameBuffer> dst,
                                            const glm::uvec4 &srcViewport,
                                            const glm::uvec4 &dstViewport,
                                            bool colorMsk, bool depthMask, bool stencilMask,
                                            bool linearFilter)
{
    CHECK_THIS_CONTEXT
    auto srcFramebuffer = std::dynamic_pointer_cast<const FrameBufferBase_4_5>(src);
    if (!srcFramebuffer)
        LOG_CRITICAL << "Source framebuffer can't be nullptr";

    CHECK_RESOURCE_CONTEXT(srcFramebuffer)

    auto dstFramebuffer = std::dynamic_pointer_cast<FrameBufferBase_4_5>(dst);
    if (!dstFramebuffer)
        LOG_CRITICAL << "Destination framebuffer can't be nullptr";

    CHECK_RESOURCE_CONTEXT(dstFramebuffer)

    GLbitfield mask = 0;
    if (colorMsk)
        mask |= GL_COLOR_BUFFER_BIT;
    if (depthMask)
        mask |= GL_DEPTH_BUFFER_BIT;
    if (stencilMask)
        mask |= GL_STENCIL_BUFFER_BIT;

    GLenum filter = linearFilter ? GL_LINEAR : GL_NEAREST;

    glBlitNamedFramebuffer(srcFramebuffer->id(), dstFramebuffer->id(),
                           static_cast<GLint>(srcViewport.x), static_cast<GLint>(srcViewport.y),
                           static_cast<GLint>(srcViewport.z), static_cast<GLint>(srcViewport.w),
                           static_cast<GLint>(dstViewport.x), static_cast<GLint>(dstViewport.y),
                           static_cast<GLint>(dstViewport.z), static_cast<GLint>(dstViewport.w),
                           mask, filter);
}

void GLFWRenderer::copyBufferSubData(const std::shared_ptr<core::graphics::IBuffer>& dst, const std::shared_ptr<const core::graphics::IBuffer>& src, size_t dstOffset, size_t srcOffset, size_t size) const
{
    CHECK_THIS_CONTEXT

    auto dst_4_5 = std::dynamic_pointer_cast<BufferBase_4_5>(dst);
    if (!dst_4_5)
        LOG_CRITICAL << "Destination buffer can't be nullptr";
    CHECK_RESOURCE_CONTEXT(dst_4_5)

    auto src_4_5 = std::dynamic_pointer_cast<const BufferBase_4_5>(src);
    if (!src_4_5)
        LOG_CRITICAL << "Source buffer can't be nullptr";
    CHECK_RESOURCE_CONTEXT(src_4_5)

    size = glm::min(size, dst_4_5->fullSize() - dstOffset);
    size = glm::min(size, src_4_5->fullSize() - srcOffset);

    glCopyNamedBufferSubData(src_4_5->id(), dst_4_5->id(), srcOffset, dstOffset, size);
}

bool GLFWRenderer::registerVertexAttribute(const std::string& name, utils::VertexAttribute id)
{
    CHECK_THIS_CONTEXT
    if (auto it = m_attributeIds.find(name); it != m_attributeIds.end())
    {
        LOG_ERROR << "Attribute " << name << " is already registered";
        return false;
    }

    m_attributeIds.insert({ name, id });
    return true;
}

bool GLFWRenderer::unregisterVertexAttribute(const std::string& name)
{
    CHECK_THIS_CONTEXT
    if (auto it = m_attributeIds.find(name); it != m_attributeIds.end())
    {
        m_attributeIds.erase(it);
        return true;
    }

    LOG_ERROR << "Attribute " << name << " has not been registered";
    return false;
}

utils::VertexAttribute GLFWRenderer::vertexAttributeByName(const std::string& name) const
{
    CHECK_THIS_CONTEXT
    auto it = m_attributeIds.find(name);
    return it != m_attributeIds.end() ? it->second : utils::VertexAttribute::Count;
}

bool GLFWRenderer::registerUniformId(const std::string& name, uint16_t id)
{
    CHECK_THIS_CONTEXT
    if (auto it = m_uniformIds.find(name); it != m_uniformIds.end())
    {
        LOG_ERROR << "Uniform " << name << " is already registered";
        return false;
    }

    if (id == 0u)
    {
        LOG_ERROR << "Uniform id can't be 0";
        return false;
    }

    m_uniformIds.insert({ name, id });
    return true;
}

bool GLFWRenderer::unregisterUniformId(const std::string& name)
{
    CHECK_THIS_CONTEXT
    if (auto it = m_uniformIds.find(name); it != m_uniformIds.end())
    {
        m_uniformIds.erase(it);
        return true;
    }

    LOG_ERROR << "Uniform " << name << " has not been registered";
    return false;
}

uint16_t GLFWRenderer::uniformIdByName(const std::string& name) const
{
    CHECK_THIS_CONTEXT
    auto it = m_uniformIds.find(name);
    return it != m_uniformIds.end() ? it->second : 0u;
}

bool GLFWRenderer::registerSSBOId(const std::string& name, uint16_t id)
{
    CHECK_THIS_CONTEXT
    if (auto it = m_SSBOIds.find(name); it != m_SSBOIds.end())
    {
        LOG_ERROR << "SSBO " << name << " is already registered";
        return false;
    }

    if (id == 0u)
    {
        LOG_ERROR << "SSBO id can't be 0";
        return false;
    }

    m_SSBOIds.insert({ name, id });
    return true;
}

bool GLFWRenderer::unregisterSSBOId(const std::string& name)
{
    CHECK_THIS_CONTEXT
    if (auto it = m_SSBOIds.find(name); it != m_SSBOIds.end())
    {
        m_SSBOIds.erase(it);
        return true;
    }

    LOG_ERROR << "SSBO " << name << " has not been registered";
    return false;
}

uint16_t GLFWRenderer::SSBOIdByName(const std::string& name) const
{
    CHECK_THIS_CONTEXT
    auto it = m_SSBOIds.find(name);
    return it != m_SSBOIds.end() ? it->second : 0u;
}

std::shared_ptr<core::graphics::IBuffer> GLFWRenderer::createBuffer(size_t size, const void *data) const
{
    CHECK_THIS_CONTEXT
    return Buffer_4_5::create(size, data);
}

std::shared_ptr<core::graphics::IDynamicBuffer> GLFWRenderer::createDynamicBuffer(size_t size, const void* data) const
{
    CHECK_THIS_CONTEXT
    return DynamicBuffer_4_5::create(size, data);
}

std::shared_ptr<core::graphics::IVertexArray> GLFWRenderer::createVertexArray(const std::shared_ptr<const utils::Mesh> &mesh,
                                                                                       bool uniteVertexBuffers) const
{
    CHECK_THIS_CONTEXT
    return VertexArray_4_5::create(mesh, uniteVertexBuffers);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1DEmpty(uint32_t width,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT
    return Texture1D_4_5::createEmpty(width, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1D(const std::shared_ptr<const utils::Image> &image,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    CHECK_THIS_CONTEXT
    return Texture1D_4_5::create(image, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2DEmpty(uint32_t width,
                                                                                      uint32_t height,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT
    return Texture2D_4_5::createEmpty(width, height, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2D(const std::shared_ptr<const utils::Image> &image,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    CHECK_THIS_CONTEXT
    return Texture2D_4_5::create(image, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture3DEmpty(uint32_t width,
                                                                                      uint32_t height,
                                                                                      uint32_t depth,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT
    return Texture3D_4_5::createEmpty(width, height, depth, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture3D(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    CHECK_THIS_CONTEXT
    return Texture3D_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCubeEmpty(uint32_t width,
                                                                                        uint32_t height,
                                                                                        core::graphics::PixelInternalFormat internalFormat,
                                                                                        uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT
    return TextureCube_4_5::createEmpty(width, height, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCube(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                   core::graphics::PixelInternalFormat internalFormat,
                                                                                   uint32_t numLevels,
                                                                                   bool genMipmaps) const
{
    CHECK_THIS_CONTEXT
    return TextureCube_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1DArrayEmpty(uint32_t width,
                                                                                           uint32_t numLayers,
                                                                                           core::graphics::PixelInternalFormat internalFormat,
                                                                                           uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT
    return Texture1DArray_4_5::createEmpty(width, numLayers, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1DArray(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels,
                                                                                      bool genMipmaps) const
{
    CHECK_THIS_CONTEXT
    return Texture1DArray_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2DArrayEmpty(uint32_t width,
                                                                                           uint32_t height,
                                                                                           uint32_t numLayers,
                                                                                           core::graphics::PixelInternalFormat internalFormat,
                                                                                           uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT
    return Texture2DArray_4_5::createEmpty(width, height, numLayers, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2DArray(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels,
                                                                                      bool genMipmaps) const
{
    CHECK_THIS_CONTEXT
    return Texture2DArray_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCubeArrayEmpty(uint32_t width,
                                                                                             uint32_t height,
                                                                                             uint32_t numLayers,
                                                                                             core::graphics::PixelInternalFormat internalFormat,
                                                                                             uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT
    return TextureCubeArray_4_5::createEmpty(width, height, numLayers, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCubeArray(const std::vector<std::vector<std::shared_ptr<const utils::Image>>> &images,
                                                                                        core::graphics::PixelInternalFormat internalFormat,
                                                                                        uint32_t numLevels,
                                                                                        bool genMipmaps) const
{
    CHECK_THIS_CONTEXT
    return TextureCubeArray_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureRectEmpty(uint32_t width,
                                                                                        uint32_t height,
                                                                                        core::graphics::PixelInternalFormat internalFormat) const
{
    CHECK_THIS_CONTEXT
    return TextureRect_4_5::createEmpty(width, height, internalFormat);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureRect(const std::shared_ptr<const utils::Image> &image,
                                                                                   core::graphics::PixelInternalFormat internalFormat) const
{
    CHECK_THIS_CONTEXT
    return TextureRect_4_5::create(image, internalFormat);
}

std::shared_ptr<core::graphics::ITextureHandle> GLFWRenderer::createTextureHandle(
    const core::graphics::PConstTexture& texture) const
{
    CHECK_THIS_CONTEXT
    return TextureHandle_4_5::create(texture);
}

std::shared_ptr<core::graphics::IImage> GLFWRenderer::createImage(core::graphics::IImage::DataAccess access,
                                                                           const core::graphics::PConstTexture &texture,
                                                                           uint32_t level) const
{
    CHECK_THIS_CONTEXT
    return Image_4_5::create(access, texture, level);
}

std::shared_ptr<core::graphics::IRenderBuffer> GLFWRenderer::createRenderBuffer(uint32_t width,
                                                                                         uint32_t height,
                                                                                         core::graphics::PixelInternalFormat internalFormat) const
{
    CHECK_THIS_CONTEXT
    return RenderBuffer_4_5::create(width, height, internalFormat);
}

std::shared_ptr<core::graphics::IFrameBuffer> GLFWRenderer::createFrameBuffer() const
{
    CHECK_THIS_CONTEXT
    return FrameBuffer_4_5::create();
}

std::shared_ptr<core::graphics::IRenderProgram> GLFWRenderer::createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                                           const std::shared_ptr<utils::Shader> &fragmentShader) const
{
    CHECK_THIS_CONTEXT
    return RenderProgram_4_5::create(vertexShader, fragmentShader);
}

std::shared_ptr<core::graphics::IRenderProgram> GLFWRenderer::createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                                           const std::shared_ptr<utils::Shader> &geometryShader,
                                                                                           const std::shared_ptr<utils::Shader> &fragmentShader) const
{
    CHECK_THIS_CONTEXT
    return RenderProgram_4_5::create(vertexShader, geometryShader, fragmentShader);
}

std::shared_ptr<core::graphics::IComputeProgram> GLFWRenderer::createComputeProgram(const std::shared_ptr<utils::Shader> &computeShader) const
{
    CHECK_THIS_CONTEXT
    return ComputeProgram_4_5::create(computeShader);
}

const core::graphics::SupportedImageFormats & GLFWRenderer::supportedImageFormats() const
{
    CHECK_THIS_CONTEXT
    return Image_4_5::supportedImageFormats();
}

const glm::uvec2 & GLFWRenderer::screenSize() const
{
    CHECK_THIS_CONTEXT
    return m_screenSize;
}

void GLFWRenderer::resize(const glm::uvec2& value)
{
    CHECK_THIS_CONTEXT
    m_screenSize = value;
}

void GLFWRenderer::clearRenderData()
{
    CHECK_THIS_CONTEXT
    m_renderData.clear();
}

void GLFWRenderer::addRenderData(const std::shared_ptr<core::graphics::IRenderProgram> &renderProgram,
                                          const std::shared_ptr<const core::Drawable> &drawable,
                                          const glm::mat4x4 &transform)
{
    CHECK_THIS_CONTEXT
    auto renderProgram_4_5 = std::dynamic_pointer_cast<RenderProgram_4_5>(renderProgram);
    if (!renderProgram_4_5)
        LOG_CRITICAL << "Render program can't be nullptr";

    CHECK_RESOURCE_CONTEXT(renderProgram_4_5)

    if (!drawable)
        LOG_CRITICAL << "Drawable can't be nullptr";

    m_renderData.push_back({transform, renderProgram_4_5, drawable});
}

void GLFWRenderer::render(const std::shared_ptr<core::graphics::IFrameBuffer> &frameBuffer,
                                   const glm::uvec4 &viewport,
                                   const glm::mat4x4 &viewMatrix,
                                   const glm::mat4x4 &projectionMatrix,
                                   const core::PConstStateSet &globalStateSet)
{
    CHECK_THIS_CONTEXT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    auto frameBuffer_4_5 = std::dynamic_pointer_cast<FrameBufferBase_4_5>(frameBuffer);
    if (!frameBuffer_4_5)
        LOG_CRITICAL << "Framebuffer can't be nullptr";

    CHECK_RESOURCE_CONTEXT(frameBuffer_4_5)

    if (frameBuffer_4_5->faceCulling())
    {
        glCullFace(Conversions::FaceType2GL(frameBuffer_4_5->cullFaceType()));
        glEnable(GL_CULL_FACE);
    }
    else
        glDisable(GL_CULL_FACE);

    for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    {
        GLboolean mask = frameBuffer_4_5->colorMask(i);
        glColorMaski(i, mask, mask, mask, mask);
    }

    if (frameBuffer_4_5->depthTest())
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(Conversions::ComparingFunc2GL(frameBuffer_4_5->depthFunc()));
        glDepthMask(frameBuffer_4_5->depthMask());
    }
    else
        glDisable(GL_DEPTH_TEST);

    if (frameBuffer_4_5->stencilTest())
    {
        glEnable(GL_STENCIL_TEST);
        for (const auto &face : {core::graphics::FaceType::Front, core::graphics::FaceType::Back})
        {
            const auto &operations = frameBuffer_4_5->stencilOperations(face);

            glStencilOpSeparate(Conversions::FaceType2GL(face),
                                Conversions::StencilOperation2GL(operations[0]),
                                Conversions::StencilOperation2GL(operations[1]),
                                Conversions::StencilOperation2GL(operations[2]));

            glStencilFuncSeparate(Conversions::FaceType2GL(face),
                                  Conversions::ComparingFunc2GL(frameBuffer_4_5->stencilComparingFunc(face)),
                frameBuffer_4_5->stencilReferenceValue(face),
                frameBuffer_4_5->stencilMaskValue(face));
        }
    }
    else
        glDisable(GL_STENCIL_TEST);

    if (frameBuffer_4_5->blending())
    {
        glEnable(GL_BLEND);

        auto blendConstantColor = frameBuffer_4_5->blendConstantColor();
        glBlendColor(blendConstantColor.r, blendConstantColor.g, blendConstantColor.b, frameBuffer_4_5->blendConstantAlpha());

        for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
        {
            glBlendEquationSeparatei(i,
                                     Conversions::BlendEquetion2GL(frameBuffer_4_5->blendColorEquation(i)),
                                     Conversions::BlendEquetion2GL(frameBuffer_4_5->blendAlphaEquation(i)));
            glBlendFuncSeparatei(i,
                                 Conversions::BlendFactor2GL(frameBuffer_4_5->blendColorSourceFactor(i)),
                                 Conversions::BlendFactor2GL(frameBuffer_4_5->blendColorDestinationFactor(i)),
                                 Conversions::BlendFactor2GL(frameBuffer_4_5->blendAlphaSourceFactor(i)),
                                 Conversions::BlendFactor2GL(frameBuffer_4_5->blendAlphaDestinationFactor(i)));
        }
    }
    else
        glDisable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_4_5->id());
    frameBuffer_4_5->clear();

    glViewport(static_cast<GLint>(viewport.x),
               static_cast<GLint>(viewport.y),
               static_cast<GLsizei>(viewport.z),
               static_cast<GLsizei>(viewport.w));

    auto mvpStateSet = std::make_shared<MVPStateSet>();
    mvpStateSet->setViewportSize(glm::uvec2(viewport.z, viewport.w));
    mvpStateSet->setViewMatrix(viewMatrix);
    mvpStateSet->setProjectionMatrix(projectionMatrix);

    for (auto &renderData : m_renderData)
    {
        auto &drawable = std::get<2>(renderData);

        auto vao_4_5 = std::dynamic_pointer_cast<const VertexArray_4_5>(drawable->vertexArray());
        if (!vao_4_5)
            LOG_CRITICAL << "VAO can't be nullptr";
        CHECK_RESOURCE_CONTEXT(vao_4_5)
        glBindVertexArray(vao_4_5->id());

        auto renderProgram_4_5 = std::get<1>(renderData);
        if (!renderProgram_4_5)
            LOG_CRITICAL << "Render program can't be nullptr";
        CHECK_RESOURCE_CONTEXT(renderProgram_4_5)

        mvpStateSet->setModelMatrix(std::get<0>(renderData));
        std::list<core::PConstStateSet> stateSetList {globalStateSet, mvpStateSet, drawable};

        glUseProgram(renderProgram_4_5->id());
        setupVertexAttributes(renderProgram_4_5, vao_4_5);
        setupUniforms(renderProgram_4_5, stateSetList);
        setupSSBOs(renderProgram_4_5, stateSetList);

        for (auto &primitiveSet : vao_4_5->primitiveSets())
        {
            if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
                glDrawArrays(Conversions::PrimitiveType2GL(drawArrays->primitiveType()),
                             static_cast<GLint>(drawArrays->first()),
                             static_cast<GLint>(drawArrays->count()));
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
                glDrawElementsBaseVertex(Conversions::PrimitiveType2GL(drawElements->primitiveType()),
                                         static_cast<GLsizei>(drawElements->count()),
                                         Conversions::DrawElementsIndexType2GL(drawElements->indexType()),
                                         reinterpret_cast<const void*>(drawElements->offset()),
                                         static_cast<GLint>(drawElements->baseVertex()));
        }
    }

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void GLFWRenderer::compute(const std::shared_ptr<core::graphics::IComputeProgram> &computeProgram,
                                    const glm::uvec3 &numInvocations,
                                    const core::PConstStateSet &globalStateSet)
{
    CHECK_THIS_CONTEXT
    auto computeProgram_4_5 = std::dynamic_pointer_cast<ComputeProgram_4_5>(computeProgram);
    if (!computeProgram_4_5)
        LOG_CRITICAL << "Compute program can't be nullptr";
    CHECK_RESOURCE_CONTEXT(computeProgram_4_5)

    std::list<core::PConstStateSet> stateSetList {globalStateSet};

    glUseProgram(computeProgram_4_5->id());
    setupUniforms(computeProgram_4_5, stateSetList);
    setupSSBOs(computeProgram_4_5, stateSetList);

    auto numWorkGroups = glm::uvec3(glm::ceil(glm::vec3(numInvocations) / glm::vec3(computeProgram->workGroupSize())) + glm::vec3(.5f));
    glDispatchCompute(numWorkGroups.x, numWorkGroups.y, numWorkGroups.z);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

bool GLFWRenderer::doMakeCurrent()
{
    if (m_widget.expired())
        LOG_CRITICAL << "GLFWWidget can't be nullptr";

    auto widgetPtr = m_widget.lock();
    if (!widgetPtr)
        LOG_CRITICAL << "GLFWWidget can't be nullptr";

    auto window = widgetPtr->m().window();
    if (window == glfwGetCurrentContext())
        return true;

    glfwMakeContextCurrent(window);
    return true;
}

bool GLFWRenderer::doDoneCurrent()
{
    glfwMakeContextCurrent(nullptr);
    return true;
}

void GLFWRenderer::setupVertexAttributes(const std::shared_ptr<RenderProgram_4_5> &renderProgram,
                                                  const std::shared_ptr<const VertexArray_4_5> &vao)
{
    CHECK_THIS_CONTEXT
    CHECK_RESOURCE_CONTEXT(renderProgram)
    CHECK_RESOURCE_CONTEXT(vao)
    for (const auto &attributeInfo : renderProgram->attributesInfo())
    {
        if (attributeInfo.id == utils::VertexAttribute::Count)
            continue;

        // TODO: component type can be different for integer vertex attributes
//        if (attributeInfo.componentType != vao->vertexAttributeComponentType(attributeInfo.id))
//            LOG_CRITICAL << "Vertex attribute \"" << renderProgram->attributeNameByIndex(attributeInfo.index) << "\" has wrong component type";

        if (attributeInfo.numComponents != vao->vertexAttributeNumComponents(attributeInfo.id))
            LOG_CRITICAL << "Vertex attribute \"" << renderProgram->attributeNameByIndex(attributeInfo.index) << "\" has wrong number of components";
    }
}

void GLFWRenderer::setupUniform(GLuint rpId,
                                         GLint loc,
                                         int32_t &textureUnit,
                                         int32_t &imageUnit,
                                         const core::PConstAbstractUniform &uniform)
{
    CHECK_THIS_CONTEXT
    if (!uniform)
        LOG_CRITICAL << "Uniform can't be nullptr";

    switch (uniform->type())
    {
    case core::graphics::UniformType::Bool:
    {
        glProgramUniform1i(rpId, loc, core::uniform_cast<bool>(uniform)->data());
        break;
    }
    case core::graphics::UniformType::Single:
    {
        glProgramUniform1f(rpId, loc, core::uniform_cast<float>(uniform)->data());
        break;
    }
    case core::graphics::UniformType::SingleVec2:
    {
        glProgramUniform2fv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::vec2>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::SingleVec3:
    {
        glProgramUniform3fv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::vec3>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::SingleVec4:
    {
        glProgramUniform4fv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::vec4>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::SingleMat2:
    {
        glProgramUniformMatrix2fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::mat2x2>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::SingleMat3:
    {
        glProgramUniformMatrix3fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::mat3x3>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::SingleMat4:
    {
        glProgramUniformMatrix4fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::mat4x4>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Double:
    {
        glProgramUniform1d(rpId, loc, core::uniform_cast<double>(uniform)->data());
        break;
    }
    case core::graphics::UniformType::DoubleVec2:
    {
        glProgramUniform2dv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::dvec2>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleVec3:
    {
        glProgramUniform3dv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::dvec3>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleVec4:
    {
        glProgramUniform4dv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::dvec4>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleMat2:
    {
        glProgramUniformMatrix2dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::dmat2x2>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleMat3:
    {
        glProgramUniformMatrix3dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::dmat3x3>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleMat4:
    {
        glProgramUniformMatrix4dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::dmat4x4>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Int32:
    {
        glProgramUniform1i(rpId, loc, core::uniform_cast<int32_t>(uniform)->data());
        break;
    }
    case core::graphics::UniformType::Int32Vec2:
    {
        glProgramUniform2iv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::i32vec2>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Int32Vec3:
    {
        glProgramUniform3iv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::i32vec3>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Int32Vec4:
    {
        glProgramUniform4iv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::i32vec4>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Uint32:
    {
        glProgramUniform1ui(rpId, loc, core::uniform_cast<uint32_t>(uniform)->data());
        break;
    }
    case core::graphics::UniformType::Uint32Vec2:
    {
        glProgramUniform2uiv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::u32vec2>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Uint32Vec3:
    {
        glProgramUniform3uiv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::u32vec3>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Uint32Vec4:
    {
        glProgramUniform4uiv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::u32vec4>(uniform)->data()));
        break;
    }
    case core::graphics::UniformType::Sampler1D:
    case core::graphics::UniformType::Sampler2D:
    case core::graphics::UniformType::Sampler3D:
    case core::graphics::UniformType::SamplerCube:
    case core::graphics::UniformType::Sampler1DArray:
    case core::graphics::UniformType::Sampler2DArray:
    case core::graphics::UniformType::SamplerCubeArray:
    case core::graphics::UniformType::SamplerRect:
    {
        auto textureUniform = core::uniform_cast<core::graphics::PConstTexture>(uniform)->data();
        bindTexture(textureUnit, textureUniform);
        glProgramUniform1i(rpId, loc, textureUnit++);
        break;
    }
    case core::graphics::UniformType::Image1D:
    case core::graphics::UniformType::Image2D:
    case core::graphics::UniformType::Image3D:
    case core::graphics::UniformType::ImageCube:
    case core::graphics::UniformType::Image1DArray:
    case core::graphics::UniformType::Image2DArray:
    case core::graphics::UniformType::ImageCubeArray:
    case core::graphics::UniformType::ImageRect:
    {
        auto imageUniform = core::uniform_cast<core::graphics::PConstImage>(uniform)->data();
        bindImage(imageUnit, imageUniform);
        glProgramUniform1i(rpId, loc, imageUnit++);
        break;
    }
    case core::graphics::UniformType::AtomicCounterUint:
    {
        auto atomicCounterUniform = core::uniform_cast<core::graphics::PConstBuffer>(uniform)->data();
        bindAtomicCounterBuffer(static_cast<GLuint>(loc), atomicCounterUniform);
        break;
    }
    default:
    {
        break;
    }
    }
}

void GLFWRenderer::setupUniforms(const std::shared_ptr<ProgramBase_4_5> &program,
                                          const std::list<core::PConstStateSet> &stateSetList)
{
    CHECK_THIS_CONTEXT
    CHECK_RESOURCE_CONTEXT(program)
    int32_t textureUnit = 0;
    int32_t imageUnit = 0;

    for (const auto &uniformInfo : program->uniformsInfo())
    {
        core::PConstAbstractUniform uniform;
        const auto uniformId = core::castToUniformId(uniformInfo.id);

        for (const auto &stateSet: stateSetList)
            if (uniform = stateSet->uniform(uniformId); uniform)
                break;

        if (!uniform)
        {
            const auto uniformName = program->uniformNameByIndex(uniformInfo.index);
            for (const auto &stateSet: stateSetList)
                if (uniform = stateSet->userUniform(uniformName); uniform)
                    break;
        }

        if (!uniform)
            LOG_CRITICAL << "Uniform \"" << program->uniformNameByIndex(uniformInfo.index) << "\" has not set";

        if (uniformInfo.type != uniform->type())
            LOG_CRITICAL << "Uniform \"" << program->uniformNameByIndex(uniformInfo.index) << "\" has wrong type";

        setupUniform(program->id(), uniformInfo.location, textureUnit, imageUnit, uniform);
    }
}

void GLFWRenderer::setupSSBOs(const std::shared_ptr<ProgramBase_4_5> &program,
                                       const std::list<core::PConstStateSet> &stateSetList)
{
    CHECK_THIS_CONTEXT
    CHECK_RESOURCE_CONTEXT(program)
    uint32_t bufferUnit = 0u;

    for (const auto &ssboInfo : program->SSBOsInfo())
    {
        core::graphics::PConstBuffer bufferRange;
        const auto ssboId = core::castToSSBOId(ssboInfo.id);

        for (const auto &stateSet: stateSetList)
            if (bufferRange = stateSet->SSBO(ssboId); bufferRange)
                break;

        if (!bufferRange)
        {
            const auto uniformName = program->SSBONameByIndex(ssboInfo.index);
//            for (const auto &stateSet: stateSetList)
//                if (bufferRange = stateSet->userSSBO(uniformName); bufferRange)
//                    break;
        }

        if (!bufferRange)
            LOG_CRITICAL << "SSBO \"" << program->SSBONameByIndex(ssboInfo.index) << "\" has not set";

        bindSSBO(bufferUnit, bufferRange);
        glShaderStorageBlockBinding(program->id(), ssboInfo.index, bufferUnit++);
    }
}

void GLFWRenderer::bindTexture(int32_t unit, const core::graphics::PConstTexture &texture)
{
    CHECK_THIS_CONTEXT
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(texture);
    if (!textureBase)
        LOG_CRITICAL << "Texture can't be nullptr";
    CHECK_RESOURCE_CONTEXT(textureBase)

    glBindTextureUnit(static_cast<GLuint>(unit), textureBase->id());
}

void GLFWRenderer::bindImage(int32_t unit, const core::graphics::PConstImage &image)
{
    CHECK_THIS_CONTEXT
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(image->texture());
    if (!textureBase)
        LOG_CRITICAL << "Image's texture can't be nullptr";
    CHECK_RESOURCE_CONTEXT(textureBase)

    glBindImageTexture(static_cast<GLuint>(unit),
                       textureBase->id(),
                       static_cast<GLint>(image->mipmapLevel()),
                       GL_TRUE,
                       0u,
                       Conversions::ImageDataAccess2GL(image->access()),
                       Conversions::PixelInternalFormat2GL(image->format()));
}

void GLFWRenderer::bindBuffer(GLenum target, GLuint bindingPoint, const core::graphics::PConstBuffer &buffer)
{
    CHECK_THIS_CONTEXT
    auto buffer_4_5 = std::dynamic_pointer_cast<const Buffer_4_5>(buffer);
    if (!buffer)
        LOG_CRITICAL << "Buffer can't be nullptr";
    CHECK_RESOURCE_CONTEXT(buffer_4_5)

    glBindBufferRange(target,
                      bindingPoint,
                      buffer_4_5->id(),
                      0u,
                      static_cast<GLsizeiptr>(buffer_4_5->size()));
}

void GLFWRenderer::bindSSBO(uint32_t unit, const core::graphics::PConstBuffer &bufferRange)
{
    CHECK_THIS_CONTEXT
    bindBuffer(GL_SHADER_STORAGE_BUFFER, unit, bufferRange);
}

void GLFWRenderer::bindAtomicCounterBuffer(GLuint bindingPoint, const core::graphics::PConstBuffer &bufferRange)
{
    CHECK_THIS_CONTEXT
    bindBuffer(GL_ATOMIC_COUNTER_BUFFER, bindingPoint, bufferRange);
}

}
}
