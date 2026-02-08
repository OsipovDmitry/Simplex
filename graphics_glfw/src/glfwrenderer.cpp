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

#include <core/drawable.h>
#include <core/uniform.h>

#include <graphics_glfw/glfwwidget.h>

#include "glfwwidgetprivate.h"
#include "glfwrenderer.h"
#include "mvpstateset.h"

#define DEFAULT_SETUP \
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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
        { GL_DEPTH32F_STENCIL8, core::graphics::PixelInternalFormat::Depth32FStencil8 },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::PixelInternalFormat::Count : it->second;
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

GLenum Conversions::ImageDataAccess2GL(core::graphics::Image::DataAccess value)
{
    static std::array<GLbitfield, core::graphics::Image::numElementsDataAccess()> s_table {
        GL_READ_ONLY,
        GL_WRITE_ONLY,
        GL_READ_WRITE
    };

    return s_table[core::graphics::Image::castFromDataAccess(value)];
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
    return (it == s_table.end()) ? core::graphics::UniformType::Count : it->second;
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
    SAVE_CURRENT_CONTEXT;
}

BufferBase_4_5::MappedData_4_5::~MappedData_4_5()
{
    CHECK_CURRENT_CONTEXT;
    if (!m_mappedBuffer.expired())
    {
        auto bufferBase_4_5 = m_mappedBuffer.lock();
        glUnmapNamedBuffer(bufferBase_4_5->id());
        bufferBase_4_5->m_isMapped = false;
    }
}

uint8_t* BufferBase_4_5::MappedData_4_5::get()
{
    CHECK_CURRENT_CONTEXT;
    return m_mappedBuffer.expired() ? nullptr : m_data;
}

const uint8_t *BufferBase_4_5::MappedData_4_5::get() const
{
    CHECK_CURRENT_CONTEXT;
    return const_cast<MappedData_4_5*>(this)->get();
}

// BufferBase_4_5

BufferBase_4_5::~BufferBase_4_5()
{
    CHECK_CURRENT_CONTEXT;
    glDeleteBuffers(1, &m_id);
}

GLuint BufferBase_4_5::id() const
{
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

BufferBase_4_5::BufferBase_4_5(uint64_t size, const void* data)
{
    SAVE_CURRENT_CONTEXT;
    glCreateBuffers(1, &m_id);
    glNamedBufferStorage(m_id, size, data, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
}

size_t BufferBase_4_5::sizeImpl() const
{
    CHECK_CURRENT_CONTEXT;
    GLint result = 0;
    glGetNamedBufferParameteriv(m_id, GL_BUFFER_SIZE, &result);
    return static_cast<size_t>(result);
}

void BufferBase_4_5::resizeImpl(size_t newSize)
{
    CHECK_CURRENT_CONTEXT;

    auto oldSize = sizeImpl();
    if (oldSize == newSize)
        return;

    GLuint newID;
    glCreateBuffers(1, &newID);
    glNamedBufferStorage(newID, newSize, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    glCopyNamedBufferSubData(m_id, newID, 0u, 0u, glm::min(oldSize, newSize));
    glDeleteBuffers(1, &m_id);
    m_id = newID;
}

std::unique_ptr<BufferBase_4_5::MappedData_4_5> BufferBase_4_5::mapImpl(core::graphics::IBuffer::MapAccess access, size_t offset, size_t size)
{
    CHECK_CURRENT_CONTEXT;
    if (m_isMapped)
    {
        LOG_ERROR << "Buffer is already mapped";
        return nullptr;
    }

    m_isMapped = true;

    return std::make_unique<MappedData_4_5>(
                weak_from_this(),
                static_cast<uint8_t*>(glMapNamedBufferRange(
                    m_id,
                    static_cast<GLintptr>(offset),
                    static_cast<GLsizei>(size),
                    Conversions::BufferMapAccess2GL(access))));
}

// StaticBuffer_4_5

StaticBuffer_4_5::StaticBuffer_4_5(uint64_t size, const void* data)
    : BufferBase_4_5(size, data)
{
    SAVE_CURRENT_CONTEXT;
}

StaticBuffer_4_5::~StaticBuffer_4_5() = default;

bool StaticBuffer_4_5::isEmpty() const
{
    CHECK_CURRENT_CONTEXT;
    return sizeImpl() == 0u;
}

size_t StaticBuffer_4_5::size() const
{
    CHECK_CURRENT_CONTEXT;
    return sizeImpl();
}

std::unique_ptr<core::graphics::IBuffer::MappedData> StaticBuffer_4_5::map(MapAccess access, size_t offset, size_t size)
{
    CHECK_CURRENT_CONTEXT;

    auto bufferSize = StaticBuffer_4_5::size();

    if (size == 0)
        size = bufferSize - offset;

    if (offset > bufferSize)
    {
        LOG_ERROR << "The offset of mapped data is out of range";
        return nullptr;
    }

    if (offset + size > bufferSize)
    {
        LOG_ERROR << "The size of mapped data is out of range";
        return nullptr;
    }

    return mapImpl(access, offset, size);
}

std::unique_ptr<const core::graphics::IBuffer::MappedData> StaticBuffer_4_5::map(MapAccess access, size_t offset, size_t size) const
{
    CHECK_CURRENT_CONTEXT;
    return const_cast<StaticBuffer_4_5*>(this)->map(access, offset, size);
}

std::shared_ptr<StaticBuffer_4_5> StaticBuffer_4_5::create(size_t size, const void *data)
{
    return std::make_shared<StaticBuffer_4_5>(size, data);
}

// DynamicBuffer_4_5

DynamicBuffer_4_5::DynamicBuffer_4_5(uint64_t size, const void* data)
    : BufferBase_4_5(size, data)
    , m_size(size)

{
    SAVE_CURRENT_CONTEXT;
}

DynamicBuffer_4_5::~DynamicBuffer_4_5() = default;


bool DynamicBuffer_4_5::isEmpty() const
{
    CHECK_CURRENT_CONTEXT;
    return m_size == 0u;
}

size_t DynamicBuffer_4_5::size() const
{
    CHECK_CURRENT_CONTEXT;
    return m_size;
}

size_t DynamicBuffer_4_5::capacity() const
{
    CHECK_CURRENT_CONTEXT;
    return sizeImpl();
}

void DynamicBuffer_4_5::reserve(size_t size)
{
    CHECK_CURRENT_CONTEXT;
    if (size > capacity())
        resizeImpl(size);
}

void DynamicBuffer_4_5::shrinkToFit()
{
    CHECK_CURRENT_CONTEXT;
    resizeImpl(m_size);
}

void DynamicBuffer_4_5::clear()
{
    CHECK_CURRENT_CONTEXT;
    m_size = 0u;
}
 
void DynamicBuffer_4_5::insert(size_t offset, const void* data, size_t insertedSize)
{
    CHECK_CURRENT_CONTEXT;

    if (!insertedSize)
        return;

    const auto oldSize = size();
    const auto newSize = oldSize + insertedSize;

    if (offset > oldSize)
    {
        LOG_ERROR << "The offset of inserted data is out of range";
        return;
    }
    
    // the case when data is inserted to the end and it's enough memory to insert is handled separatly
    // it's a freq-used case and no need to reallocate the memory 
    if ((offset == oldSize) && (newSize <= capacity()))
    {
        if (data)
        {
            glNamedBufferSubData(
                m_id,
                oldSize,
                insertedSize,
                data);
        }
    }
    else
    {
        GLuint newID;
        glCreateBuffers(1, &newID);
        glNamedBufferStorage(newID, newSize, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

        glCopyNamedBufferSubData(
            m_id,
            newID,
            0u,
            0u,
            static_cast<GLsizeiptr>(offset));

        if (data)
        {
            glNamedBufferSubData(
                newID,
                offset,
                insertedSize,
                data);
        }

        glCopyNamedBufferSubData(
            m_id,
            newID,
            static_cast<GLintptr>(offset),
            static_cast<GLintptr>(offset + insertedSize),
            static_cast<GLsizeiptr>(oldSize - offset));

        glDeleteBuffers(1, &m_id);
        m_id = newID;
    }

    m_size = newSize;
}

void DynamicBuffer_4_5::erase(size_t offset, size_t erasedSize)
{
    CHECK_CURRENT_CONTEXT;

    if (!erasedSize)
        return;

    auto oldSize = size();

    if (offset > oldSize)
    {
        LOG_ERROR << "The offset of erased data is out of range";
        return;
    }

    if (offset + erasedSize > oldSize)
    {
        LOG_ERROR << "The size of erased data is out of range";
        return;
    }

    auto newSize = oldSize - erasedSize;

    GLuint newID;
    glCreateBuffers(1, &newID);
    glNamedBufferStorage(newID, newSize, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    glCopyNamedBufferSubData(
        m_id,
        newID,
        0u,
        0u,
        static_cast<GLsizeiptr>(offset));
    glCopyNamedBufferSubData(
        m_id,
        newID,
        static_cast<GLintptr>(offset + erasedSize),
        static_cast<GLintptr>(offset),
        static_cast<GLsizeiptr>(oldSize - offset - erasedSize));
    glDeleteBuffers(1, &m_id);
    m_id = newID;
    m_size = newSize;
}

void DynamicBuffer_4_5::resize(size_t size)
{
    CHECK_CURRENT_CONTEXT;

    if (auto capacitySize = capacity();  size > capacitySize)
        reserve(glm::max(size, capacitySize * 2u));

    m_size = size;
}

std::unique_ptr<core::graphics::IBuffer::MappedData> DynamicBuffer_4_5::map(MapAccess access, size_t offset, size_t size)
{
    CHECK_CURRENT_CONTEXT;

    auto bufferSize = DynamicBuffer_4_5::size();

    if (size == 0)
        size = bufferSize - offset;

    if (offset > bufferSize)
    {
        LOG_ERROR << "The offset of mapped data is out of range";
        return nullptr;
    }

    if (offset + size > bufferSize)
    {
        LOG_ERROR << "The size of mapped data is out of range";
        return nullptr;
    }

    return mapImpl(access, offset, size);
}

std::unique_ptr<const core::graphics::IBuffer::MappedData> DynamicBuffer_4_5::map(MapAccess access, size_t offset, size_t size) const
{
    CHECK_CURRENT_CONTEXT;
    return const_cast<DynamicBuffer_4_5*>(this)->map(access, offset, size);
}

std::shared_ptr<DynamicBuffer_4_5> DynamicBuffer_4_5::create(size_t size, const void* data)
{
    return std::make_shared<DynamicBuffer_4_5>(size, data);
}

// VertexArray_4_5

VertexArray_4_5::VertexArray_4_5()
{
    SAVE_CURRENT_CONTEXT;
    glCreateVertexArrays(1, &m_id);
}

VertexArray_4_5::~VertexArray_4_5()
{
    CHECK_CURRENT_CONTEXT;
    glDeleteVertexArrays(1, &m_id);
}

GLuint VertexArray_4_5::id() const
{
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

uint32_t VertexArray_4_5::attachVertexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer, size_t offset, size_t stride)
{
    CHECK_CURRENT_CONTEXT;
    auto bufferBase_4_5 = std::dynamic_pointer_cast<BufferBase_4_5>(buffer);
    if (!bufferBase_4_5)
        LOG_CRITICAL << "Buffer can't be nullptr";
    CHECK_SHARED_CONTEXTS(this, bufferBase_4_5);

    auto bindingIndex = static_cast<uint32_t>(-1);

    auto declaration = VertexBufferDeclaration{ buffer, offset, stride };

    if (auto it = std::find(m_vertexBuffers.begin(), m_vertexBuffers.end(), declaration); it != m_vertexBuffers.end())
        return static_cast<uint32_t>(it - m_vertexBuffers.begin());
    
    auto it = std::find_if(m_vertexBuffers.begin(), m_vertexBuffers.end(), [](const VertexBufferDeclaration&v){ return v.buffer == nullptr;});
    bindingIndex = static_cast<uint32_t>(it - m_vertexBuffers.begin());
    if (it == m_vertexBuffers.end())
        m_vertexBuffers.resize(m_vertexBuffers.size() + 1);
    m_vertexBuffers[bindingIndex] = declaration;

    glVertexArrayVertexBuffer(m_id, static_cast<GLuint>(bindingIndex), bufferBase_4_5->id(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));

    return bindingIndex;
}

void VertexArray_4_5::detachVertexBuffer(uint32_t bindingIndex)
{
    CHECK_CURRENT_CONTEXT;
        m_vertexBuffers[bindingIndex] = VertexBufferDeclaration{ nullptr, 0u, 0u };

    for (const auto &[attrib, attributeDeclaration]: m_attributes)
        if (attributeDeclaration.bindingIndex == bindingIndex)
        {
            undeclareVertexAttribute(attrib);
            break;
        }

    glVertexArrayVertexBuffer(m_id, static_cast<GLuint>(bindingIndex), 0, 0, 0);
}

std::shared_ptr<const core::graphics::IBuffer> VertexArray_4_5::vertexBuffer(uint32_t bindingIndex) const
{
    CHECK_CURRENT_CONTEXT;
    return m_vertexBuffers[bindingIndex].buffer;
}

size_t VertexArray_4_5::vertexBufferOffset(uint32_t bindingIndex) const
{
    CHECK_CURRENT_CONTEXT;
    return m_vertexBuffers[bindingIndex].offset;
}

size_t VertexArray_4_5::vertexBufferStride(uint32_t bindingIndex) const
{
    CHECK_CURRENT_CONTEXT;
    return m_vertexBuffers[bindingIndex].stride;
}

void VertexArray_4_5::declareVertexAttribute(utils::VertexAttribute attrib,
                                             uint32_t bindingIndex,
                                             uint32_t numComponents,
                                             utils::VertexComponentType type,
                                             uint32_t relativeOffset)
{
    CHECK_CURRENT_CONTEXT;
    if (numComponents < 1 || numComponents > 4)
        LOG_CRITICAL << "Num components must be [1..4]";

    m_attributes[attrib] = AttributeDeclaration{ bindingIndex, numComponents, type, relativeOffset };

    setupVertexAttrubute(attrib, static_cast<GLuint>(utils::castFromVertexAttribute(attrib)));
}

void VertexArray_4_5::undeclareVertexAttribute(utils::VertexAttribute attrib)
{
    CHECK_CURRENT_CONTEXT;
    glDisableVertexArrayAttrib(m_id, static_cast<GLuint>(utils::castFromVertexAttribute(attrib)));

    m_attributes.erase(attrib);
}

uint32_t VertexArray_4_5::vertexAttributeBindingIndex(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT;
    auto it = m_attributes.find(attrib);
    return (it != m_attributes.end()) ? it->second.bindingIndex : static_cast<uint32_t>(-1);
}

uint32_t VertexArray_4_5::vertexAttributeNumComponents(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT;
    auto it = m_attributes.find(attrib);
    return (it != m_attributes.end()) ? it->second.numComponents : 0u;
}

utils::VertexComponentType VertexArray_4_5::vertexAttributeComponentType(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT;
    auto it = m_attributes.find(attrib);
    return (it != m_attributes.end()) ? it->second.componentType : utils::VertexComponentType::Count;
}

uint32_t VertexArray_4_5::vertexAttributeRelativeOffset(utils::VertexAttribute attrib) const
{
    CHECK_CURRENT_CONTEXT;
    auto it = m_attributes.find(attrib);
    return (it != m_attributes.end()) ? it->second.relativeOffset : 0u;
}

void VertexArray_4_5::attachIndexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer)
{
    CHECK_CURRENT_CONTEXT;
    auto bufferBase_4_5 = std::dynamic_pointer_cast<BufferBase_4_5>(buffer);
    if (!bufferBase_4_5)
        LOG_CRITICAL << "Buffer can't be nullptr";
    CHECK_SHARED_CONTEXTS(this, bufferBase_4_5);

    m_indexBuffer = buffer;

    glVertexArrayElementBuffer(m_id, bufferBase_4_5->id());
}

void VertexArray_4_5::detachIndexBuffer()
{
    CHECK_CURRENT_CONTEXT;
    m_indexBuffer = nullptr;

    glVertexArrayElementBuffer(m_id, 0);
}

std::shared_ptr<const core::graphics::IBuffer> VertexArray_4_5::indexBuffer() const
{
    CHECK_CURRENT_CONTEXT;
    return m_indexBuffer;
}

std::shared_ptr<VertexArray_4_5> VertexArray_4_5::create()
{
    return std::make_shared<VertexArray_4_5>();;
}

void VertexArray_4_5::setupVertexAttrubute(utils::VertexAttribute attrib, int32_t loc)
{
    auto it = m_attributes.find(attrib);
    if (it == m_attributes.end())
    {
        LOG_CRITICAL << "Vertex attribute is not found";
        return;
    }

    glVertexArrayAttribBinding(m_id, static_cast<GLuint>(loc), static_cast<GLuint>(it->second.bindingIndex));
    glEnableVertexArrayAttrib(m_id, static_cast<GLuint>(loc));

    switch (it->second.componentType)
    {
    case utils::VertexComponentType::Single: {
        glVertexArrayAttribFormat(m_id,
            loc,
            static_cast<GLint>(it->second.numComponents),
            Conversions::VertexComponentType2GL(it->second.componentType),
            GL_FALSE,
            static_cast<GLuint>(it->second.relativeOffset));
        break;
    }
    case utils::VertexComponentType::Double: {
        glVertexArrayAttribLFormat(m_id,
            loc,
            static_cast<GLint>(it->second.numComponents),
            Conversions::VertexComponentType2GL(it->second.componentType),
            static_cast<GLuint>(it->second.relativeOffset));
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
            static_cast<GLint>(it->second.numComponents),
            Conversions::VertexComponentType2GL(it->second.componentType),
            static_cast<GLuint>(it->second.relativeOffset));
        break;
    }
    default: {
        LOG_CRITICAL << "Undefined verte component type";
        break;
    }
    }
}

// TextureBase_4_5

TextureBase_4_5::TextureBase_4_5()
    : m_id(0u)
{
    SAVE_CURRENT_CONTEXT;
}

TextureBase_4_5::~TextureBase_4_5()
{
    CHECK_CURRENT_CONTEXT;
    glDeleteTextures(1, &m_id);
}

GLuint TextureBase_4_5::id() const
{
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

GLenum TextureBase_4_5::GLinternalFormat() const
{
    CHECK_CURRENT_CONTEXT;
    GLint result;

    glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_INTERNAL_FORMAT, &result);

    return static_cast<GLenum>(result);
}

glm::uvec2 TextureBase_4_5::size() const
{
    CHECK_CURRENT_CONTEXT;
    return mipmapSize(0u);
}

core::graphics::PixelInternalFormat TextureBase_4_5::internalFormat() const
{
    CHECK_CURRENT_CONTEXT;
    return Conversions::GL2PixelInternalFormat(GLinternalFormat());
}

bool TextureBase_4_5::hasAlpha() const
{
    CHECK_CURRENT_CONTEXT;
    GLint result;

    glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_ALPHA_SIZE, &result);

    return result != 0;
}

bool TextureBase_4_5::hasDepth() const
{
    CHECK_CURRENT_CONTEXT;
    GLint result;

    glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_DEPTH_SIZE, &result);

    return result != 0;
}

glm::uvec3 TextureBase_4_5::mipmapSize(uint32_t level) const
{
    CHECK_CURRENT_CONTEXT;
    GLint w, h, d;

    glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_WIDTH, &w);
    glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_HEIGHT, &h);
    glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_DEPTH, &d);

    return glm::uvec3(w, h, d);
}

uint32_t TextureBase_4_5::numMipmapLevels() const
{
    CHECK_CURRENT_CONTEXT;
    GLint result;

    glGetTextureParameteriv(m_id, GL_TEXTURE_IMMUTABLE_LEVELS, &result);

    return static_cast<uint32_t>(result);
}

uint32_t TextureBase_4_5::numFaces() const
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    glGenerateTextureMipmap(m_id);
}

void TextureBase_4_5::setBorderColor(const glm::vec4 &value)
{
    CHECK_CURRENT_CONTEXT;
    glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
}

void TextureBase_4_5::setWrapMode(core::graphics::TextureWrapMode value)
{
    CHECK_CURRENT_CONTEXT;
    auto glValue = static_cast<GLint>(Conversions::TextureWrapMode2GL(value));

    glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, glValue);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, glValue);
    glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, glValue);
}

void TextureBase_4_5::setFilterMode(core::graphics::TextureFilterMode value)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    SAVE_CURRENT_CONTEXT;
    glCreateTextures(GL_TEXTURE_1D, 1, &m_id);
    glTextureStorage1D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width));
}

Texture1D_4_5::~Texture1D_4_5() = default;

core::graphics::TextureType Texture1D_4_5::type() const
{
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::Type1D;
}

void Texture1D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, internalFormat(), numMipmapLevels());
}

std::shared_ptr<Texture1D_4_5> Texture1D_4_5::createEmpty(uint32_t width, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
{
    if (width == 0u)
        LOG_CRITICAL << "Width can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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
    SAVE_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::Type2D;
}

void Texture2D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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
    SAVE_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::Type3D;
}

void Texture3D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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
    SAVE_CURRENT_CONTEXT;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
    glTextureStorage2D(
        m_id,
        static_cast<GLsizei>(numLevels),
        Conversions::PixelInternalFormat2GL(internalFormat),
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height));
    glTextureParameteri(m_id, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);
}

TextureCube_4_5::~TextureCube_4_5() = default;

core::graphics::TextureType TextureCube_4_5::type() const
{
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::TypeCube;
}

uint32_t TextureCube_4_5::numFaces() const
{
    CHECK_CURRENT_CONTEXT;
    return 6u;
}

void TextureCube_4_5::setSubImage(uint32_t level,const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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
    SAVE_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::Type1DArray;
}

void Texture1DArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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
    SAVE_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::Type2DArray;
}

void Texture2DArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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
    SAVE_CURRENT_CONTEXT;
    glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &m_id);
    glTextureStorage3D(
        m_id,
        static_cast<GLsizei>(numLevels),
        Conversions::PixelInternalFormat2GL(internalFormat),
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        static_cast<GLsizei>(6u * numLayers));
    glTextureParameteri(m_id, GL_TEXTURE_CUBE_MAP_SEAMLESS, GL_TRUE);
}

TextureCubeArray_4_5::~TextureCubeArray_4_5() = default;

core::graphics::TextureType TextureCubeArray_4_5::type() const
{
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::TypeCubeArray;
}

uint32_t TextureCubeArray_4_5::numFaces() const
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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
    SAVE_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    return core::graphics::TextureType::TypeRect;
}

void TextureRect_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    auto baseSize = mipmapSize();
    return createEmpty(baseSize.x, baseSize.y, internalFormat());
}

std::shared_ptr<TextureRect_4_5> TextureRect_4_5::createEmpty(uint32_t width,
    uint32_t height,
    core::graphics::PixelInternalFormat internalFormat)
{
    if (width * height == 0u)
        LOG_CRITICAL << "Width and height can't be 0";

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
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

    if (internalFormat == core::graphics::PixelInternalFormat::Count)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createEmpty(image->width(), image->height(), internalFormat);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), image->height(), 0u), image->numComponents(), image->type(), image->data());

    return result;
}

// TextureHandle_4_5

TextureHandle_4_5::TextureHandle_4_5(const core::graphics::PConstTexture& texture)
    : m_texture(texture)
{
    SAVE_CURRENT_CONTEXT;

    if (auto texture_4_5 = std::dynamic_pointer_cast<const TextureBase_4_5>(m_texture); texture_4_5)
    {
        CHECK_SHARED_CONTEXTS(this, texture_4_5);
        m_id = glGetTextureHandleARB(texture_4_5->id());
    }
    else
        LOG_CRITICAL << "Texture can't be nullptr";
}

TextureHandle_4_5::~TextureHandle_4_5()
{
    CHECK_CURRENT_CONTEXT;
    glMakeTextureHandleNonResidentARB(m_id);
}

core::graphics::TextureHandle TextureHandle_4_5::handle() const
{
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

core::graphics::PConstTexture TextureHandle_4_5::texture() const
{
    CHECK_CURRENT_CONTEXT;
    return m_texture;
}

void TextureHandle_4_5::makeResident()
{
    CHECK_CURRENT_CONTEXT;
    glMakeTextureHandleResidentARB(m_id);
}

void TextureHandle_4_5::doneResident()
{
    CHECK_CURRENT_CONTEXT;
    glMakeTextureHandleNonResidentARB(m_id);
}

std::shared_ptr<TextureHandle_4_5> TextureHandle_4_5::create(const core::graphics::PConstTexture& texture)
{
    return std::make_shared<TextureHandle_4_5>(texture);
}

// ImageHandle_4_5

ImageHandle_4_5::ImageHandle_4_5(const core::graphics::PConstImage& image)
    : m_image(image)
{
    SAVE_CURRENT_CONTEXT;

    if (auto texture_4_5 = std::dynamic_pointer_cast<const TextureBase_4_5>(image->texture()); texture_4_5)
    {
        CHECK_SHARED_CONTEXTS(this, texture_4_5);
        m_id = glGetImageHandleARB(
            texture_4_5->id(),
            image->mipmapLevel(),
            GL_TRUE,
            0u,
            Conversions::PixelInternalFormat2GL(texture_4_5->internalFormat()));
    }
    else
        LOG_CRITICAL << "Texture can't be nullptr";
}

ImageHandle_4_5::~ImageHandle_4_5()
{
    CHECK_CURRENT_CONTEXT;
    glMakeImageHandleNonResidentARB(m_id);
}

core::graphics::TextureHandle ImageHandle_4_5::handle() const
{
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

core::graphics::PConstImage ImageHandle_4_5::image() const
{
    CHECK_CURRENT_CONTEXT;
    return m_image;
}

void ImageHandle_4_5::makeResident()
{
    CHECK_CURRENT_CONTEXT;
    glMakeImageHandleResidentARB(m_id, Conversions::ImageDataAccess2GL(m_image->access()));
}

void ImageHandle_4_5::doneResident()
{
    CHECK_CURRENT_CONTEXT;
    glMakeImageHandleNonResidentARB(m_id);

}

std::shared_ptr<ImageHandle_4_5> ImageHandle_4_5::create(const core::graphics::PConstImage& image)
{
    return std::make_shared<ImageHandle_4_5>(image);
}

// RenderBuffer_4_5

RenderBuffer_4_5::RenderBuffer_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat)
{
    SAVE_CURRENT_CONTEXT;
    glCreateRenderbuffers(1, &m_id);
    glNamedRenderbufferStorage(
        m_id,
        Conversions::PixelInternalFormat2GL(internalFormat),
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height));
}

RenderBuffer_4_5::~RenderBuffer_4_5()
{
    CHECK_CURRENT_CONTEXT;
    glDeleteRenderbuffers(1, &m_id);
}

GLuint RenderBuffer_4_5::id() const
{
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

glm::uvec2 RenderBuffer_4_5::size() const
{
    CHECK_CURRENT_CONTEXT;
    GLint width, height;
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_WIDTH, &width);
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_HEIGHT, &height);

    return glm::uvec2(width, height);
}

core::graphics::PixelInternalFormat RenderBuffer_4_5::internalFormat() const
{
    CHECK_CURRENT_CONTEXT;
    GLint result;
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_INTERNAL_FORMAT, &result);

    return Conversions::GL2PixelInternalFormat(static_cast<GLenum>(result));
}

bool RenderBuffer_4_5::hasAlpha() const
{
    CHECK_CURRENT_CONTEXT;
    GLint result;
    glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_ALPHA_SIZE, &result);

    return result != 0;
}

bool RenderBuffer_4_5::hasDepth() const
{
    CHECK_CURRENT_CONTEXT;
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
    SAVE_CURRENT_CONTEXT;
    for (uint32_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
        setClearColor(i, glm::vec4(.5f, .5f, 1.f, 1.f));

    setClearDepth(1.f);
    setClearStencil(0x00u);

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
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

bool FrameBufferBase_4_5::isComplete() const
{
    CHECK_CURRENT_CONTEXT;
    return glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void FrameBufferBase_4_5::clear(const std::unordered_set<core::graphics::FrameBufferAttachment>& mask)
{
    CHECK_CURRENT_CONTEXT;

    for (auto attachment : mask)
    {
        if (!m_attachments.count(attachment))
            continue;

        if (core::graphics::IsFrameBufferColorAttachment(attachment))
        {
            auto drawBuffer = core::graphics::FrameBufferColorAttachmentIndex(attachment);

            auto colorMask = m_colorMasks[drawBuffer];
            glColorMaski(static_cast<GLuint>(drawBuffer), colorMask, colorMask, colorMask, colorMask);

            const auto& clearColor = m_clearColor[drawBuffer];
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
        }
        else if (attachment == core::graphics::FrameBufferAttachment::DepthStencil)
        {
            glDepthMask(m_depthMask);
            glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);
        }
        else if (attachment == core::graphics::FrameBufferAttachment::Depth)
        {
            glDepthMask(m_depthMask);
            glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &m_clearDepth);
        }
        else if (attachment == core::graphics::FrameBufferAttachment::Stencil)
        {
            glClearNamedFramebufferiv(m_id, GL_STENCIL, 0, &m_clearStencil);
        }
    }
}

std::shared_ptr<const core::graphics::ISurface> FrameBufferBase_4_5::attachmentSurface(core::graphics::FrameBufferAttachment value) const
{
    CHECK_CURRENT_CONTEXT;
    std::shared_ptr<const core::graphics::ISurface> result;
    if (auto it = m_attachments.find(value); it != m_attachments.end())
        result = it->second.surface;
    return result;
}

uint32_t FrameBufferBase_4_5::attachmentMipmapLevel(core::graphics::FrameBufferAttachment value) const
{
    CHECK_CURRENT_CONTEXT;
    auto result = static_cast<uint32_t>(-1);
    if (auto it = m_attachments.find(value); it != m_attachments.end())
        result = it->second.mipmapLevel;
    return result;
}

uint32_t FrameBufferBase_4_5::attachmentLayer(core::graphics::FrameBufferAttachment value) const
{
    CHECK_CURRENT_CONTEXT;
    auto result = static_cast<uint32_t>(-1);
    if (auto it = m_attachments.find(value); it != m_attachments.end())
        result = it->second.layer;
    return result;
}

const core::graphics::FrameBufferClearColor &FrameBufferBase_4_5::clearColor(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    return m_clearColor[index];
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::vec4 &value)
{
    CHECK_CURRENT_CONTEXT;
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.floatColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Single, clearColorValue};
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::i32vec4 &value)
{
    CHECK_CURRENT_CONTEXT;
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.intColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Int32, clearColorValue };
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::u32vec4 &value)
{
    CHECK_CURRENT_CONTEXT;
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.uintColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Uint32, clearColorValue };
}

float FrameBufferBase_4_5::clearDepth() const
{
    CHECK_CURRENT_CONTEXT;
    return m_clearDepth;
}

void FrameBufferBase_4_5::setClearDepth(float value)
{
    CHECK_CURRENT_CONTEXT;
    m_clearDepth = value;
}

int32_t FrameBufferBase_4_5::clearStencil() const
{
    CHECK_CURRENT_CONTEXT;
    return m_clearStencil;
}

void FrameBufferBase_4_5::setClearStencil(uint8_t value)
{
    CHECK_CURRENT_CONTEXT;
    m_clearStencil = value;
}

bool FrameBufferBase_4_5::faceCulling() const
{
    CHECK_CURRENT_CONTEXT;
    return m_faceCulling;
}

core::graphics::FaceType FrameBufferBase_4_5::cullFaceType() const
{
    CHECK_CURRENT_CONTEXT;
    return m_cullFaceType;
}

void FrameBufferBase_4_5::setFaceCulling(bool value, core::graphics::FaceType type)
{
    CHECK_CURRENT_CONTEXT;
    m_faceCulling = value;
    m_cullFaceType = type;
}

bool FrameBufferBase_4_5::colorMask(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    return m_colorMasks[index];
}

void FrameBufferBase_4_5::setColorMask(uint32_t index, bool value)
{
    CHECK_CURRENT_CONTEXT;
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    m_colorMasks[index] = value;
}

void FrameBufferBase_4_5::setColorMasks(bool value)
{
    CHECK_CURRENT_CONTEXT;
    for (auto &mask : m_colorMasks)
        mask = value;
}

bool FrameBufferBase_4_5::depthTest() const
{
    CHECK_CURRENT_CONTEXT;
    return m_depthTest;
}

core::graphics::ComparingFunc FrameBufferBase_4_5::depthFunc() const
{
    CHECK_CURRENT_CONTEXT;
    return m_depthFunc;
}

void FrameBufferBase_4_5::setDepthTest(bool value, core::graphics::ComparingFunc func)
{
    CHECK_CURRENT_CONTEXT;
    m_depthTest = value;
    m_depthFunc = func;
}

bool FrameBufferBase_4_5::depthMask() const
{
    CHECK_CURRENT_CONTEXT;
    return m_depthMask;
}

void FrameBufferBase_4_5::setDepthMask(bool value)
{
    CHECK_CURRENT_CONTEXT;
    m_depthMask = value;
}

bool FrameBufferBase_4_5::stencilTest() const
{
    CHECK_CURRENT_CONTEXT;
    return m_stencilTest;
}

void FrameBufferBase_4_5::setStencilTest(bool value)
{
    CHECK_CURRENT_CONTEXT;
    m_stencilTest = value;
}

core::graphics::ComparingFunc FrameBufferBase_4_5::stencilComparingFunc(core::graphics::FaceType value) const
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    return m_blending;
}

void FrameBufferBase_4_5::setBlending(bool value)
{
    CHECK_CURRENT_CONTEXT;
    m_blending = value;
}

core::graphics::BlendEquation FrameBufferBase_4_5::blendColorEquation(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendColorEquation[index];
}

core::graphics::BlendEquation FrameBufferBase_4_5::blendAlphaEquation(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendAlphaEquation[index];
}

void FrameBufferBase_4_5::setBlendEquation(uint32_t index, core::graphics::BlendEquation colorValue, core::graphics::BlendEquation alphaValue)
{
    CHECK_CURRENT_CONTEXT;
    m_blendColorEquation[index] = colorValue;
    m_blendAlphaEquation[index] = alphaValue;
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendColorSourceFactor(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendColorSourceFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendAlphaSourceFactor(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendAlphaSourceFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendColorDestinationFactor(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendColorDestFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendAlphaDestinationFactor(uint32_t index) const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendAlphaDestFactor[index];
}

void FrameBufferBase_4_5::setBlendFactor(uint32_t index,
                                         core::graphics::BlendFactor colorSourceValue,
                                         core::graphics::BlendFactor colorDestValue,
                                         core::graphics::BlendFactor alphaSourceValue,
                                         core::graphics::BlendFactor alphaDestValue)
{
    CHECK_CURRENT_CONTEXT;
    m_blendColorSourceFactor[index] = colorSourceValue;
    m_blendColorDestFactor[index] = colorDestValue;
    m_blendAlphaSourceFactor[index] = alphaSourceValue;
    m_blendAlphaDestFactor[index] = alphaDestValue;
}

glm::vec3 FrameBufferBase_4_5::blendConstantColor() const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendConstColor;
}

void FrameBufferBase_4_5::setBlendConstantColor(const glm::vec3 &value)
{
    CHECK_CURRENT_CONTEXT;
    m_blendConstColor = value;
}

float FrameBufferBase_4_5::blendConstantAlpha() const
{
    CHECK_CURRENT_CONTEXT;
    return m_blendConstAlpha;
}

void FrameBufferBase_4_5::setBlendConstantAlpha(float value)
{
    CHECK_CURRENT_CONTEXT;
    m_blendConstAlpha = value;
}

// FrameBuffer_4_5

FrameBuffer_4_5::FrameBuffer_4_5()
    : FrameBufferBase_4_5(0u)
{
    SAVE_CURRENT_CONTEXT;
    glCreateFramebuffers(1, &m_id);
}

FrameBuffer_4_5::~FrameBuffer_4_5()
{
    CHECK_CURRENT_CONTEXT;
    glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer_4_5::attach(core::graphics::FrameBufferAttachment key,
                             std::shared_ptr<const core::graphics::ISurface> surface,
                             uint32_t level)
{
    CHECK_CURRENT_CONTEXT;
    detach(key);

    if (auto texture_4_5 = std::dynamic_pointer_cast<const TextureBase_4_5>(surface); texture_4_5)
    {
        CHECK_SHARED_CONTEXTS(this, texture_4_5);
        glNamedFramebufferTexture(m_id, Conversions::FrameBufferAttachment2GL(key), texture_4_5->id(), static_cast<GLint>(level));
        m_attachments[key] = { surface, level, 0u };
    }
    else if (auto renderBuffer_4_5 = std::dynamic_pointer_cast<const RenderBuffer_4_5>(surface); renderBuffer_4_5)
    {
        CHECK_SHARED_CONTEXTS(this, renderBuffer_4_5);
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
    CHECK_CURRENT_CONTEXT;
    detach(key);

    if (auto texture_4_5 = std::dynamic_pointer_cast<const TextureBase_4_5>(texture); texture_4_5)
    {
        CHECK_SHARED_CONTEXTS(this, texture_4_5);
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
    CHECK_CURRENT_CONTEXT;
    if (auto it = m_attachments.find(key); it != m_attachments.end())
    {
        glNamedFramebufferRenderbuffer(
            m_id,
            Conversions::FrameBufferAttachment2GL(key),
            GL_RENDERBUFFER,
            0u);
        m_attachments.erase(it);
    }
}

void FrameBuffer_4_5::detachAll()
{
    CHECK_CURRENT_CONTEXT;
    while (!m_attachments.empty())
        detach(m_attachments.begin()->first);
}

std::shared_ptr<FrameBuffer_4_5> FrameBuffer_4_5::create()
{
    return std::make_shared<FrameBuffer_4_5>();
}

// DefaultFrameBuffer_4_5

DefaultFrameBuffer_4_5::DefaultFrameBuffer_4_5(GLuint defaultFbo)
    : FrameBufferBase_4_5(defaultFbo)
{
    SAVE_CURRENT_CONTEXT;

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
    m_attachments[core::graphics::FrameBufferColorAttachment(0u)] = {nullptr, 0, 0};
}

DefaultFrameBuffer_4_5::~DefaultFrameBuffer_4_5() = default;

void DefaultFrameBuffer_4_5::attach(core::graphics::FrameBufferAttachment, std::shared_ptr<const core::graphics::ISurface>, uint32_t)
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::attachLayer(core::graphics::FrameBufferAttachment, std::shared_ptr<const core::graphics::ITexture>, uint32_t, uint32_t)
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::detach(core::graphics::FrameBufferAttachment)
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::detachAll()
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::vec4&)
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::i32vec4&)
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::u32vec4&)
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::setClearDepth(float)
{
    CHECK_CURRENT_CONTEXT;
}

void DefaultFrameBuffer_4_5::setClearStencil(uint8_t)
{
    CHECK_CURRENT_CONTEXT;
}

std::shared_ptr<DefaultFrameBuffer_4_5> DefaultFrameBuffer_4_5::create(GLuint id)
{
    return std::make_shared<DefaultFrameBuffer_4_5>(id);
}

// ProgramBase_4_5

ProgramBase_4_5::ProgramBase_4_5()
    : m_uniformNameMaxLength(0)
    , m_uniformBlockNameMaxLength(0)
    , m_bufferVariableNameMaxLength(0)
    , m_shaderStorageBlockNameMaxLength(0)
{
    SAVE_CURRENT_CONTEXT;
    m_id = glCreateProgram();
}

ProgramBase_4_5::~ProgramBase_4_5()
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
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
    CHECK_CURRENT_CONTEXT;
    return m_id;
}

bool ProgramBase_4_5::preBuild(std::string &)
{
    CHECK_CURRENT_CONTEXT;
    return true;
}

bool ProgramBase_4_5::postBuild(std::string &)
{
    CHECK_CURRENT_CONTEXT;
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
        static const std::vector<GLenum> uniformProperties {GL_BLOCK_INDEX, GL_TYPE, GL_LOCATION};
        std::vector<GLint> uniformValues(uniformProperties.size());

        glGetProgramResourceiv(
            m_id,
            GL_UNIFORM,
            uniformIndex,
            uniformProperties.size(),
            uniformProperties.data(),
            uniformValues.size(),
            nullptr,
            uniformValues.data());

        if (uniformValues[0u] != -1) // Skip any uniforms that are in a block
            continue;

        if (uniformValues[1u] == GL_UNSIGNED_INT_ATOMIC_COUNTER) // get atomic counter binding point instead of location
        {
            GLint atomicCounterBufferIndex;
            glGetActiveUniformsiv(
                m_id,
                1u,
                &uniformIndex,
                GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX,
                &atomicCounterBufferIndex);

            glGetActiveAtomicCounterBufferiv(
                m_id,
                static_cast<GLuint>(atomicCounterBufferIndex),
                GL_ATOMIC_COUNTER_BUFFER_BINDING,
                uniformValues.data() + 2u);
        }

        glGetProgramResourceName(
            m_id,
            GL_UNIFORM,
            uniformIndex,
            m_uniformNameMaxLength,
            nullptr,
            uniformName.data());

        m_uniformsInfo.push_back({ currentRenderer->uniformByName(uniformName.data()),
                                   static_cast<uint16_t>(uniformIndex),
                                   uniformValues[2u],
                                   Conversions::GL2UniformType(static_cast<GLenum>(uniformValues[1u])) });
    }

    GLint numUniformBlocks;
    glGetProgramInterfaceiv(m_id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numUniformBlocks);
    m_uniformBlocksInfo.reserve(static_cast<size_t>(numUniformBlocks));

    glGetProgramInterfaceiv(m_id, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &m_uniformBlockNameMaxLength);
    std::vector<GLchar> uniformBlockName(static_cast<size_t>(m_uniformBlockNameMaxLength));

    for (GLuint uniformBlockIndex = 0u; uniformBlockIndex < static_cast<GLuint>(numUniformBlocks); ++uniformBlockIndex)
    {
        static const std::vector<GLenum> uniformBlockProperties{ GL_NUM_ACTIVE_VARIABLES };
        std::vector<GLint> uniformBlockValues(uniformBlockProperties.size());

        glGetProgramResourceiv(
            m_id,
            GL_UNIFORM_BLOCK,
            uniformBlockIndex,
            uniformBlockProperties.size(),
            uniformBlockProperties.data(),
            uniformBlockValues.size(),
            nullptr,
            uniformBlockValues.data());

        static const std::array<GLenum, 1> variablesIndicesProperty{ GL_ACTIVE_VARIABLES };
        std::vector<GLint> variablesIndices(static_cast<GLuint>(uniformBlockValues[0u]));

        glGetProgramResourceiv(
            m_id,
            GL_UNIFORM_BLOCK,
            uniformBlockIndex,
            variablesIndicesProperty.size(),
            variablesIndicesProperty.data(),
            static_cast<GLsizei>(variablesIndices.size()),
            nullptr,
            variablesIndices.data());

        std::vector<core::graphics::BufferVariableInfo> variables;
        variables.reserve(static_cast<size_t>(uniformBlockValues[0u]));

        for (auto variableIndex : variablesIndices)
        {
            static const std::vector<GLenum> variableProperties{
                GL_TYPE,
                GL_ARRAY_SIZE,
                GL_OFFSET,
                GL_ARRAY_STRIDE,
                GL_MATRIX_STRIDE,
                GL_TOP_LEVEL_ARRAY_SIZE,
                GL_TOP_LEVEL_ARRAY_STRIDE };
            std::vector<GLint> variableValues(variableProperties.size());

            glGetProgramResourceiv(
                m_id,
                GL_UNIFORM,
                static_cast<GLuint>(variableIndex),
                variableProperties.size(),
                variableProperties.data(),
                static_cast<GLsizei>(variableValues.size()),
                nullptr,
                variableValues.data());

            // name of buffer variable is not stored
            //glGetProgramResourceName(
            //    m_id,
            //    GL_UNIFORM,
            //    static_cast<GLuint>(variableIndex),
            //    m_unirormNameMaxLength,
            //    nullptr,
            //    uniformName.data());

            variables.push_back({
                static_cast<uint16_t>(variableIndex),
                // Conversions::GL2SSBOVariableType(static_cast<GLenum>(uniformValues[0u])),
                static_cast<uint16_t>(variableValues[1u]),
                static_cast<uint16_t>(variableValues[2u]),
                static_cast<uint16_t>(variableValues[3u]),
                static_cast<uint16_t>(variableValues[4u]),
                static_cast<uint16_t>(variableValues[5u]),
                static_cast<uint16_t>(variableValues[6u]) });
        }

        glGetProgramResourceName(
            m_id,
            GL_UNIFORM_BLOCK,
            uniformBlockIndex,
            m_uniformBlockNameMaxLength,
            nullptr,
            uniformBlockName.data());

        m_uniformBlocksInfo.push_back({
            currentRenderer->uniformBlockByName(uniformBlockName.data()),
            static_cast<uint16_t>(uniformBlockIndex),
            std::move(variables) });
    }

    GLint numShaderStorageBlocks;
    glGetProgramInterfaceiv(m_id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numShaderStorageBlocks);
    m_shaderStorageBlocksInfo.reserve(static_cast<size_t>(numShaderStorageBlocks));

    glGetProgramInterfaceiv(m_id, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &m_shaderStorageBlockNameMaxLength);
    std::vector<GLchar> shaderStorageBlockName(static_cast<size_t>(m_shaderStorageBlockNameMaxLength));

    glGetProgramInterfaceiv(m_id, GL_BUFFER_VARIABLE, GL_MAX_NAME_LENGTH, &m_bufferVariableNameMaxLength);
    std::vector<GLchar> bufferVariableName(static_cast<size_t>(m_bufferVariableNameMaxLength));

    for (GLuint shaderStorageBlockIndex = 0u; shaderStorageBlockIndex < static_cast<GLuint>(numShaderStorageBlocks); ++shaderStorageBlockIndex)
    {
        static const std::vector<GLenum> shaderStorageBlockProperties { GL_NUM_ACTIVE_VARIABLES};
        std::vector<GLint> shaderStorageBlockValues(shaderStorageBlockProperties.size());

        glGetProgramResourceiv(
            m_id,
            GL_SHADER_STORAGE_BLOCK,
            shaderStorageBlockIndex,
            shaderStorageBlockProperties.size(),
            shaderStorageBlockProperties.data(),
            shaderStorageBlockValues.size(),
            nullptr,
            shaderStorageBlockValues.data());

        static const std::array<GLenum, 1> variablesIndicesProperty {GL_ACTIVE_VARIABLES};
        std::vector<GLint> variablesIndices(static_cast<GLuint>(shaderStorageBlockValues[0u]));

        glGetProgramResourceiv(
            m_id,
            GL_SHADER_STORAGE_BLOCK,
            shaderStorageBlockIndex,
            variablesIndicesProperty.size(),
            variablesIndicesProperty.data(),
            static_cast<GLsizei>(variablesIndices.size()),
            nullptr,
            variablesIndices.data());

        std::vector<core::graphics::BufferVariableInfo> variables;
        variables.reserve(static_cast<size_t>(shaderStorageBlockValues[0u]));

        for (auto variableIndex : variablesIndices)
        {
            static const std::vector<GLenum> variableProperties {
                GL_TYPE,
                GL_ARRAY_SIZE,
                GL_OFFSET,
                GL_ARRAY_STRIDE,
                GL_MATRIX_STRIDE,
                GL_TOP_LEVEL_ARRAY_SIZE,
                GL_TOP_LEVEL_ARRAY_STRIDE};
            std::vector<GLint> variableValues(variableProperties.size());

            glGetProgramResourceiv(
                m_id,
                GL_BUFFER_VARIABLE,
                static_cast<GLuint>(variableIndex),
                variableProperties.size(),
                variableProperties.data(),
                static_cast<GLsizei>(variableValues.size()),
                nullptr,
                variableValues.data());

            // name of buffer variable is not stored
            //glGetProgramResourceName(
            //    m_id,
            //    GL_BUFFER_VARIABLE,
            //    static_cast<GLuint>(variableIndex),
            //    m_bufferVariableNameMaxLength,
            //    nullptr,
            //    bufferVariableName.data());

            variables.push_back({
                static_cast<uint16_t>(variableIndex),
                // Conversions::GL2SSBOVariableType(static_cast<GLenum>(uniformValues[0u])),
                static_cast<uint16_t>(variableValues[1u]),
                static_cast<uint16_t>(variableValues[2u]),
                static_cast<uint16_t>(variableValues[3u]),
                static_cast<uint16_t>(variableValues[4u]),
                static_cast<uint16_t>(variableValues[5u]),
                static_cast<uint16_t>(variableValues[6u]) });
        }

        glGetProgramResourceName(
            m_id,
            GL_SHADER_STORAGE_BLOCK,
            shaderStorageBlockIndex,
            m_shaderStorageBlockNameMaxLength,
            nullptr,
            shaderStorageBlockName.data());

        m_shaderStorageBlocksInfo.push_back({ currentRenderer->shaderStorageBlockByName(shaderStorageBlockName.data()),
                                static_cast<uint16_t>(shaderStorageBlockIndex),
                                std::move(variables) });
    }

    return true;
}

//int32_t ProgramBase_4_5::uniformLocationByName(const std::string &value) const
//{
//    CHECK_CURRENT_CONTEXT;
//    return glGetProgramResourceLocation(m_id, GL_UNIFORM, value.data());
//}

const std::vector<core::graphics::UniformInfo> &ProgramBase_4_5::uniformsInfo() const
{
    CHECK_CURRENT_CONTEXT;
    return m_uniformsInfo;
}

const std::vector<core::graphics::UniformBlockInfo>& ProgramBase_4_5::uniformBlocksInfo() const
{
    CHECK_CURRENT_CONTEXT;
    return m_uniformBlocksInfo;
}

const std::vector<core::graphics::ShaderStorageBlockInfo> &ProgramBase_4_5::shaderStorageBlocksInfo() const
{
    CHECK_CURRENT_CONTEXT;
    return m_shaderStorageBlocksInfo;
}

std::string ProgramBase_4_5::uniformNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT;
    std::vector<GLchar> name(static_cast<size_t>(m_uniformNameMaxLength));
    glGetProgramResourceName(m_id,
                             GL_UNIFORM,
                             static_cast<GLuint>(index),
                             m_uniformNameMaxLength,
                             nullptr,
                             name.data());

    return name.data();
}

std::string ProgramBase_4_5::bufferVariableNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT;
    std::vector<GLchar> name(static_cast<size_t>(m_bufferVariableNameMaxLength));
    glGetProgramResourceName(
        m_id,
        GL_BUFFER_VARIABLE,
        static_cast<GLuint>(index),
        m_bufferVariableNameMaxLength,
        nullptr,
        name.data());

    return name.data();
}

std::string ProgramBase_4_5::uniformBlockNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT;
    std::vector<GLchar> name(static_cast<size_t>(m_uniformBlockNameMaxLength));
    glGetProgramResourceName(
        m_id,
        GL_UNIFORM_BLOCK,
        static_cast<GLuint>(index),
        m_uniformBlockNameMaxLength,
        nullptr,
        name.data());

    return name.data();
}

std::string ProgramBase_4_5::shaderStorageBlockNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT;
    std::vector<GLchar> name(static_cast<size_t>(m_shaderStorageBlockNameMaxLength));
    glGetProgramResourceName(
        m_id,
        GL_SHADER_STORAGE_BLOCK,
        static_cast<GLuint>(index),
        m_shaderStorageBlockNameMaxLength,
        nullptr,
        name.data());

    return name.data();
}

// RenderProgram_4_5

RenderProgram_4_5::RenderProgram_4_5()
    : ProgramBase_4_5()
    , m_attributeNameMaxLength(0)
{
    SAVE_CURRENT_CONTEXT;
}

RenderProgram_4_5::~RenderProgram_4_5() = default;


bool RenderProgram_4_5::postBuild(std::string &log)
{
    CHECK_CURRENT_CONTEXT;
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

        static const std::unordered_set<std::string> s_excludedAttributes{
            "gl_DrawID",
            "gl_VertexID",
            "gl_BaseInstance"};

        if (const std::string attributeNameString = attributeName.data(); !s_excludedAttributes.count(attributeNameString))
        {
            m_attributesInfo.push_back({
                currentRenderer->attributeByName(attributeNameString),
                static_cast<uint16_t>(i),
                static_cast<int32_t>(values[1u]),
                Conversions::GL2VertexNumComponents(static_cast<GLenum>(values[0u])),
                Conversions::GL2VertexComponentType(static_cast<GLenum>(values[0u])) });
        }
    }

    GLint numOutputs;
    glGetProgramInterfaceiv(m_id, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &numOutputs);
    m_outputInfo.reserve(static_cast<size_t>(numOutputs));

    glGetProgramInterfaceiv(m_id, GL_PROGRAM_OUTPUT, GL_MAX_NAME_LENGTH, &m_outputNameMaxLength);
    std::vector<GLchar> outputName(static_cast<size_t>(m_outputNameMaxLength));

    for (GLuint i = 0; i < static_cast<GLuint>(numOutputs); ++i)
    {
        static const std::array<GLenum, 2> properties{ GL_TYPE, GL_LOCATION };
        static std::array<GLint, 2> values{};

        glGetProgramResourceiv(m_id,
            GL_PROGRAM_OUTPUT,
            i,
            properties.size(),
            properties.data(),
            values.size(),
            nullptr,
            values.data());

        glGetProgramResourceName(m_id,
            GL_PROGRAM_OUTPUT,
            i,
            m_outputNameMaxLength,
            nullptr,
            outputName.data());

        static const std::unordered_set<std::string> s_excludedOutputs{
            "gl_FragDepth" };

        if (const std::string outputNameString = outputName.data(); !s_excludedOutputs.count(outputNameString))
        {
            m_outputInfo.push_back({
                currentRenderer->outputByName(outputNameString),
                static_cast<uint16_t>(i),
                static_cast<int32_t>(values[1u]) });
        }
    }

    return ProgramBase_4_5::postBuild(log);
}

//int32_t RenderProgram_4_5::attributeLocationByName(const std::string &value) const
//{
//    CHECK_CURRENT_CONTEXT;
//    return glGetProgramResourceLocation(m_id, GL_PROGRAM_INPUT, value.data());
//}

const std::vector<core::graphics::AttributeInfo> &RenderProgram_4_5::attributesInfo() const
{
    CHECK_CURRENT_CONTEXT;
    return m_attributesInfo;
}

std::string RenderProgram_4_5::attributeNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT;
    std::vector<GLchar> name(static_cast<size_t>(m_attributeNameMaxLength));
    glGetProgramResourceName(
        m_id,
        GL_PROGRAM_INPUT,
        static_cast<GLuint>(index),
        m_attributeNameMaxLength,
        nullptr,
        name.data());

    return name.data();
}

const std::vector<core::graphics::OutputInfo>& RenderProgram_4_5::outputsInfo() const
{
    CHECK_CURRENT_CONTEXT;
    return m_outputInfo;
}

std::string RenderProgram_4_5::outputNameByIndex(uint16_t index) const
{
    CHECK_CURRENT_CONTEXT;
    std::vector<GLchar> name(static_cast<size_t>(m_outputNameMaxLength));
    glGetProgramResourceName(
        m_id,
        GL_PROGRAM_OUTPUT,
        static_cast<GLuint>(index),
        m_outputNameMaxLength,
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
    SAVE_CURRENT_CONTEXT;
}

ComputeProgram_4_5::~ComputeProgram_4_5() = default;


bool ComputeProgram_4_5::postBuild(std::string &log)
{
    CHECK_CURRENT_CONTEXT;
    std::array<GLint, 3> params;
    glGetProgramiv(m_id, GL_COMPUTE_WORK_GROUP_SIZE, params.data());
    m_workGroupSize = glm::uvec3(static_cast<uint32_t>(params[0]),
                                 static_cast<uint32_t>(params[1]),
                                 static_cast<uint32_t>(params[2]));

    return ProgramBase_4_5::postBuild(log);
}

glm::uvec3 ComputeProgram_4_5::workGroupSize() const
{
    CHECK_CURRENT_CONTEXT;
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
    CHECK_THIS_CONTEXT;
    auto srcFramebuffer = std::dynamic_pointer_cast<const FrameBufferBase_4_5>(src);
    if (!srcFramebuffer)
        LOG_CRITICAL << "Source framebuffer can't be nullptr";

    CHECK_RESOURCE_CONTEXT(srcFramebuffer);

    auto dstFramebuffer = std::dynamic_pointer_cast<FrameBufferBase_4_5>(dst);
    if (!dstFramebuffer)
        LOG_CRITICAL << "Destination framebuffer can't be nullptr";

    CHECK_RESOURCE_CONTEXT(dstFramebuffer);

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

std::shared_ptr<core::graphics::IStaticBuffer> GLFWRenderer::createStaticBuffer(size_t size, const void *data) const
{
    CHECK_THIS_CONTEXT;
    return StaticBuffer_4_5::create(size, data);
}

std::shared_ptr<core::graphics::IDynamicBuffer> GLFWRenderer::createDynamicBuffer(size_t size, const void* data) const
{
    CHECK_THIS_CONTEXT;
    return DynamicBuffer_4_5::create(size, data);
}

std::shared_ptr<core::graphics::IVertexArray> GLFWRenderer::createVertexArray() const
{
    CHECK_THIS_CONTEXT;
    return VertexArray_4_5::create();
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1DEmpty(uint32_t width,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT;
    return Texture1D_4_5::createEmpty(width, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1D(const std::shared_ptr<const utils::Image> &image,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    CHECK_THIS_CONTEXT;
    return Texture1D_4_5::create(image, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2DEmpty(uint32_t width,
                                                                                      uint32_t height,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT;
    return Texture2D_4_5::createEmpty(width, height, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2D(const std::shared_ptr<const utils::Image> &image,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    CHECK_THIS_CONTEXT;
    return Texture2D_4_5::create(image, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture3DEmpty(uint32_t width,
                                                                                      uint32_t height,
                                                                                      uint32_t depth,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT;
    return Texture3D_4_5::createEmpty(width, height, depth, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture3D(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    CHECK_THIS_CONTEXT;
    return Texture3D_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCubeEmpty(uint32_t width,
                                                                                        uint32_t height,
                                                                                        core::graphics::PixelInternalFormat internalFormat,
                                                                                        uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT;
    return TextureCube_4_5::createEmpty(width, height, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCube(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                   core::graphics::PixelInternalFormat internalFormat,
                                                                                   uint32_t numLevels,
                                                                                   bool genMipmaps) const
{
    CHECK_THIS_CONTEXT;
    return TextureCube_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1DArrayEmpty(uint32_t width,
                                                                                           uint32_t numLayers,
                                                                                           core::graphics::PixelInternalFormat internalFormat,
                                                                                           uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT;
    return Texture1DArray_4_5::createEmpty(width, numLayers, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture1DArray(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels,
                                                                                      bool genMipmaps) const
{
    CHECK_THIS_CONTEXT;
    return Texture1DArray_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2DArrayEmpty(uint32_t width,
                                                                                           uint32_t height,
                                                                                           uint32_t numLayers,
                                                                                           core::graphics::PixelInternalFormat internalFormat,
                                                                                           uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT;
    return Texture2DArray_4_5::createEmpty(width, height, numLayers, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTexture2DArray(const std::vector<std::shared_ptr<const utils::Image>> &images,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels,
                                                                                      bool genMipmaps) const
{
    CHECK_THIS_CONTEXT;
    return Texture2DArray_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCubeArrayEmpty(uint32_t width,
                                                                                             uint32_t height,
                                                                                             uint32_t numLayers,
                                                                                             core::graphics::PixelInternalFormat internalFormat,
                                                                                             uint32_t numLevels) const
{
    CHECK_THIS_CONTEXT;
    return TextureCubeArray_4_5::createEmpty(width, height, numLayers, internalFormat, numLevels);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureCubeArray(const std::vector<std::vector<std::shared_ptr<const utils::Image>>> &images,
                                                                                        core::graphics::PixelInternalFormat internalFormat,
                                                                                        uint32_t numLevels,
                                                                                        bool genMipmaps) const
{
    CHECK_THIS_CONTEXT;
    return TextureCubeArray_4_5::create(images, internalFormat, numLevels, genMipmaps);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureRectEmpty(uint32_t width,
                                                                                        uint32_t height,
                                                                                        core::graphics::PixelInternalFormat internalFormat) const
{
    CHECK_THIS_CONTEXT;
    return TextureRect_4_5::createEmpty(width, height, internalFormat);
}

std::shared_ptr<core::graphics::ITexture> GLFWRenderer::createTextureRect(const std::shared_ptr<const utils::Image> &image,
                                                                                   core::graphics::PixelInternalFormat internalFormat) const
{
    CHECK_THIS_CONTEXT;
    return TextureRect_4_5::create(image, internalFormat);
}

std::shared_ptr<core::graphics::ITextureHandle> GLFWRenderer::createTextureHandle(
    const core::graphics::PConstTexture& texture) const
{
    CHECK_THIS_CONTEXT;
    return TextureHandle_4_5::create(texture);
}

std::shared_ptr<core::graphics::IImageHandle> GLFWRenderer::createImageHandle(
    const core::graphics::PConstImage& image) const
{
    CHECK_THIS_CONTEXT;
    return ImageHandle_4_5::create(image);
}

std::shared_ptr<core::graphics::IRenderBuffer> GLFWRenderer::createRenderBuffer(uint32_t width,
                                                                                         uint32_t height,
                                                                                         core::graphics::PixelInternalFormat internalFormat) const
{
    CHECK_THIS_CONTEXT;
    return RenderBuffer_4_5::create(width, height, internalFormat);
}

std::shared_ptr<core::graphics::IFrameBuffer> GLFWRenderer::createFrameBuffer() const
{
    CHECK_THIS_CONTEXT;
    return FrameBuffer_4_5::create();
}

std::shared_ptr<core::graphics::IRenderProgram> GLFWRenderer::createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                                           const std::shared_ptr<utils::Shader> &fragmentShader) const
{
    CHECK_THIS_CONTEXT;
    return RenderProgram_4_5::create(vertexShader, fragmentShader);
}

std::shared_ptr<core::graphics::IRenderProgram> GLFWRenderer::createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                                           const std::shared_ptr<utils::Shader> &geometryShader,
                                                                                           const std::shared_ptr<utils::Shader> &fragmentShader) const
{
    CHECK_THIS_CONTEXT;
    return RenderProgram_4_5::create(vertexShader, geometryShader, fragmentShader);
}

std::shared_ptr<core::graphics::IComputeProgram> GLFWRenderer::createComputeProgram(const std::shared_ptr<utils::Shader> &computeShader) const
{
    CHECK_THIS_CONTEXT;
    return ComputeProgram_4_5::create(computeShader);
}

void GLFWRenderer::compute(
    const glm::uvec3& numInvocations,
    const std::shared_ptr<core::graphics::IComputeProgram>& computeProgram,
    const core::StateSetList& stateSetList)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupCompute(computeProgram, stateSetList);

    auto numWorkGroups = glm::uvec3(glm::ceil(glm::vec3(numInvocations) / glm::vec3(computeProgram->workGroupSize())) + glm::vec3(.5f));
    glDispatchCompute(numWorkGroups.x, numWorkGroups.y, numWorkGroups.z);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void GLFWRenderer::drawArrays(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList, 
    utils::PrimitiveType primitiveType,
    size_t first,
    size_t count)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glDrawArrays(
        Conversions::PrimitiveType2GL(primitiveType),
        static_cast<GLint>(first),
        static_cast<GLsizei>(count));
}

void GLFWRenderer::drawElements(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    size_t count,
    utils::DrawElementsIndexType indexType,
    size_t offset)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glDrawElements(
        Conversions::PrimitiveType2GL(primitiveType),
        static_cast<GLsizei>(count),
        Conversions::DrawElementsIndexType2GL(indexType),
        reinterpret_cast<const void*>(offset));
}

void GLFWRenderer::multiDrawArrays(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    const std::vector<size_t>& firsts,
    const std::vector<size_t>& counts)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    const auto count = firsts.size();
    if (counts.size() != count)
    {
        LOG_CRITICAL << "The arrays \"firsts\" and \"counts\" have different sizes";
        return;
    }

    std::vector<GLint> GLFirsts(count);
    std::vector<GLsizei> GLCounts(count);
    for (size_t i = 0u; i < count; ++i)
    {
        GLFirsts[i] = static_cast<GLint>(firsts[i]);
        GLCounts[i] = static_cast<GLsizei>(counts[i]);
    }

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glMultiDrawArrays(
        Conversions::PrimitiveType2GL(primitiveType),
        GLFirsts.data(),
        GLCounts.data(),
        static_cast<GLsizei>(count));
}

void GLFWRenderer::multiDrawElements(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    const std::vector<size_t>& counts,
    utils::DrawElementsIndexType indexType,
    const std::vector<size_t>& offsets)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    const auto count = counts.size();
    if (offsets.size() != count)
    {
        LOG_CRITICAL << "The arrays \"counts\" and \"offsets\" have different sizes";
        return;
    }

    std::vector<GLsizei> GLCounts(count);
    std::vector<void*> GLIndices(count);
    for (size_t i = 0u; i < count; ++i)
    {
        GLCounts[i] = static_cast<GLint>(counts[i]);
        GLIndices[i] = reinterpret_cast<void*>(offsets[i]);
    }

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glMultiDrawElements(
        Conversions::PrimitiveType2GL(primitiveType),
        GLCounts.data(),
        Conversions::DrawElementsIndexType2GL(indexType),
        GLIndices.data(),
        static_cast<GLsizei>(count));
}

void GLFWRenderer::drawElementsBaseVertex(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    size_t count,
    utils::DrawElementsIndexType indexType,
    size_t offset,
    uint32_t baseVertex)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glDrawElementsBaseVertex(
        Conversions::PrimitiveType2GL(primitiveType),
        static_cast<GLsizei>(count),
        Conversions::DrawElementsIndexType2GL(indexType),
        reinterpret_cast<const void*>(offset),
        static_cast<GLint>(baseVertex));
}

void GLFWRenderer::drawArraysInstanced(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    size_t first,
    size_t count,
    size_t numInstances)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glDrawArraysInstanced(
        Conversions::PrimitiveType2GL(primitiveType),
        static_cast<GLint>(first),
        static_cast<GLsizei>(count),
        static_cast<GLsizei>(numInstances));
}

void GLFWRenderer::drawElementsInstanced(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    size_t count,
    utils::DrawElementsIndexType indexType,
    size_t offset,
    size_t numInstances)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glDrawElementsInstanced(
        Conversions::PrimitiveType2GL(primitiveType),
        static_cast<GLsizei>(count),
        Conversions::DrawElementsIndexType2GL(indexType),
        reinterpret_cast<const void*>(offset),
        static_cast<GLsizei>(numInstances));
}

void GLFWRenderer::drawArraysInstancedBaseInstance(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    size_t first,
    size_t count,
    size_t numInstances,
    uint32_t baseInstance)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glDrawArraysInstancedBaseInstance(
        Conversions::PrimitiveType2GL(primitiveType),
        static_cast<GLint>(first),
        static_cast<GLsizei>(count),
        static_cast<GLsizei>(numInstances),
        static_cast<GLuint>(baseInstance));
}

void GLFWRenderer::drawElementsInstancedBaseInstance(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    size_t count,
    utils::DrawElementsIndexType indexType,
    size_t offset,
    size_t numInstances,
    uint32_t baseInstance)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);

    glDrawElementsInstancedBaseInstance(
        Conversions::PrimitiveType2GL(primitiveType),
        static_cast<GLsizei>(count),
        Conversions::DrawElementsIndexType2GL(indexType),
        reinterpret_cast<const void*>(offset),
        static_cast<GLsizei>(numInstances),
        static_cast<GLuint>(baseInstance));
}

void GLFWRenderer::drawArraysIndirect(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    const core::graphics::PDrawArraysIndirectCommandsConstBuffer& commandsBuffer)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);
    bindDrawIndirectBuffer(commandsBuffer->buffer());

    glDrawArraysIndirect(
        Conversions::PrimitiveType2GL(primitiveType),
        nullptr);
}

void GLFWRenderer::drawElementsIndirect(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    utils::DrawElementsIndexType indexType,
    const core::graphics::PDrawElementsIndirectCommandConstBuffer& commandsBuffer)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);
    bindDrawIndirectBuffer(commandsBuffer->buffer());

    glDrawElementsIndirect(
        Conversions::PrimitiveType2GL(primitiveType),
        Conversions::DrawElementsIndexType2GL(indexType),
        nullptr);
}

void GLFWRenderer::multiDrawArraysIndirect(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    const core::graphics::PDrawArraysIndirectCommandsConstBuffer& commandsBuffer)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);
    bindDrawIndirectBuffer(commandsBuffer->buffer());

    glMultiDrawArraysIndirect(
        Conversions::PrimitiveType2GL(primitiveType),
        nullptr,
        static_cast<GLsizei>(commandsBuffer->size()),
        static_cast<GLsizei>(0u));
}

void GLFWRenderer::multiDrawElementsIndirect(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    utils::DrawElementsIndexType indexType,
    const core::graphics::PDrawElementsIndirectCommandConstBuffer& commandsBuffer)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);
    bindDrawIndirectBuffer(commandsBuffer->buffer());

    glMultiDrawElementsIndirect(
        Conversions::PrimitiveType2GL(primitiveType),
        Conversions::DrawElementsIndexType2GL(indexType),
        nullptr,
        static_cast<GLsizei>(commandsBuffer->size()),
        static_cast<GLsizei>(0u));
}

void GLFWRenderer::multiDrawArraysIndirectCount(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    const core::graphics::PDrawArraysIndirectCommandsConstBuffer& commandsBuffer,
    const core::graphics::PConstBufferRange& parameterBuffer)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);
    bindDrawIndirectBuffer(commandsBuffer->buffer());
    bindParameterBuffer(parameterBuffer->buffer());

    glMultiDrawArraysIndirectCountARB(
        Conversions::PrimitiveType2GL(primitiveType),
        nullptr,
        static_cast<GLintptr>(parameterBuffer->offset()),
        static_cast<GLsizei>(commandsBuffer->size()),
        static_cast<GLsizei>(0u));
}

void GLFWRenderer::multiDrawElementsIndirectCount(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebuffer,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList,
    utils::PrimitiveType primitiveType,
    utils::DrawElementsIndexType indexType,
    const core::graphics::PDrawElementsIndirectCommandConstBuffer& commandsBuffer,
    const core::graphics::PConstBufferRange& parameterBuffer)
{
    CHECK_THIS_CONTEXT;
    DEFAULT_SETUP;

    setupRender(viewport, renderProgram, framebuffer, VAO, stateSetList);
    bindDrawIndirectBuffer(commandsBuffer->buffer());
    bindParameterBuffer(parameterBuffer->buffer());

    glMultiDrawElementsIndirectCountARB(
        Conversions::PrimitiveType2GL(primitiveType),
        Conversions::DrawElementsIndexType2GL(indexType),
        nullptr,
        static_cast<GLintptr>(parameterBuffer->offset()),
        static_cast<GLsizei>(commandsBuffer->size()),
        static_cast<GLsizei>(0u));
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

void GLFWRenderer::setupCompute(
    const std::shared_ptr<core::graphics::IComputeProgram>& computeProgram,
    const core::StateSetList& stateSetList)
{
    CHECK_THIS_CONTEXT;

    auto computeProgram_4_5 = std::dynamic_pointer_cast<ComputeProgram_4_5>(computeProgram);
    if (!computeProgram_4_5)
        LOG_CRITICAL << "Compute program can't be nullptr";
    CHECK_RESOURCE_CONTEXT(computeProgram_4_5);
    glUseProgram(computeProgram_4_5->id());

    setupUniforms(computeProgram_4_5, stateSetList);
    setupShaderStorageBlocks(computeProgram_4_5, stateSetList);
}

void GLFWRenderer::setupRender(
    const glm::uvec4& viewport,
    const std::shared_ptr<core::graphics::IRenderProgram>& renderProgram,
    const std::shared_ptr<core::graphics::IFrameBuffer>& framebufferBase,
    const std::shared_ptr<core::graphics::IVertexArray>& VAO,
    const core::StateSetList& stateSetList)
{
    CHECK_THIS_CONTEXT;

    auto frameBufferBase_4_5 = std::dynamic_pointer_cast<FrameBufferBase_4_5>(framebufferBase);
    if (!frameBufferBase_4_5)
        LOG_CRITICAL << "Framebuffer can't be nullptr";
    CHECK_RESOURCE_CONTEXT(frameBufferBase_4_5);

    auto vao_4_5 = std::dynamic_pointer_cast<VertexArray_4_5>(VAO);
    if (!vao_4_5)
        LOG_CRITICAL << "VAO can't be nullptr";
    CHECK_RESOURCE_CONTEXT(vao_4_5);

    auto renderProgram_4_5 = std::dynamic_pointer_cast<RenderProgram_4_5>(renderProgram);
    if (!renderProgram_4_5)
        LOG_CRITICAL << "Render program can't be nullptr";
    CHECK_RESOURCE_CONTEXT(renderProgram_4_5);
    glUseProgram(renderProgram_4_5->id());

    setupFramebuffer(renderProgram_4_5, frameBufferBase_4_5);
    setupVAO(renderProgram_4_5, vao_4_5);

    setupUniforms(renderProgram_4_5, stateSetList);
    setupShaderStorageBlocks(renderProgram_4_5, stateSetList);
    setupUniformBlocks(renderProgram_4_5, stateSetList);

    glViewport(
        static_cast<GLint>(viewport.x),
        static_cast<GLint>(viewport.y),
        static_cast<GLsizei>(viewport.z),
        static_cast<GLsizei>(viewport.w));
}

void GLFWRenderer::setupFramebuffer(
    const std::shared_ptr<const RenderProgram_4_5>& renderProgram,
    const std::shared_ptr<FrameBufferBase_4_5>& framebuffer)
{
    CHECK_THIS_CONTEXT;
    CHECK_RESOURCE_CONTEXT(renderProgram);
    CHECK_RESOURCE_CONTEXT(framebuffer);

    if (framebuffer->faceCulling())
    {
        glCullFace(Conversions::FaceType2GL(framebuffer->cullFaceType()));
        glEnable(GL_CULL_FACE);
    }
    else
        glDisable(GL_CULL_FACE);

    for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    {
        GLboolean mask = framebuffer->colorMask(i);
        glColorMaski(i, mask, mask, mask, mask);
    }

    if (framebuffer->depthTest())
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(Conversions::ComparingFunc2GL(framebuffer->depthFunc()));
        glDepthMask(framebuffer->depthMask());
    }
    else
        glDisable(GL_DEPTH_TEST);

    if (framebuffer->stencilTest())
    {
        glEnable(GL_STENCIL_TEST);
        for (const auto& face : { core::graphics::FaceType::Front, core::graphics::FaceType::Back })
        {
            const auto& operations = framebuffer->stencilOperations(face);

            glStencilOpSeparate(Conversions::FaceType2GL(face),
                Conversions::StencilOperation2GL(operations[0]),
                Conversions::StencilOperation2GL(operations[1]),
                Conversions::StencilOperation2GL(operations[2]));

            glStencilFuncSeparate(Conversions::FaceType2GL(face),
                Conversions::ComparingFunc2GL(framebuffer->stencilComparingFunc(face)),
                framebuffer->stencilReferenceValue(face),
                framebuffer->stencilMaskValue(face));
        }
    }
    else
        glDisable(GL_STENCIL_TEST);

    if (framebuffer->blending())
    {
        glEnable(GL_BLEND);

        auto blendConstantColor = framebuffer->blendConstantColor();
        glBlendColor(blendConstantColor.r, blendConstantColor.g, blendConstantColor.b, framebuffer->blendConstantAlpha());

        for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
        {
            glBlendEquationSeparatei(i,
                Conversions::BlendEquetion2GL(framebuffer->blendColorEquation(i)),
                Conversions::BlendEquetion2GL(framebuffer->blendAlphaEquation(i)));
            glBlendFuncSeparatei(i,
                Conversions::BlendFactor2GL(framebuffer->blendColorSourceFactor(i)),
                Conversions::BlendFactor2GL(framebuffer->blendColorDestinationFactor(i)),
                Conversions::BlendFactor2GL(framebuffer->blendAlphaSourceFactor(i)),
                Conversions::BlendFactor2GL(framebuffer->blendAlphaDestinationFactor(i)));
        }
    }
    else
        glDisable(GL_BLEND);

    int32_t maxOuputLocation = -1;
    for (const auto& outputInfo : renderProgram->outputsInfo())
        maxOuputLocation = glm::max(maxOuputLocation, outputInfo.location);

    std::vector<GLenum> bufs(static_cast<size_t>(maxOuputLocation + 1), GL_NONE);
    for (const auto& outputInfo : renderProgram->outputsInfo())
    {
        if (outputInfo.ID == core::graphics::FrameBufferAttachment::Count)
            LOG_CRITICAL << "Output \"" << renderProgram->outputNameByIndex(outputInfo.index) << "\" is undefined";

        bufs[static_cast<size_t>(outputInfo.location)] =
            GL_COLOR_ATTACHMENT0 + core::graphics::FrameBufferColorAttachmentIndex(outputInfo.ID);
    }
    glNamedFramebufferDrawBuffers(framebuffer->id(), static_cast<GLsizei>(bufs.size()), bufs.data());

    if (!framebuffer->isComplete())
    {
        LOG_CRITICAL << "Frame buffer is not complete";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id());
}

void GLFWRenderer::setupVAO(
    const std::shared_ptr<const RenderProgram_4_5> &renderProgram,
    const std::shared_ptr<VertexArray_4_5> &vao)
{
    CHECK_THIS_CONTEXT;
    CHECK_RESOURCE_CONTEXT(renderProgram);
    CHECK_RESOURCE_CONTEXT(vao);

    for (const auto &attributeInfo : renderProgram->attributesInfo())
    {
        if (attributeInfo.ID == utils::VertexAttribute::Count)
            LOG_CRITICAL << "Attribute \"" << renderProgram->attributeNameByIndex(attributeInfo.index) << "\" is undefined";

        // TODO: component type can be different for integer vertex attributes
//        if (attributeInfo.componentType != vao->vertexAttributeComponentType(attributeInfo.id))
//            LOG_CRITICAL << "Vertex attribute \"" << renderProgram->attributeNameByIndex(attributeInfo.index) << "\" has wrong component type";

        if (attributeInfo.numComponents != vao->vertexAttributeNumComponents(attributeInfo.ID))
            LOG_CRITICAL << "Vertex attribute \"" << renderProgram->attributeNameByIndex(attributeInfo.index) << "\" has wrong number of components";
    
        vao->setupVertexAttrubute(attributeInfo.ID, attributeInfo.location);
    }

    glBindVertexArray(vao->id());
}

bool GLFWRenderer::setupUniform(
    GLuint rpId,
    core::graphics::UniformType type,
    GLint loc,
    int32_t &textureUnit,
    int32_t &imageUnit,
    const core::PConstAbstractUniform &abstactUniform)
{
    CHECK_THIS_CONTEXT;
    auto result = false;
    if (!abstactUniform)
    {
        LOG_CRITICAL << "Uniform can't be nullptr";
        return result;
    }

    switch (type)
    {
    case core::graphics::UniformType::Bool:
    {
        if (auto uniform = core::uniform_cast<bool>(abstactUniform))
        {
            glProgramUniform1i(rpId, loc, uniform->data());
            result = true;
        }
        break;
    }
    case core::graphics::UniformType::Single:
    {
        if (auto uniform = core::uniform_cast<float>(abstactUniform))
        {
            glProgramUniform1f(rpId, loc, uniform->data());
            result = true;
        }
        break;
    }
    case core::graphics::UniformType::SingleVec2:
    {
        if (auto uniform = core::uniform_cast<glm::vec2>(abstactUniform))
        {
            glProgramUniform2fv(rpId, loc, 1, glm::value_ptr(uniform->data()));
            result = true;
        }
        break;
    }
    case core::graphics::UniformType::SingleVec3:
    {
        if (auto uniform = core::uniform_cast<glm::vec3>(abstactUniform))
        {
        glProgramUniform3fv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::SingleVec4:
    {
        if (auto uniform = core::uniform_cast<glm::vec4>(abstactUniform))
        {
        glProgramUniform4fv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::SingleMat2:
    {
        if (auto uniform = core::uniform_cast<glm::mat2x2>(abstactUniform))
        {
        glProgramUniformMatrix2fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::SingleMat3:
    {
        if (auto uniform = core::uniform_cast<glm::mat3x3>(abstactUniform))
        {
        glProgramUniformMatrix3fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::SingleMat4:
    {
        if (auto uniform = core::uniform_cast<glm::mat4x4>(abstactUniform))
        {
        glProgramUniformMatrix4fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Double:
    {
        if (auto uniform = core::uniform_cast<double>(abstactUniform))
        {
        glProgramUniform1d(rpId, loc, uniform->data());
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::DoubleVec2:
    {
        if (auto uniform = core::uniform_cast<glm::dvec2>(abstactUniform))
        {
        glProgramUniform2dv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::DoubleVec3:
    {
        if (auto uniform = core::uniform_cast<glm::dvec3>(abstactUniform))
        {
        glProgramUniform3dv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::DoubleVec4:
    {
        if (auto uniform = core::uniform_cast<glm::dvec4>(abstactUniform))
        {
        glProgramUniform4dv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::DoubleMat2:
    {
        if (auto uniform = core::uniform_cast<glm::dmat2x2>(abstactUniform))
        {
        glProgramUniformMatrix2dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::DoubleMat3:
    {
        if (auto uniform = core::uniform_cast<glm::dmat3x3>(abstactUniform))
        {
        glProgramUniformMatrix3dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::DoubleMat4:
    {
        if (auto uniform = core::uniform_cast<glm::dmat4x4>(abstactUniform))
        {
        glProgramUniformMatrix4dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Int32:
    {
        if (auto uniform = core::uniform_cast<int32_t>(abstactUniform))
        {
        glProgramUniform1i(rpId, loc, uniform->data());
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Int32Vec2:
    {
        if (auto uniform = core::uniform_cast<glm::i32vec2>(abstactUniform))
        {
        glProgramUniform2iv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Int32Vec3:
    {
        if (auto uniform = core::uniform_cast<glm::i32vec3>(abstactUniform))
        {
        glProgramUniform3iv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Int32Vec4:
    {
        if (auto uniform = core::uniform_cast<glm::i32vec4>(abstactUniform))
        {
        glProgramUniform4iv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Uint32:
    {
        if (auto uniform = core::uniform_cast<uint32_t>(abstactUniform))
        {
        glProgramUniform1ui(rpId, loc, uniform->data());
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Uint32Vec2:
    {
        if (auto uniform = core::uniform_cast<glm::u32vec2>(abstactUniform))
        {
        glProgramUniform2uiv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Uint32Vec3:
    {
        if (auto uniform = core::uniform_cast<glm::u32vec3>(abstactUniform))
        {
        glProgramUniform3uiv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
        break;
    }
    case core::graphics::UniformType::Uint32Vec4:
    {
        if (auto uniform = core::uniform_cast<glm::u32vec4>(abstactUniform))
        {
        glProgramUniform4uiv(rpId, loc, 1, glm::value_ptr(uniform->data()));
        result = true;
    }
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
        if (auto textureUniform = core::uniform_cast<core::graphics::PConstTexture>(abstactUniform))
        {
            bindTexture(textureUnit, textureUniform->data());
            glProgramUniform1i(rpId, loc, textureUnit++);
            result = true;
        }
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
        if (auto imageUniform = core::uniform_cast<core::graphics::PConstImage>(abstactUniform))
        {
            bindImage(imageUnit, imageUniform->data());
            result = true;
        }
        else if (auto textureUniform = core::uniform_cast<core::graphics::PConstTexture>(abstactUniform))
        {
            bindImage(imageUnit, textureUniform->data(), 0u, core::graphics::Image::DataAccess::ReadWrite);
            result = true;
        }
        else
            LOG_CRITICAL << "Failed to bind image";

        glProgramUniform1i(rpId, loc, imageUnit++);
        break;
    }
    case core::graphics::UniformType::AtomicCounterUint:
    {
        if (auto atomicCounterUniform = core::uniform_cast<core::graphics::PConstBufferRange>(abstactUniform))
        {
            bindAtomicCounterBuffer(static_cast<GLuint>(loc), atomicCounterUniform->data());
            result = true;
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return result;
}

void GLFWRenderer::setupUniforms(const std::shared_ptr<ProgramBase_4_5> &program,
                                          const core::StateSetList &stateSetList)
{
    CHECK_THIS_CONTEXT;
    CHECK_RESOURCE_CONTEXT(program);
    int32_t textureUnit = 0;
    int32_t imageUnit = 0;

    for (const auto &uniformInfo : program->uniformsInfo())
    {
        core::PConstAbstractUniform abstractUniform;
        const auto uniformId = uniformInfo.ID;

        for (const auto &stateSet: stateSetList)
            if (abstractUniform = stateSet->uniform(uniformId); abstractUniform)
                break;

        if (!abstractUniform)
        {
            const auto uniformName = program->uniformNameByIndex(uniformInfo.index);
            for (const auto &stateSet: stateSetList)
                if (abstractUniform = stateSet->userUniform(uniformName); abstractUniform)
                    break;
        }

        if (!abstractUniform)
            LOG_CRITICAL << "Uniform \"" << program->uniformNameByIndex(uniformInfo.index) << "\" has not set";

        if (!setupUniform(program->id(), uniformInfo.type, uniformInfo.location, textureUnit, imageUnit, abstractUniform))
            LOG_CRITICAL << "Failed to setup uniform \"" << program->uniformNameByIndex(uniformInfo.index) << "\". It has wrong type";
    }
}

void GLFWRenderer::setupUniformBlocks(const std::shared_ptr<ProgramBase_4_5>& program, const core::StateSetList& stateSetList)
{
    CHECK_THIS_CONTEXT;
    CHECK_RESOURCE_CONTEXT(program);
    uint32_t bindingPoint = 0u;

    for (const auto& uniformBlockInfo : program->uniformBlocksInfo())
    {
        core::graphics::PConstBufferRange buffer;

        for (const auto& stateSet : stateSetList)
            if (buffer = stateSet->uniformBlock(uniformBlockInfo.ID); buffer)
                break;

        if (!buffer)
        {
            const auto name = program->uniformNameByIndex(uniformBlockInfo.index);
            //            for (const auto &stateSet: stateSetList)
            //                if (bufferRange = stateSet->userShaderStorageBlock(name); bufferRange)
            //                    break;
        }

        if (!buffer)
            LOG_CRITICAL << "Uniform block \"" << program->uniformNameByIndex(uniformBlockInfo.index) << "\" has not set";


        bindUniformBlock(bindingPoint, buffer);
        glUniformBlockBinding(program->id(), uniformBlockInfo.index, bindingPoint++);
    }
}

void GLFWRenderer::setupShaderStorageBlocks(
    const std::shared_ptr<ProgramBase_4_5> &program,
    const core::StateSetList &stateSetList)
{
    CHECK_THIS_CONTEXT;
    CHECK_RESOURCE_CONTEXT(program);
    uint32_t bindingPoint = 0u;

    for (const auto &shaderStorageBlockInfo : program->shaderStorageBlocksInfo())
    {
        core::graphics::PConstBufferRange buffer;

        for (const auto &stateSet: stateSetList)
            if (buffer = stateSet->shaderStorageBlock(shaderStorageBlockInfo.ID); buffer)
                break;

        if (!buffer)
        {
            const auto name = program->shaderStorageBlockNameByIndex(shaderStorageBlockInfo.index);
//            for (const auto &stateSet: stateSetList)
//                if (bufferRange = stateSet->userShaderStorageBlock(name); bufferRange)
//                    break;
        }

        if (!buffer)
            LOG_CRITICAL << "Shader storage block \"" << program->shaderStorageBlockNameByIndex(shaderStorageBlockInfo.index) << "\" has not set";


        bindShaderStorageBlock(bindingPoint, buffer);
        glShaderStorageBlockBinding(program->id(), shaderStorageBlockInfo.index, bindingPoint++);
    }
}

void GLFWRenderer::bindTexture(int32_t unit, const core::graphics::PConstTexture &texture)
{
    CHECK_THIS_CONTEXT;
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(texture);
    if (!textureBase)
        LOG_CRITICAL << "Texture can't be nullptr";
    CHECK_RESOURCE_CONTEXT(textureBase);

    glBindTextureUnit(static_cast<GLuint>(unit), textureBase->id());
}

void GLFWRenderer::bindImage(int32_t unit, const core::graphics::PConstImage &image)
{
    CHECK_THIS_CONTEXT;
    bindImage(unit, image->texture(), image->mipmapLevel(), image->access());
}

void GLFWRenderer::bindImage(
    int32_t unit,
    const core::graphics::PConstTexture& texture,
    uint32_t level,
    core::graphics::Image::DataAccess access)
{
    CHECK_THIS_CONTEXT;
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(texture);
    if (!textureBase)
        LOG_CRITICAL << "Image's texture can't be nullptr";
    CHECK_RESOURCE_CONTEXT(textureBase);

    glBindImageTexture(static_cast<GLuint>(unit),
        textureBase->id(),
        static_cast<GLint>(level),
        GL_TRUE,
        0u,
        Conversions::ImageDataAccess2GL(access),
        Conversions::PixelInternalFormat2GL(texture->internalFormat()));
}

void GLFWRenderer::bindBuffer(
    GLenum target,
    const core::graphics::PConstBuffer& buffer)
{
    CHECK_THIS_CONTEXT;
    auto buffer_4_5 = std::dynamic_pointer_cast<const BufferBase_4_5>(buffer);
    if (!buffer_4_5)
        LOG_CRITICAL << "Buffer can't be nullptr";
    CHECK_RESOURCE_CONTEXT(buffer_4_5);

    glBindBuffer(target, buffer_4_5->id());
}

void GLFWRenderer::bindBufferRange(
    GLenum target,
    GLuint bindingPoint,
    const core::graphics::PConstBufferRange &bufferRange)
{
    CHECK_THIS_CONTEXT;
    auto buffer_4_5 = std::dynamic_pointer_cast<const BufferBase_4_5>(bufferRange->buffer());
    if (!buffer_4_5)
        LOG_CRITICAL << "Buffer can't be nullptr";
    CHECK_RESOURCE_CONTEXT(buffer_4_5);

    glBindBufferRange(target,
                      bindingPoint,
                      buffer_4_5->id(),
                      static_cast<GLintptr>(bufferRange->offset()),
                      static_cast<GLsizeiptr>(bufferRange->size()));
}

void GLFWRenderer::bindUniformBlock(uint32_t bindingPoint, const core::graphics::PConstBufferRange& bufferRange)
{
    CHECK_THIS_CONTEXT;
    bindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, bufferRange);
}

void GLFWRenderer::bindShaderStorageBlock(
    uint32_t bindingPoint,
    const core::graphics::PConstBufferRange &bufferRange)
{
    CHECK_THIS_CONTEXT;
    bindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferRange);
}

void GLFWRenderer::bindAtomicCounterBuffer(
    GLuint bindingPoint,
    const core::graphics::PConstBufferRange &bufferRange)
{
    CHECK_THIS_CONTEXT;
    bindBufferRange(GL_ATOMIC_COUNTER_BUFFER, bindingPoint, bufferRange);
}

void GLFWRenderer::bindDrawIndirectBuffer(const core::graphics::PConstBuffer& buffer)
{
    CHECK_THIS_CONTEXT;
    bindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);
}

void GLFWRenderer::bindParameterBuffer(const core::graphics::PConstBuffer& buffer)
{
    CHECK_THIS_CONTEXT;
    bindBuffer(GL_PARAMETER_BUFFER, buffer);
}

}
}
