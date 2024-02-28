#include <array>
#include <functional>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/glm/gtx/texture.hpp>
#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/image.h>
#include <utils/shader.h>

#include <core/renderinfo.h>
#include <core/drawable.h>
#include <core/uniform.h>

#include "qtopengl_4_5_renderer.h"

namespace simplex
{
namespace qt
{

std::unordered_map<QOpenGLContext*, std::weak_ptr<QtOpenGL_4_5_Renderer>> QtOpenGL_4_5_Renderer::s_instances;

// Conversions

GLenum Conversions::PrimitiveType2GL(utils::PrimitiveType value)
{
    static std::array<GLenum, utils::numElementsPrimitiveType()> s_table {
        GL_NONE,
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
        GL_INT,
        GL_UNSIGNED_INT
    };

    return s_table[utils::castFromVertexComponentType(value)];
}

GLenum Conversions::DrawElementsIndexType2GL(utils::DrawElementsIndexType value)
{
    static std::array<GLenum, utils::numElementsDrawElementsIndexType()> s_table {
        GL_NONE,
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
        GL_NONE,
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

// Buffer_4_5::MappedData_4_5

Buffer_4_5::MappedData_4_5::MappedData_4_5(std::weak_ptr<const Buffer_4_5> mappedBuffer, uint8_t *data)
    : m_mappedBuffer(mappedBuffer)
    , m_data(data)
{
}

Buffer_4_5::MappedData_4_5::~MappedData_4_5()
{
    if (!m_mappedBuffer.expired())
    {
        auto buffer_4_5 = m_mappedBuffer.lock();
        auto instance = QtOpenGL_4_5_Renderer::instance();
        instance->glUnmapNamedBuffer(buffer_4_5->id());
        buffer_4_5->m_isMapped = false;
    }
}

const uint8_t *Buffer_4_5::MappedData_4_5::get() const
{
    return const_cast<MappedData_4_5*>(this)->get();
}

uint8_t *Buffer_4_5::MappedData_4_5::get()
{
    return m_mappedBuffer.expired() ? nullptr : m_data;
}

// Buffer_4_5

Buffer_4_5::Buffer_4_5(uint64_t size, const void *data)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateBuffers(1, &m_id);
    Buffer_4_5::resize(size, data);
}

Buffer_4_5::~Buffer_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteBuffers(1, &m_id);
}

GLuint Buffer_4_5::id() const
{
    return m_id;
}

size_t Buffer_4_5::size() const
{
    GLint result;
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetNamedBufferParameteriv(m_id, GL_BUFFER_SIZE, &result);
    return static_cast<size_t>(result);
}

void Buffer_4_5::resize(size_t size, const void *data)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glNamedBufferData(m_id, static_cast<GLsizei>(size), data, GL_STATIC_DRAW);
}

std::unique_ptr<const core::graphics::IBuffer::MappedData> Buffer_4_5::map(MapAccess access, size_t offset, size_t size) const
{
    return const_cast<Buffer_4_5*>(this)->map(access, offset, size);
}

std::unique_ptr<core::graphics::IBuffer::MappedData> Buffer_4_5::map(MapAccess access, size_t offset, size_t size)
{
    if (m_isMapped)
    {
        LOG_ERROR << "Buffer is already mapped";
        return nullptr;
    }

    if (size == 0)
        size = Buffer_4_5::size() - offset;

    m_isMapped = true;
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    return std::make_unique<MappedData_4_5>(
                shared_from_this(),
                static_cast<uint8_t*>(renderer->glMapNamedBufferRange(m_id,
                                                                      static_cast<GLintptr>(offset),
                                                                      static_cast<GLsizei>(size),
                                                                      Conversions::BufferMapAccess2GL(access))));
}

// BufferRange_4_5

BufferRange_4_5::BufferRange_4_5(std::shared_ptr<core::graphics::IBuffer> buffer, size_t offset, size_t size)
    : m_buffer(buffer)
    , m_offset(offset)
    , m_size(size)
{
}

BufferRange_4_5::~BufferRange_4_5() = default;

std::shared_ptr<const core::graphics::IBuffer> BufferRange_4_5::buffer() const
{
    return m_buffer;
}

std::shared_ptr<core::graphics::IBuffer> BufferRange_4_5::buffer()
{
    return m_buffer;
}

size_t BufferRange_4_5::offset() const
{
    return m_offset;
}

size_t BufferRange_4_5::size() const
{
    return m_size;
}

// VertexArray_4_5

VertexArray_4_5::VertexArray_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateVertexArrays(1, &m_id);
}

VertexArray_4_5::~VertexArray_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteVertexArrays(1, &m_id);
}

GLuint VertexArray_4_5::id() const
{
    return m_id;
}

uint32_t VertexArray_4_5::attachVertexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer, size_t offset, uint32_t stride)
{
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

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayVertexBuffer(m_id, static_cast<GLuint>(bindingIndex), buffer_4_5->id(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));

    return bindingIndex;
}

void VertexArray_4_5::detachVertexBuffer(uint32_t bindingIndex)
{
    m_vertexBuffers[bindingIndex] = std::make_tuple(nullptr, static_cast<size_t>(0), static_cast<uint32_t>(0));

    for (const auto &[attrib, tuple]: m_vertexDeclarations)
        if (std::get<0>(tuple) == bindingIndex)
            undeclareVertexAttribute(attrib);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayVertexBuffer(m_id, static_cast<GLuint>(bindingIndex), 0, 0, 0);

}

std::shared_ptr<const core::graphics::IBuffer> VertexArray_4_5::vertexBuffer(uint32_t bindingIndex) const
{
    return std::get<0>(m_vertexBuffers[bindingIndex]);
}

size_t VertexArray_4_5::vertexBufferOffset(uint32_t bindingIndex) const
{
    return std::get<1>(m_vertexBuffers[bindingIndex]);
}

uint32_t VertexArray_4_5::vertexBufferStride(uint32_t bindingIndex) const
{
    return std::get<2>(m_vertexBuffers[bindingIndex]);
}

void VertexArray_4_5::declareVertexAttribute(utils::VertexAttribute attrib,
                                             uint32_t bindingIndex,
                                             uint32_t numComponents,
                                             utils::VertexComponentType type,
                                             uint32_t relativeOffset)
{
    if (numComponents < 1 || numComponents > 4)
        LOG_CRITICAL << "Num components must be [1..4]";

    if (type != core::graphics::IRenderProgram::attributeVertexComponentTypeByAttributeId(attrib))
        LOG_CRITICAL << "Vertex attribute has wrong component type";

    m_vertexDeclarations[attrib] = std::make_tuple(bindingIndex, numComponents, type, relativeOffset);

    auto loc = static_cast<GLuint>(attrib);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayAttribBinding(m_id, loc, static_cast<GLuint>(bindingIndex));
    renderer->glEnableVertexArrayAttrib(m_id, loc);

    switch (type)
    {
    case utils::VertexComponentType::Single: {
        renderer->glVertexArrayAttribFormat(m_id,
                                            loc,
                                            static_cast<GLint>(numComponents),
                                            Conversions::VertexComponentType2GL(type),
                                            GL_FALSE,
                                            static_cast<GLuint>(relativeOffset));
        break;
    }
    case utils::VertexComponentType::Double: {
        renderer->glVertexArrayAttribLFormat(m_id,
                                             loc,
                                             static_cast<GLint>(numComponents),
                                             Conversions::VertexComponentType2GL(type),
                                             static_cast<GLuint>(relativeOffset));
        break;
    }
    case utils::VertexComponentType::Int32:
    case utils::VertexComponentType::Uint32: {
        renderer->glVertexArrayAttribIFormat(m_id,
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
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDisableVertexArrayAttrib(m_id, static_cast<GLuint>(attrib));

    m_vertexDeclarations.erase(attrib);
}

uint32_t VertexArray_4_5::vertexAttributeBindingIndex(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<0>(it->second) : static_cast<uint32_t>(-1);
}

uint32_t VertexArray_4_5::vertexAttributeNumComponents(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<1>(it->second) : 0u;
}

utils::VertexComponentType VertexArray_4_5::vertexAttributeComponentType(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<2>(it->second) : utils::VertexComponentType::Count;
}

uint32_t VertexArray_4_5::vertexAttributeRelativeOffset(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<3>(it->second) : 0u;
}

void VertexArray_4_5::attachIndexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer)
{
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    if (!buffer_4_5)
        LOG_CRITICAL << "Buffer can't be nullptr";

    m_indexBuffer = buffer_4_5;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayElementBuffer(m_id, buffer_4_5->id());
}

void VertexArray_4_5::detachIndexBuffer()
{
    m_indexBuffer = nullptr;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayElementBuffer(m_id, 0);
}

std::shared_ptr<const core::graphics::IBuffer> VertexArray_4_5::indexBuffer() const
{
    return m_indexBuffer;
}

void VertexArray_4_5::addPrimitiveSet(std::shared_ptr<utils::PrimitiveSet> primitiveSet)
{
    if (!primitiveSet)
        LOG_CRITICAL << "Primitive set can't be nullptr";

    m_primitiveSets.insert(primitiveSet);
}

void VertexArray_4_5::removePrimitiveSet(std::shared_ptr<utils::PrimitiveSet> primitiveSet)
{
    m_primitiveSets.erase(primitiveSet);
}

const std::unordered_set<std::shared_ptr<utils::PrimitiveSet>> &VertexArray_4_5::primitiveSets() const
{
    return m_primitiveSets;
}

// TextureBase_4_5

TextureBase_4_5::TextureBase_4_5()
    : m_id(0u)
{
}

TextureBase_4_5::~TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteTextures(1, &m_id);
}

GLuint TextureBase_4_5::id() const
{
    return m_id;
}

void TextureBase_4_5::setSwizzleMask(const core::graphics::TextureSwizzleMask &value)
{
    const std::array<GLint, 4u> glValues {
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[0u])),
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[1u])),
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[2u])),
        static_cast<GLint>(Conversions::TextureSwizzle2GL(value[3u])),
    };

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_RGBA, glValues.data());
}

glm::uvec2 TextureBase_4_5::size() const
{
    return mipmapSize(0u);
}

core::graphics::PixelInternalFormat TextureBase_4_5::internalFormat() const
{
    return Conversions::GL2PixelInternalFormat(GLinternalFormat());
}

bool TextureBase_4_5::hasAlpha() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_ALPHA_SIZE, &result);

    return result != 0;
}

glm::uvec3 TextureBase_4_5::mipmapSize(uint32_t level) const
{
    GLint w, h, d;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_WIDTH, &w);
    renderer->glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_HEIGHT, &h);
    renderer->glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_DEPTH, &d);

    return glm::uvec3(w, h, d);
}

uint32_t TextureBase_4_5::numMipmapLevels() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureParameteriv(m_id, GL_TEXTURE_MAX_LEVEL, &result);

    return static_cast<uint32_t>(result);
}

void TextureBase_4_5::generateMipmaps()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGenerateTextureMipmap(m_id);
}

void TextureBase_4_5::setBorderColor(const glm::vec4 &value)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
}

void TextureBase_4_5::setWrapMode(core::graphics::TextureWrapMode value)
{
    auto glValue = static_cast<GLint>(Conversions::TextureWrapMode2GL(value));

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, glValue);
    renderer->glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, glValue);
    renderer->glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, glValue);
}

void TextureBase_4_5::setFilterMode(core::graphics::TextureFilterMode value)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    switch (value)
    {
    case core::graphics::TextureFilterMode::Point: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    }
    case core::graphics::TextureFilterMode::Linear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    }
    case core::graphics::TextureFilterMode::Bilinear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    }
    case core::graphics::TextureFilterMode::Trilinear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
    default: break;
    }
}

GLenum TextureBase_4_5::GLinternalFormat() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_INTERNAL_FORMAT, &result);

    return static_cast<GLenum>(result);
}

// Texture1D_4_5

Texture1D_4_5::Texture1D_4_5(uint32_t width, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_1D, 1, &m_id);
    renderer->glTextureStorage1D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width));
}

Texture1D_4_5::~Texture1D_4_5() = default;

core::graphics::TextureType Texture1D_4_5::type() const
{
    return core::graphics::TextureType::Type1D;
}

void Texture1D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage1D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x),
                                  static_cast<GLint>(size.x),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// Texture2D_4_5

Texture2D_4_5::Texture2D_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
    renderer->glTextureStorage2D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height));
}

Texture2D_4_5::~Texture2D_4_5() = default;

core::graphics::TextureType Texture2D_4_5::type() const
{
    return core::graphics::TextureType::Type2D;
}

void Texture2D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// Texture3D_4_5

Texture3D_4_5::Texture3D_4_5(uint32_t width, uint32_t height, uint32_t depth, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_3D, 1, &m_id);
    renderer->glTextureStorage3D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height),
                                 static_cast<GLsizei>(depth));
}

Texture3D_4_5::~Texture3D_4_5() = default;

core::graphics::TextureType Texture3D_4_5::type() const
{
    return core::graphics::TextureType::Type3D;
}

void Texture3D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// TextureCube_4_5

TextureCube_4_5::TextureCube_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
    renderer->glTextureStorage2D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height));
}

TextureCube_4_5::~TextureCube_4_5() = default;

core::graphics::TextureType TextureCube_4_5::type() const
{
    return core::graphics::TextureType::TypeCube;
}

void TextureCube_4_5::setSubImage(uint32_t level,const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if (offset.z + size.z > 6u)
        LOG_CRITICAL << "Number of cubemap faces must be 6";

    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// Texture1DArray_4_5

Texture1DArray_4_5::Texture1DArray_4_5(uint32_t width, uint32_t numLayers, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_1D_ARRAY, 1, &m_id);
    renderer->glTextureStorage2D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(numLayers));
}

Texture1DArray_4_5::~Texture1DArray_4_5() = default;

core::graphics::TextureType Texture1DArray_4_5::type() const
{
    return core::graphics::TextureType::Type1DArray;
}

void Texture1DArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// Texture2DArray_4_5

Texture2DArray_4_5::Texture2DArray_4_5(uint32_t width, uint32_t height, uint32_t numLayers, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_id);
    renderer->glTextureStorage3D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height),
                                 static_cast<GLsizei>(numLayers));
}

Texture2DArray_4_5::~Texture2DArray_4_5() = default;

core::graphics::TextureType Texture2DArray_4_5::type() const
{
    return core::graphics::TextureType::Type2DArray;
}

void Texture2DArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// TextureCubeArray_4_5

TextureCubeArray_4_5::TextureCubeArray_4_5(uint32_t width, uint32_t height, uint32_t numLayers, core::graphics::PixelInternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &m_id);
    renderer->glTextureStorage3D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height),
                                 static_cast<GLsizei>(6u * numLayers));
}

TextureCubeArray_4_5::~TextureCubeArray_4_5() = default;

core::graphics::TextureType TextureCubeArray_4_5::type() const
{
    return core::graphics::TextureType::TypeCubeArray;
}

void TextureCubeArray_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage3D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y), static_cast<GLint>(offset.z),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// TextureRect_4_5

TextureRect_4_5::TextureRect_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_RECTANGLE, 1, &m_id);
    renderer->glTextureStorage2D(m_id,
                                 1u,
                                 Conversions::PixelInternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height));
}

TextureRect_4_5::~TextureRect_4_5() = default;

core::graphics::TextureType TextureRect_4_5::type() const
{
    return core::graphics::TextureType::TypeRect;
}

void TextureRect_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::PixelComponentType type, const void *data)
{
    if (level != 0)
        LOG_CRITICAL << "Level must be 0 for TextureRect";

    if ((numComponents < 1) || (numComponents > 4))
        LOG_CRITICAL << "Num components must be in [1..4]";

    if (type == utils::PixelComponentType::Undefined)
        LOG_CRITICAL << "Undefined pixel component type";

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
}

// Image_4_5

Image_4_5::Image_4_5(std::shared_ptr<core::graphics::ITexture> texture, DataAccess access, uint32_t level)
    : m_texture(texture)
    , m_level(level)
    , m_access(access)
{
}

Image_4_5::~Image_4_5() = default;

std::shared_ptr<const core::graphics::ITexture> Image_4_5::texture() const
{
    return m_texture;
}

std::shared_ptr<core::graphics::ITexture> Image_4_5::texture()
{
    return m_texture;
}

core::graphics::IImage::DataAccess Image_4_5::access() const
{
    return m_access;
}

uint32_t Image_4_5::mipmapLevel() const
{
    return m_level;
}

// RenderBuffer_4_5

RenderBuffer_4_5::RenderBuffer_4_5(uint32_t width, uint32_t height, core::graphics::PixelInternalFormat internalFormat)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateRenderbuffers(1, &m_id);
    renderer->glNamedRenderbufferStorage(m_id,
                                         Conversions::PixelInternalFormat2GL(internalFormat),
                                         static_cast<GLsizei>(width),
                                         static_cast<GLsizei>(height));
}

RenderBuffer_4_5::~RenderBuffer_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteRenderbuffers(1, &m_id);
}

GLuint RenderBuffer_4_5::id() const
{
    return m_id;
}

glm::uvec2 RenderBuffer_4_5::size() const
{
    GLint width, height;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_WIDTH, &width);
    renderer->glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_HEIGHT, &height);

    return glm::uvec2(width, height);
}

core::graphics::PixelInternalFormat RenderBuffer_4_5::internalFormat() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_INTERNAL_FORMAT, &result);

    return Conversions::GL2PixelInternalFormat(static_cast<GLenum>(result));
}

bool RenderBuffer_4_5::hasAlpha() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_ALPHA_SIZE, &result);

    return result != 0;
}

// FrameBuffer_4_5

FrameBufferBase_4_5::FrameBufferBase_4_5(GLuint id)
    : m_id(id)
{
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
    return m_id;
}

void FrameBufferBase_4_5::clear()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    bool depthStensilWasCleared = false;
    for (auto attachment : m_clearMask)
    {
        if (core::graphics::IsFrameBufferColorAttachment(attachment) && m_attchments.count(attachment))
        {
            auto drawBuffer = core::graphics::FrameBufferColorAttachmentIndex(attachment);
            const auto &clearColor = m_clearColor[drawBuffer];
            switch (clearColor.first)
            {
            case core::graphics::FrameBufferClearColorType::Single: {
                renderer->glClearNamedFramebufferfv(m_id,
                                                    GL_COLOR,
                                                    static_cast<GLint>(drawBuffer),
                                                    glm::value_ptr(clearColor.second.floatColor));
                break;
            }
            case core::graphics::FrameBufferClearColorType::Int32: {
                renderer->glClearNamedFramebufferiv(m_id,
                                                    GL_COLOR,
                                                    static_cast<GLint>(drawBuffer),
                                                    glm::value_ptr(clearColor.second.intColor));
                break;
            }
            case core::graphics::FrameBufferClearColorType::Uint32: {
                renderer->glClearNamedFramebufferuiv(m_id,
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
            if (m_attchments.count(core::graphics::FrameBufferAttachment::DepthStencil))
            {
                if (!depthStensilWasCleared)
                {
                    GLint boundFBO;
                    renderer->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);
                    renderer->glBindFramebuffer(GL_FRAMEBUFFER, m_id);
                    renderer->glClearBufferfi(GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);//renderer->glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, m_clearDepth, m_clearStencil);
                    renderer->glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(boundFBO));
                }
                depthStensilWasCleared = true;
            }
            else if (m_attchments.count(core::graphics::FrameBufferAttachment::Depth))
            {
                renderer->glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &m_clearDepth);
            }
            continue;
        }

        if ((attachment == core::graphics::FrameBufferAttachment::Stencil) ||
            (attachment == core::graphics::FrameBufferAttachment::DepthStencil))
        {
            if (m_attchments.count(core::graphics::FrameBufferAttachment::DepthStencil))
            {
                if (!depthStensilWasCleared)
                {
                    GLint boundFBO;
                    renderer->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFBO);
                    renderer->glBindFramebuffer(GL_FRAMEBUFFER, m_id);
                    renderer->glClearBufferfi(GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);//renderer->glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, m_clearDepth, m_clearStencil);
                    renderer->glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(boundFBO));
                }
                depthStensilWasCleared = true;
            }
            else if (m_attchments.count(core::graphics::FrameBufferAttachment::Stencil))
            {
                renderer->glClearNamedFramebufferiv(m_id, GL_STENCIL, 0, &m_clearStencil);
            }
            continue;
        }
    }
}

bool FrameBufferBase_4_5::isComplete() const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return renderer->glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

const core::graphics::IFrameBuffer::Attachments &FrameBufferBase_4_5::attachments() const
{
    return m_attchments;
}

bool FrameBufferBase_4_5::attachment(core::graphics::FrameBufferAttachment value, AttachmentInfo &result) const
{
    auto it = m_attchments.find(value);
    if (it == m_attchments.end())
        return false;

    result = it->second;
    return true;
}

const core::graphics::FrameBufferClearColor &FrameBufferBase_4_5::clearColor(uint32_t index) const
{
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    return m_clearColor[index];
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::vec4 &value)
{
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.floatColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Single, clearColorValue};
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::i32vec4 &value)
{
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.intColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Int32, clearColorValue };
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::u32vec4 &value)
{
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    core::graphics::FrameBufferClearColorValue clearColorValue;
    clearColorValue.uintColor = value;
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Uint32, clearColorValue };
}

float FrameBufferBase_4_5::clearDepth() const
{
    return m_clearDepth;
}

int32_t FrameBufferBase_4_5::clearStencil() const
{
    return m_clearStencil;
}

void FrameBufferBase_4_5::setClearDepthStencil(float depth, uint8_t stencil)
{
    m_clearDepth = depth;
    m_clearStencil = stencil;
}

const std::unordered_set<core::graphics::FrameBufferAttachment> &FrameBufferBase_4_5::clearMask() const
{
    return m_clearMask;
}

void FrameBufferBase_4_5::setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment> &value)
{
    m_clearMask = value;
}

void FrameBufferBase_4_5::setDrawBuffers(const std::vector<core::graphics::FrameBufferAttachment> &attachments)
{
    std::vector<GLenum> buffers;
    for (const auto &attachment : attachments)
    {
        if (!core::graphics::IsFrameBufferColorAttachment(attachment))
            LOG_CRITICAL << "Not color attachment can be draw buffer";

        buffers.push_back(GL_COLOR_ATTACHMENT0 + core::graphics::FrameBufferColorAttachmentIndex(attachment));
    }

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glNamedFramebufferDrawBuffers(m_id,
                                            static_cast<GLsizei>(buffers.size()),
                                            buffers.data());
}

bool FrameBufferBase_4_5::faceCulling() const
{
    return m_faceCulling;
}

core::graphics::FaceType FrameBufferBase_4_5::cullFaceType() const
{
    return m_cullFaceType;
}

void FrameBufferBase_4_5::setFaceCulling(bool value, core::graphics::FaceType type)
{
    m_faceCulling = value;
    m_cullFaceType = type;
}

bool FrameBufferBase_4_5::colorMask(uint32_t index) const
{
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    return m_colorMasks[index];
}

void FrameBufferBase_4_5::setColorMask(uint32_t index, bool value)
{
    if (index >= core::graphics::FrameBufferColorAttachmentsCount())
        LOG_CRITICAL << "Index must be less than " << core::graphics::FrameBufferColorAttachmentsCount();

    m_colorMasks[index] = value;
}

void FrameBufferBase_4_5::setColorMasks(bool value)
{
    for (auto &mask : m_colorMasks)
        mask = value;
}

bool FrameBufferBase_4_5::depthTest() const
{
    return m_depthTest;
}

core::graphics::ComparingFunc FrameBufferBase_4_5::depthFunc() const
{
    return m_depthFunc;
}

void FrameBufferBase_4_5::setDepthTest(bool value, core::graphics::ComparingFunc func)
{
    m_depthTest = value;
    m_depthFunc = func;
}

bool FrameBufferBase_4_5::depthMask() const
{
    return m_depthMask;
}

void FrameBufferBase_4_5::setDepthMask(bool value)
{
    m_depthMask = value;
}

bool FrameBufferBase_4_5::stencilTest() const
{
    return m_stencilTest;
}

void FrameBufferBase_4_5::setStencilTest(bool value)
{
    m_stencilTest = value;
}

core::graphics::ComparingFunc FrameBufferBase_4_5::stencilComparingFunc(core::graphics::FaceType value) const
{
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
    return m_blending;
}

void FrameBufferBase_4_5::setBlending(bool value)
{
    m_blending = value;
}

core::graphics::BlendEquation FrameBufferBase_4_5::blendColorEquation(uint32_t index)
{
    return m_blendColorEquation[index];
}

core::graphics::BlendEquation FrameBufferBase_4_5::blendAlphaEquation(uint32_t index)
{
    return m_blendAlphaEquation[index];
}

void FrameBufferBase_4_5::setBlendEquation(uint32_t index, core::graphics::BlendEquation colorValue, core::graphics::BlendEquation alphaValue)
{
    m_blendColorEquation[index] = colorValue;
    m_blendAlphaEquation[index] = alphaValue;
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendColorSourceFactor(uint32_t index)
{
    return m_blendColorSourceFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendAlphaSourceFactor(uint32_t index)
{
    return m_blendAlphaSourceFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendColorDestinationFactor(uint32_t index)
{
    return m_blendColorDestFactor[index];
}

core::graphics::BlendFactor FrameBufferBase_4_5::blendAlphaDestinationFactor(uint32_t index)
{
    return m_blendAlphaDestFactor[index];
}

void FrameBufferBase_4_5::setBlendFactor(uint32_t index,
                                         core::graphics::BlendFactor colorSourceValue,
                                         core::graphics::BlendFactor colorDestValue,
                                         core::graphics::BlendFactor alphaSourceValue,
                                         core::graphics::BlendFactor alphaDestValue)
{
    m_blendColorSourceFactor[index] = colorSourceValue;
    m_blendColorDestFactor[index] = colorDestValue;
    m_blendAlphaSourceFactor[index] = alphaSourceValue;
    m_blendAlphaDestFactor[index] = alphaDestValue;
}

glm::vec3 FrameBufferBase_4_5::blendConstantColor() const
{
    return m_blendConstColor;
}

void FrameBufferBase_4_5::setBlendConstantColor(const glm::vec3 &value)
{
    m_blendConstColor = value;
}

float FrameBufferBase_4_5::blendConstantAlpha() const
{
    return m_blendConstAlpha;
}

void FrameBufferBase_4_5::setBlendConstantAlpha(float value)
{
    m_blendConstAlpha = value;
}

// FrameBuffer_4_5

FrameBuffer_4_5::FrameBuffer_4_5()
    : FrameBufferBase_4_5(0u)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateFramebuffers(1, &m_id);
}

FrameBuffer_4_5::~FrameBuffer_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer_4_5::attach(core::graphics::FrameBufferAttachment key,
                             std::shared_ptr<const core::graphics::ISurface> surface,
                             uint32_t level,
                             uint32_t layer)
{
    detach(key);

    if (auto texture = std::dynamic_pointer_cast<const TextureBase_4_5>(surface); texture)
    {
        auto renderer = QtOpenGL_4_5_Renderer::instance();
        if (false
//            || std::dynamic_pointer_cast<Texture3D_4_5>(texture)
//            || std::dynamic_pointer_cast<TextureCubemap_4_5>(texture)
//            || std::dynamic_pointer_cast<Texture1DArray_4_5>(texture)
//            || std::dynamic_pointer_cast<Texture2DArray_4_5>(texture)
//            || std::dynamic_pointer_cast<TextureCubemapArray_4_5>(texture)
           )
        {
            renderer->glNamedFramebufferTextureLayer(m_id,
                                                     Conversions::FrameBufferAttachment2GL(key),
                                                     texture->id(),
                                                     static_cast<GLint>(level),
                                                     static_cast<GLint>(layer));
        }
        else
        {
            renderer->glNamedFramebufferTexture(m_id, Conversions::FrameBufferAttachment2GL(key), texture->id(), static_cast<GLint>(level));
        }
        m_attchments[key] = { surface, level, layer };
    }
    else if (auto renderBuffer = std::dynamic_pointer_cast<const RenderBuffer_4_5>(surface); renderBuffer)
    {
        auto renderer = QtOpenGL_4_5_Renderer::instance();
        renderer->glNamedFramebufferRenderbuffer(m_id,
                                                 Conversions::FrameBufferAttachment2GL(key),
                                                 GL_RENDERBUFFER,
                                                 renderBuffer->id());
        m_attchments[key] = { surface, level, layer };
    }
    else
        LOG_CRITICAL << "Attachment can't be nullptr";
}

void FrameBuffer_4_5::detach(core::graphics::FrameBufferAttachment key)
{
    auto it = m_attchments.find(key);
    if (it != m_attchments.end())
    {
        auto renderer = QtOpenGL_4_5_Renderer::instance();
        renderer->glNamedFramebufferRenderbuffer(m_id,
                                                 Conversions::FrameBufferAttachment2GL(key),
                                                 GL_RENDERBUFFER,
                                                 0u);
        m_attchments.erase(it);
    }
}

// DefaultFrameBuffer_4_5

DefaultFrameBuffer_4_5::DefaultFrameBuffer_4_5(GLuint defaultFbo)
    : FrameBufferBase_4_5(defaultFbo)
{
}

DefaultFrameBuffer_4_5::~DefaultFrameBuffer_4_5()
{
}

void DefaultFrameBuffer_4_5::attach(core::graphics::FrameBufferAttachment, std::shared_ptr<const core::graphics::ISurface>, uint32_t, uint32_t)
{
}

void DefaultFrameBuffer_4_5::detach(core::graphics::FrameBufferAttachment)
{
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::vec4&)
{
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::i32vec4&)
{
}

void DefaultFrameBuffer_4_5::setClearColor(uint32_t, const glm::u32vec4&)
{
}

void DefaultFrameBuffer_4_5::setClearDepthStencil(float, uint8_t)
{
}

void DefaultFrameBuffer_4_5::setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment>&)
{
}

// ProgramBase_4_5

ProgramBase_4_5::ProgramBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    m_id = renderer->glCreateProgram();
}

ProgramBase_4_5::~ProgramBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLint count;
    renderer->glGetProgramiv(m_id, GL_ATTACHED_SHADERS, &count);

    std::vector<GLuint> shaders(static_cast<size_t>(count));
    GLsizei resCount;

    renderer->glGetAttachedShaders(m_id, count, &resCount, shaders.data());

    for (GLsizei i = 0; i < resCount; ++i)
    {
        GLuint shaderId = shaders[static_cast<size_t>(i)];
        renderer->glDetachShader(m_id, shaderId);
        renderer->glDeleteShader(shaderId);
    }

    renderer->glDeleteProgram(m_id);
}

GLuint ProgramBase_4_5::id() const
{
    return m_id;
}

bool ProgramBase_4_5::preBuild(std::string &)
{
    return true;
}

bool ProgramBase_4_5::postBuild(std::string &)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLint numUniforms;
    renderer->glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    m_uniformsInfo.reserve(static_cast<size_t>(numUniforms));

    renderer->glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_MAX_NAME_LENGTH, &m_uniformNameMaxLength);
    std::vector<GLchar> uniformName(static_cast<size_t>(m_uniformNameMaxLength));

    for (GLuint i = 0; i < static_cast<GLuint>(numUniforms); ++i)
    {
        static const std::array<GLenum, 3> properties {GL_BLOCK_INDEX, GL_TYPE, GL_LOCATION};
        static std::array<GLint, 3> values {};

        renderer->glGetProgramResourceiv(m_id,
                                         GL_UNIFORM,
                                         i,
                                         properties.size(),
                                         properties.data(),
                                         values.size(),
                                         nullptr,
                                         values.data());

        if (values[0] != -1) // Skip any uniforms that are in a block
            continue;

        if (values[1] == GL_UNSIGNED_INT_ATOMIC_COUNTER) // get atomic counter binding point instead of location
        {
            GLint atomicCounterBufferIndex;
            renderer->glGetActiveUniformsiv(m_id,
                                            1u,
                                            &i,
                                            GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX,
                                            &atomicCounterBufferIndex);

            renderer->glGetActiveAtomicCounterBufferiv(m_id,
                                                       static_cast<GLuint>(atomicCounterBufferIndex),
                                                       GL_ATOMIC_COUNTER_BUFFER_BINDING,
                                                       values.data() + 2u);
        }

        renderer->glGetProgramResourceName(m_id,
                                           GL_UNIFORM,
                                           i,
                                           m_uniformNameMaxLength,
                                           nullptr,
                                           uniformName.data());

        m_uniformsInfo.push_back({ core::graphics::IProgram::UniformIdByName(uniformName.data()),
                                   static_cast<uint16_t>(i),
                                   values[2],
                                   Conversions::GL2UniformType(static_cast<GLenum>(values[1])) });
    }

    GLint numSSBOs;
    renderer->glGetProgramInterfaceiv(m_id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numSSBOs);
    m_SSBOsInfo.reserve(static_cast<size_t>(numSSBOs));

    renderer->glGetProgramInterfaceiv(m_id, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &m_SSBONameMaxLength);
    std::vector<GLchar> SSBOName(static_cast<size_t>(m_SSBONameMaxLength));

    for (GLuint i = 0; i < static_cast<GLuint>(numSSBOs); ++i)
    {
        renderer->glGetProgramResourceName(m_id,
                                           GL_SHADER_STORAGE_BLOCK,
                                           i,
                                           m_SSBONameMaxLength,
                                           nullptr,
                                           SSBOName.data());

        m_SSBOsInfo.push_back({ core::graphics::IProgram::SSBOIdByName(SSBOName.data()),
                                static_cast<uint16_t>(i) });
    }

    return true;
}

int32_t ProgramBase_4_5::uniformLocationByName(const std::string &value) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return renderer->glGetProgramResourceLocation(m_id, GL_UNIFORM, value.data());
}

const std::vector<core::graphics::UniformInfo> &ProgramBase_4_5::uniformsInfo() const
{
    return m_uniformsInfo;
}

const std::vector<core::graphics::SSBOInfo> &ProgramBase_4_5::SSBOsInfo() const
{
    return m_SSBOsInfo;
}

std::string ProgramBase_4_5::uniformNameByIndex(uint16_t index) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    std::vector<GLchar> name(static_cast<size_t>(m_uniformNameMaxLength));
    renderer->glGetProgramResourceName(m_id,
                                       GL_UNIFORM,
                                       static_cast<GLuint>(index),
                                       m_uniformNameMaxLength,
                                       nullptr,
                                       name.data());

    return name.data();
}

std::string ProgramBase_4_5::SSBONameByIndex(uint16_t index) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    std::vector<GLchar> name(static_cast<size_t>(m_SSBONameMaxLength));
    renderer->glGetProgramResourceName(m_id,
                                       GL_SHADER_STORAGE_BLOCK,
                                       static_cast<GLuint>(index),
                                       m_SSBONameMaxLength,
                                       nullptr,
                                       name.data());

    return name.data();
}

// RenderProgram_4_5

RenderProgram_4_5::RenderProgram_4_5()
    : ProgramBase_4_5()
{
}

RenderProgram_4_5::~RenderProgram_4_5() = default;


bool RenderProgram_4_5::postBuild(std::string &log)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLint numAttributes;
    renderer->glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numAttributes);
    m_attributesInfo.reserve(static_cast<size_t>(numAttributes));

    renderer->glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_MAX_NAME_LENGTH, &m_attributeNameMaxLength);
    std::vector<GLchar> attributeName(static_cast<size_t>(m_attributeNameMaxLength));

    for (GLuint i = 0; i < static_cast<GLuint>(numAttributes); ++i)
    {
        static const std::array<GLenum, 2> properties {GL_TYPE, GL_LOCATION};
        static std::array<GLint, 2> values {};

        renderer->glGetProgramResourceiv(m_id,
                                         GL_PROGRAM_INPUT,
                                         i,
                                         properties.size(),
                                         properties.data(),
                                         values.size(),
                                         nullptr,
                                         values.data());

        renderer->glGetProgramResourceName(m_id,
                                           GL_PROGRAM_INPUT,
                                           i,
                                           m_attributeNameMaxLength,
                                           nullptr,
                                           attributeName.data());

        m_attributesInfo.push_back({ vertexAttributeByName(attributeName.data()),
                                     static_cast<uint16_t>(i),
                                     values[1],
                                     Conversions::GL2VertexNumComponents(static_cast<GLenum>(values[0])),
                                     Conversions::GL2VertexComponentType(static_cast<GLenum>(values[0])) });
    }

    return ProgramBase_4_5::postBuild(log);
}

int32_t RenderProgram_4_5::attributeLocationByName(const std::string &value) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return renderer->glGetProgramResourceLocation(m_id, GL_PROGRAM_INPUT, value.data());
}

const std::vector<core::graphics::AttributeInfo> &RenderProgram_4_5::attributesInfo() const
{
    return m_attributesInfo;
}

std::string RenderProgram_4_5::attributeNameByIndex(uint16_t index) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    std::vector<GLchar> name(static_cast<size_t>(m_attributeNameMaxLength));
    renderer->glGetProgramResourceName(m_id,
                                       GL_PROGRAM_INPUT,
                                       static_cast<GLuint>(index),
                                       m_attributeNameMaxLength,
                                       nullptr,
                                       name.data());

    return name.data();
}

// ComputeProgram_4_5

ComputeProgram_4_5::ComputeProgram_4_5()
    : ProgramBase_4_5()
{
}

ComputeProgram_4_5::~ComputeProgram_4_5() = default;


bool ComputeProgram_4_5::postBuild(std::string &log)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    std::array<GLint, 3> params;
    renderer->glGetProgramiv(m_id, GL_COMPUTE_WORK_GROUP_SIZE, params.data());
    m_workGroupSize = glm::uvec3(static_cast<uint32_t>(params[0]),
                                 static_cast<uint32_t>(params[1]),
                                 static_cast<uint32_t>(params[2]));

    return ProgramBase_4_5::postBuild(log);
}

glm::uvec3 ComputeProgram_4_5::workGroupSize() const
{
    return m_workGroupSize;
}

// QtOpenGL_4_5_Renderer

QtOpenGL_4_5_Renderer::~QtOpenGL_4_5_Renderer()
{
    auto context = owningContext();
    if (!context)
        LOG_CRITICAL << "Context is nullptr";

    if(!s_instances.count(context))
        LOG_CRITICAL << "Context is not founded";

    s_instances.erase(context);

    LOG_INFO << "GraphicsRenderer \"" << QtOpenGL_4_5_Renderer::name() << "\" has been destroyed";
}

const std::string &QtOpenGL_4_5_Renderer::name() const
{
    static const std::string s_name = "QtOpenGL_4_5_Renderer";
    return s_name;
}

std::shared_ptr<core::graphics::IFrameBuffer> QtOpenGL_4_5_Renderer::defaultFrameBuffer()
{
    return m_defaultFrameBuffer;
}

std::shared_ptr<const core::graphics::IFrameBuffer> QtOpenGL_4_5_Renderer::defaultFrameBuffer() const
{
    return const_cast<QtOpenGL_4_5_Renderer*>(this)->defaultFrameBuffer();
}

void QtOpenGL_4_5_Renderer::blitFrameBuffer(std::shared_ptr<const core::graphics::IFrameBuffer> src,
                                            std::shared_ptr<core::graphics::IFrameBuffer> dst,
                                            const glm::uvec4 &srcViewport,
                                            const glm::uvec4 &dstViewport,
                                            bool colorMsk, bool depthMask, bool stencilMask,
                                            bool linearFilter)
{
    auto srcFramebuffer = std::dynamic_pointer_cast<const FrameBufferBase_4_5>(src);
    if (!srcFramebuffer)
        LOG_CRITICAL << "Source framebuffer can't be nullptr";

    auto dstFramebuffer = std::dynamic_pointer_cast<FrameBufferBase_4_5>(dst);
    if (!dstFramebuffer)
        LOG_CRITICAL << "Destination framebuffer can't be nullptr";

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

std::shared_ptr<core::graphics::IBuffer> QtOpenGL_4_5_Renderer::createBuffer(size_t size, const void *data) const
{
    return std::make_shared<Buffer_4_5>(size, data);
}

std::shared_ptr<core::graphics::IBufferRange> QtOpenGL_4_5_Renderer::createBufferRange(const std::shared_ptr<core::graphics::IBuffer> &buffer,
                                                                                       size_t offset,
                                                                                       size_t size) const
{
    return std::make_shared<BufferRange_4_5>(buffer, offset, size);
}

std::shared_ptr<core::graphics::IVertexArray> QtOpenGL_4_5_Renderer::createVertexArray(const std::shared_ptr<utils::Mesh> &mesh,
                                                                                       bool uniteVertexBuffers) const
{
    auto vertexArray = std::make_shared<VertexArray_4_5>();

    if (mesh)
    {
        uint32_t numVertices = mesh->vertexBuffers().empty() ? 0u : mesh->vertexBuffers().begin()->second->numVertices();

        if (uniteVertexBuffers)
        {
            size_t totalSize = 0u;
            uint32_t stride = 0u;
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                if (numVertices != buffer->numVertices())
                    LOG_CRITICAL << "Buffers have different size";

                totalSize += buffer->sizeInBytes();
                stride += buffer->numComponents() * buffer->componentSize();
            }

            auto buffer = createBuffer(totalSize);
            auto bindingIndex = vertexArray->attachVertexBuffer(buffer, 0u, stride);

            uint32_t relativeOffset = 0u;
            auto bufferData = buffer->map(core::graphics::IBuffer::MapAccess::WriteOnly);
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                uint32_t vertexSize = buffer->numComponents() * buffer->componentSize();
                for (uint32_t i = 0; i < buffer->numVertices(); ++i)
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

                auto bindingIndex = vertexArray->attachVertexBuffer(createBuffer(buffer->sizeInBytes(), buffer->data()),
                                                                    0u,
                                                                    buffer->numComponents() * buffer->componentSize());
                vertexArray->declareVertexAttribute(attrib, bindingIndex, buffer->numComponents(), buffer->componentType(), 0u);
            }
        }

        uint64_t indexBufferTotalSize = 0u;

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
                                                                                       drawElementsBuffer->type(),
                                                                                       indexBufferTotalSize,
                                                                                       drawElementsBuffer->baseVertex()));
                    indexBufferTotalSize += drawElementsBuffer->sizeInBytes();
                }
        }

        if (indexBufferTotalSize)
        {
            auto buffer = createBuffer(indexBufferTotalSize);
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture1DEmpty(uint32_t width,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture1D(const std::shared_ptr<utils::Image> &image,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    if (!image)
        LOG_CRITICAL << "Image can't be nullptr";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createTexture1DEmpty(image->width(), internalFormat, numLevels);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), 0u, 0u), image->numComponents(), image->type(), image->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture2DEmpty(uint32_t width,
                                                                                      uint32_t height,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture2D(const std::shared_ptr<utils::Image> &image,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    if (!image)
        LOG_CRITICAL << "Image can't be nullptr";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createTexture2DEmpty(image->width(), image->height(), internalFormat, numLevels);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), image->height(), 0u), image->numComponents(), image->type(), image->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture3DEmpty(uint32_t width,
                                                                                      uint32_t height,
                                                                                      uint32_t depth,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture3D(const std::vector<std::shared_ptr<utils::Image>> &images,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
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

    auto result = createTexture3DEmpty(width, height, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0; layer < static_cast<uint32_t>(images.size()); ++layer)
        result->setSubImage(0u, glm::uvec3(0u, 0u, layer), glm::uvec3(width, height, 1u), numComponents, type, images[layer]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTextureCubeEmpty(uint32_t width,
                                                                                        uint32_t height,
                                                                                        core::graphics::PixelInternalFormat internalFormat,
                                                                                        uint32_t numLevels) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTextureCube(const std::vector<std::shared_ptr<utils::Image>> &images,
                                                                                   core::graphics::PixelInternalFormat internalFormat,
                                                                                   uint32_t numLevels,
                                                                                   bool genMipmaps) const
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

    auto result = createTextureCubeEmpty(width, height, internalFormat, numLevels);
    for (uint32_t face = 0; face < static_cast<uint32_t>(images.size()); ++face)
        result->setSubImage(0u, glm::uvec3(0u, 0u, face), glm::uvec3(width, height, 1u), numComponents, type, images[face]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture1DArrayEmpty(uint32_t width,
                                                                                           uint32_t numLayers,
                                                                                           core::graphics::PixelInternalFormat internalFormat,
                                                                                           uint32_t numLevels) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture1DArray(const std::vector<std::shared_ptr<utils::Image>> &images,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels,
                                                                                      bool genMipmaps) const
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

    auto result = createTexture1DArrayEmpty(width, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0u; layer < static_cast<uint32_t>(images.size()); ++layer)
        result->setSubImage(0u, glm::uvec3(0u, layer, 0u), glm::uvec3(width, 1u, 0u), numComponents, type, images[layer]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture2DArrayEmpty(uint32_t width,
                                                                                           uint32_t height,
                                                                                           uint32_t numLayers,
                                                                                           core::graphics::PixelInternalFormat internalFormat,
                                                                                           uint32_t numLevels) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture2DArray(const std::vector<std::shared_ptr<utils::Image>> &images,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels,
                                                                                      bool genMipmaps) const
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

    auto result = createTexture2DArrayEmpty(width, height, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0u; layer < static_cast<uint32_t>(images.size()); ++layer)
        result->setSubImage(0u, glm::uvec3(0u, 0u, layer), glm::uvec3(width, height, 1u), numComponents, type, images[layer]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTextureCubeArrayEmpty(uint32_t width,
                                                                                             uint32_t height,
                                                                                             uint32_t numLayers,
                                                                                             core::graphics::PixelInternalFormat internalFormat,
                                                                                             uint32_t numLevels) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTextureCubeArray(const std::vector<std::vector<std::shared_ptr<utils::Image>>> &images,
                                                                                        core::graphics::PixelInternalFormat internalFormat,
                                                                                        uint32_t numLevels,
                                                                                        bool genMipmaps) const
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

    auto result = createTextureCubeArrayEmpty(width, height, static_cast<uint32_t>(images.size()), internalFormat, numLevels);
    for (uint32_t layer = 0u; layer < static_cast<uint32_t>(images.size()); ++layer)
        for (uint32_t face = 0; face < static_cast<uint32_t>(images.size()); ++face)
            result->setSubImage(0u, glm::uvec3(0u, 0u, 6u * layer + face), glm::uvec3(width, height, 1u), numComponents, type, images[layer][face]->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTextureRectEmpty(uint32_t width,
                                                                                        uint32_t height,
                                                                                        core::graphics::PixelInternalFormat internalFormat) const
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTextureRect(const std::shared_ptr<utils::Image> &image,
                                                                                   core::graphics::PixelInternalFormat internalFormat) const
{
    if (!image)
        LOG_CRITICAL << "Image can't be nullptr";

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = core::graphics::pixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createTextureRectEmpty(image->width(), image->height(), internalFormat);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), image->height(), 0u), image->numComponents(), image->type(), image->data());

    return result;
}

std::shared_ptr<core::graphics::IImage> QtOpenGL_4_5_Renderer::createImage(const std::shared_ptr<core::graphics::ITexture> &texture,
                                                                           core::graphics::IImage::DataAccess access,
                                                                           uint32_t level) const
{
    if (!texture)
        LOG_CRITICAL << "Texture can't be nullptr";

    if (level >= texture->numMipmapLevels())
        LOG_CRITICAL << "Level must be less than texture levels count";

    return std::make_shared<Image_4_5>(texture, access, level);
}

std::shared_ptr<core::graphics::IRenderBuffer> QtOpenGL_4_5_Renderer::createRenderBuffer(uint32_t width,
                                                                                         uint32_t height,
                                                                                         core::graphics::PixelInternalFormat internalFormat) const
{
    return std::make_shared<RenderBuffer_4_5>(width, height, internalFormat);
}

std::shared_ptr<core::graphics::IFrameBuffer> QtOpenGL_4_5_Renderer::createFrameBuffer() const
{
    return std::make_shared<FrameBuffer_4_5>();
}

std::shared_ptr<core::graphics::IRenderProgram> QtOpenGL_4_5_Renderer::createRenderProgram(const std::shared_ptr<utils::Shader> &vertexShader,
                                                                                           const std::shared_ptr<utils::Shader> &fragmentShader) const
{
    if (!vertexShader)
        LOG_CRITICAL << "Vertex shader can't be nullptr";

    if (!fragmentShader)
        LOG_CRITICAL << "Fragment shader can't be nullptr";

    auto renderProgram = std::make_shared<RenderProgram_4_5>();
    return createProgram(renderProgram,
                         {std::make_pair(GL_VERTEX_SHADER, std::cref(vertexShader->data())),
                          std::make_pair(GL_FRAGMENT_SHADER, std::cref(fragmentShader->data()))}) ?
                renderProgram :
                nullptr;
}

std::shared_ptr<core::graphics::IComputeProgram> QtOpenGL_4_5_Renderer::createComputeProgram(const std::shared_ptr<utils::Shader> &computeShader) const
{
    if (!computeShader)
        LOG_CRITICAL << "Compute shader can't be nullptr";

    auto computerProgram = std::make_shared<ComputeProgram_4_5>();
    return createProgram(computerProgram,
                         {std::make_pair(GL_COMPUTE_SHADER, std::cref(computeShader->data()))}) ?
                computerProgram :
                nullptr;
}

void QtOpenGL_4_5_Renderer::resize(uint32_t width, uint32_t height)
{
    m_viewportSize.x = width;
    m_viewportSize.y = height;
}

const glm::uvec2 &QtOpenGL_4_5_Renderer::viewportSize() const
{
    return m_viewportSize;
}

void QtOpenGL_4_5_Renderer::clearRenderData()
{
    m_renderData.clear();
}

void QtOpenGL_4_5_Renderer::addRenderData(const std::shared_ptr<core::graphics::IRenderProgram> &renderProgram,
                                          const std::shared_ptr<const core::Drawable> &drawable,
                                          const glm::mat4x4 &transform)
{
    auto renderProgram_4_5 = std::dynamic_pointer_cast<RenderProgram_4_5>(renderProgram);
    if (!renderProgram_4_5)
        LOG_CRITICAL << "Render program can't be nullptr";

    if (!drawable)
        LOG_CRITICAL << "Drawable can't be nullptr";

    m_renderData.push_back({transform, renderProgram_4_5, drawable});
}

void QtOpenGL_4_5_Renderer::render(const std::shared_ptr<core::graphics::IFrameBuffer> &frameBuffer,
                                   const core::RenderInfo &renderInfo,
                                   const glm::uvec4 &viewport)
{
    if (frameBuffer->faceCulling())
    {
        glCullFace(Conversions::FaceType2GL(frameBuffer->cullFaceType()));
        glEnable(GL_CULL_FACE);
    }
    else
        glDisable(GL_CULL_FACE);

    for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    {
        GLboolean mask = frameBuffer->colorMask(i);
        glColorMaski(i, mask, mask, mask, mask);
    }

    if (frameBuffer->depthTest())
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(Conversions::ComparingFunc2GL(frameBuffer->depthFunc()));
        glDepthMask(frameBuffer->depthMask());
    }
    else
        glDisable(GL_DEPTH_TEST);

    if (frameBuffer->stencilTest())
    {
        glEnable(GL_STENCIL_TEST);
        for (const auto &face : {core::graphics::FaceType::Front, core::graphics::FaceType::Back})
        {
            const auto &operations = frameBuffer->stencilOperations(face);

            glStencilOpSeparate(Conversions::FaceType2GL(face),
                                Conversions::StencilOperation2GL(operations[0]),
                                Conversions::StencilOperation2GL(operations[1]),
                                Conversions::StencilOperation2GL(operations[2]));

            glStencilFuncSeparate(Conversions::FaceType2GL(face),
                                  Conversions::ComparingFunc2GL(frameBuffer->stencilComparingFunc(face)),
                                  frameBuffer->stencilReferenceValue(face),
                                  frameBuffer->stencilMaskValue(face));
        }
    }
    else
        glDisable(GL_STENCIL_TEST);

    if (frameBuffer->blending())
    {
        glEnable(GL_BLEND);

        auto blendConstantColor = frameBuffer->blendConstantColor();
        glBlendColor(blendConstantColor.r, blendConstantColor.g, blendConstantColor.b, frameBuffer->blendConstantAlpha());

        for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
        {
            glBlendEquationSeparatei(i,
                                     Conversions::BlendEquetion2GL(frameBuffer->blendColorEquation(i)),
                                     Conversions::BlendEquetion2GL(frameBuffer->blendAlphaEquation(i)));
            glBlendFuncSeparatei(i,
                                 Conversions::BlendFactor2GL(frameBuffer->blendColorSourceFactor(i)),
                                 Conversions::BlendFactor2GL(frameBuffer->blendColorDestinationFactor(i)),
                                 Conversions::BlendFactor2GL(frameBuffer->blendAlphaSourceFactor(i)),
                                 Conversions::BlendFactor2GL(frameBuffer->blendAlphaDestinationFactor(i)));
        }
    }
    else
        glDisable(GL_BLEND);

    auto frameBuffer_4_5 = std::dynamic_pointer_cast<FrameBufferBase_4_5>(frameBuffer);
    if (!frameBuffer_4_5)
        LOG_CRITICAL << "Framebuffer can't be nullptr";

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_4_5->id());
    frameBuffer_4_5->clear();

    glViewport(static_cast<GLint>(viewport.x),
               static_cast<GLint>(viewport.y),
               static_cast<GLsizei>(viewport.z),
               static_cast<GLsizei>(viewport.w));

    for (auto &renderData : m_renderData)
    {
        auto &drawable = std::get<2>(renderData);

        auto vao_4_5 = std::dynamic_pointer_cast<const VertexArray_4_5>(drawable->vertexArray());
        if (!vao_4_5)
            LOG_CRITICAL << "VAO can't be nullptr";
        glBindVertexArray(vao_4_5->id());

        auto renderProgram_4_5 = std::get<1>(renderData);
        if (!renderProgram_4_5)
            LOG_CRITICAL << "Render program can't be nullptr";
        glUseProgram(renderProgram_4_5->id());
        setupVertexAttributes(renderProgram_4_5, vao_4_5);
        setupUniforms(renderProgram_4_5, drawable, renderInfo, glm::uvec2(viewport.z, viewport.w), std::get<0>(renderData));
        setupSSBOs(renderProgram_4_5, drawable, renderInfo);

        for (auto &primitiveSet : vao_4_5->primitiveSets())
        {
            if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
                glDrawArrays(Conversions::PrimitiveType2GL(drawArrays->primitiveType()),
                             static_cast<GLint>(drawArrays->first()),
                             static_cast<GLint>(drawArrays->count()));
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
                glDrawElementsBaseVertex(Conversions::PrimitiveType2GL(drawElements->primitiveType()),
                                         static_cast<GLsizei>(drawElements->count()),
                                         Conversions::DrawElementsIndexType2GL(drawElements->type()),
                                         reinterpret_cast<const void*>(drawElements->offset()),
                                         static_cast<GLint>(drawElements->baseVertex()));
        }
    }

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    // it is required after all rendering for right work of qt
    glDepthMask(GL_TRUE);
}

void QtOpenGL_4_5_Renderer::compute(const std::shared_ptr<core::graphics::IComputeProgram> &computeProgram,
                                    const core::RenderInfo &renderInfo,
                                    const glm::uvec3 &numInvocations)
{
    auto computeProgram_4_5 = std::dynamic_pointer_cast<ComputeProgram_4_5>(computeProgram);
    if (!computeProgram_4_5)
        LOG_CRITICAL << "Compute program can't be nullptr";
    glUseProgram(computeProgram_4_5->id());
    setupUniforms(computeProgram_4_5, nullptr, renderInfo, glm::uvec2(0u), glm::mat4x4(1.0f));
    setupSSBOs(computeProgram_4_5, nullptr, renderInfo);

    auto numWorkGroups = glm::uvec3(glm::ceil(glm::vec3(numInvocations) / glm::vec3(computeProgram->workGroupSize())) + glm::vec3(.5f));
    glDispatchCompute(numWorkGroups.x, numWorkGroups.y, numWorkGroups.z);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

int32_t QtOpenGL_4_5_Renderer::bindTexture(const core::graphics::PConstTexture &texture)
{
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(texture);
    if (!textureBase)
        LOG_CRITICAL << "Texture can't be nullptr";

    glBindTextureUnit(static_cast<GLuint>(m_textureUnit), textureBase->id());
    return m_textureUnit++;
}

int32_t QtOpenGL_4_5_Renderer::bindImage(const core::graphics::PConstImage &image)
{
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(image->texture());
    if (!textureBase)
        LOG_CRITICAL << "Image's texture can't be nullptr";

    glBindImageTexture(static_cast<GLuint>(m_imageUnit),
                       textureBase->id(),
                       static_cast<GLint>(image->mipmapLevel()),
                       GL_TRUE,
                       0u,
                       Conversions::ImageDataAccess2GL(image->access()),
                       textureBase->GLinternalFormat());

    return m_imageUnit++;
}

void QtOpenGL_4_5_Renderer::bindBuffer(GLenum target, GLuint bindingPoint, const core::graphics::PConstBufferRange &bufferRange)
{
    auto buffer = std::dynamic_pointer_cast<const Buffer_4_5>(bufferRange->buffer());
    if (!buffer)
        LOG_CRITICAL << "Buffer can't be nullptr";

    auto size = bufferRange->size();
    if (size == static_cast<size_t>(-1))
        size = buffer->size() - bufferRange->offset();

    glBindBufferRange(target,
                      bindingPoint,
                      buffer->id(),
                      static_cast<GLintptr>(bufferRange->offset()),
                      static_cast<GLsizeiptr>(size));
}

uint32_t QtOpenGL_4_5_Renderer::bindSSBO(const core::graphics::PConstBufferRange &bufferRange)
{
    bindBuffer(GL_SHADER_STORAGE_BUFFER, m_bufferUnit, bufferRange);
    return m_bufferUnit++;
}

void QtOpenGL_4_5_Renderer::bindAtomicCounterBuffer(GLuint bindingPoint, const core::graphics::PConstBufferRange &bufferRange)
{
    bindBuffer(GL_ATOMIC_COUNTER_BUFFER, bindingPoint, bufferRange);
}

std::shared_ptr<QtOpenGL_4_5_Renderer> QtOpenGL_4_5_Renderer::create(QOpenGLContext *context, GLuint defaultFbo)
{
    if (auto renderer = instance(context); renderer)
    {
        LOG_WARNING << "QtOpenGL_4_5_Renderer instance has already been created";
        return renderer;
    }

    auto renderer = std::shared_ptr<QtOpenGL_4_5_Renderer>(new QtOpenGL_4_5_Renderer(context, defaultFbo));

    s_instances[context] = renderer;
    return renderer;
}

std::shared_ptr<QtOpenGL_4_5_Renderer> QtOpenGL_4_5_Renderer::instance(QOpenGLContext *context)
{
    if (!context)
        context = QOpenGLContext::currentContext();

    if (!context)
        return nullptr;

    auto it = s_instances.find(context);
    if (it == s_instances.end())
        return nullptr;

    return it->second.lock();
}

QtOpenGL_4_5_Renderer::QtOpenGL_4_5_Renderer(QOpenGLContext *context, GLuint defaultFbo)
    : m_viewportSize(0u, 0u)
{
    setOwningContext(context);
    if (!initializeOpenGLFunctions())
        LOG_CRITICAL << "Can't initialize QOpenGLFunctions";

    makeDefaultFrameBuffer(defaultFbo);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    LOG_INFO << "GraphicsRenderer \"" << QtOpenGL_4_5_Renderer::name() << "\" has been created";
}

void QtOpenGL_4_5_Renderer::makeDefaultFrameBuffer(GLuint defaultFbo)
{
    m_defaultFrameBuffer = std::make_shared<DefaultFrameBuffer_4_5>(defaultFbo);
    auto &attachments = const_cast<core::graphics::IFrameBuffer::Attachments&>(m_defaultFrameBuffer->attachments());

    GLint objType = GL_NONE;
    glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    if (objType != GL_NONE)
        attachments[core::graphics::FrameBufferAttachment::DepthStencil] = {nullptr, 0, 0};
    else
    {
        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments[core::graphics::FrameBufferAttachment::Depth] = {nullptr, 0, 0};

        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments[core::graphics::FrameBufferAttachment::Stencil] = {nullptr, 0, 0};
    }

    std::vector<GLenum> drawBuffers;
    for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    {
        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
        {
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            attachments[core::graphics::FrameBufferColorAttachment(i)] = {nullptr, 0, 0};
        }
    }
    glNamedFramebufferDrawBuffers(defaultFbo, static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
}

void QtOpenGL_4_5_Renderer::setupVertexAttributes(const std::shared_ptr<RenderProgram_4_5> &renderProgram,
                                                  const std::shared_ptr<const VertexArray_4_5> &vao)
{
    for (const auto &attributeInfo : renderProgram->attributesInfo())
    {
        if (attributeInfo.id == utils::VertexAttribute::Count)
            continue;

        if (attributeInfo.componentType != vao->vertexAttributeComponentType(attributeInfo.id))
            LOG_CRITICAL << "Vertex attribute \"" << renderProgram->attributeNameByIndex(attributeInfo.index) << "\" has wrong component type";

        if (attributeInfo.numComponents != vao->vertexAttributeNumComponents(attributeInfo.id))
            LOG_CRITICAL << "Vertex attribute \"" << renderProgram->attributeNameByIndex(attributeInfo.index) << "\" has wrong number of components";
    }
}

void QtOpenGL_4_5_Renderer::setupUniform(GLuint rpId,
                                         GLint loc,
                                         const core::PConstAbstractUniform &uniform)
{
    if (!uniform)
        LOG_CRITICAL << "Uniform can't be nullptr";

    switch (uniform->type())
    {
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
        glProgramUniform1i(rpId, loc, bindTexture(textureUniform));
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
        glProgramUniform1i(rpId, loc, bindImage(imageUniform));
        break;
    }
    case core::graphics::UniformType::AtomicCounterUint:
    {
        auto atomicCounterUniform = core::uniform_cast<core::graphics::PConstBufferRange>(uniform)->data();
        bindAtomicCounterBuffer(static_cast<GLuint>(loc), atomicCounterUniform);
        break;
    }
    default:
    {
        break;
    }
    }
}

void QtOpenGL_4_5_Renderer::setupUniforms(const std::shared_ptr<ProgramBase_4_5> &program,
                                          const std::shared_ptr<const core::Drawable> &drawable,
                                          const core::RenderInfo &renderInfo,
                                          const glm::vec2 &viewportSize,
                                          const glm::mat4 &modelMatrix)
{
    auto programId = program->id();
    m_textureUnit = 0;
    m_imageUnit = 0;

    for (const auto &uniformInfo : program->uniformsInfo())
    {
        core::PConstAbstractUniform uniform;

        switch (uniformInfo.id)
        {
        case core::graphics::UniformId::ViewportSize:
            uniform = core::makeUniform(viewportSize);
            break;
        case core::graphics::UniformId::ModelMatrix:
            uniform = core::makeUniform(modelMatrix);
            break;
        case core::graphics::UniformId::NormalMatrix:
            uniform = core::makeUniform(glm::mat3x3(glm::inverseTranspose(modelMatrix)));
            break;
        case core::graphics::UniformId::ViewMatrix:
            uniform = renderInfo.viewMatrixUniform();
            break;
        case core::graphics::UniformId::ViewMatrixInverse:
            uniform = renderInfo.viewMatrixInverseUniform();
            break;
        case core::graphics::UniformId::ProjectionMatrix:
            uniform = renderInfo.projectionMatrixUniform();
            break;
        case core::graphics::UniformId::ProjectionMatrixInverse:
            uniform = renderInfo.projectionMatrixInverseUniform();
            break;
        case core::graphics::UniformId::ViewProjectionMatrix:
            uniform = renderInfo.viewProjectionMatrixUniform();
            break;
        case core::graphics::UniformId::ViewProjectionMatrixInverse:
            uniform = renderInfo.viewProjectionMatrixInverseUniform();
            break;
        case core::graphics::UniformId::ModelViewMatrix:
            uniform = core::makeUniform(renderInfo.viewMatrixUniform()->data() * modelMatrix);
            break;
        case core::graphics::UniformId::NormalViewMatrix:
            uniform = core::makeUniform(glm::mat3x3(glm::inverseTranspose(renderInfo.viewMatrixUniform()->data() * modelMatrix)));
            break;
        case core::graphics::UniformId::ModelViewProjectionMatrix:
            uniform = core::makeUniform(renderInfo.viewProjectionMatrixUniform()->data() * modelMatrix);
            break;
        case core::graphics::UniformId::ModelPosition:
            uniform = core::makeUniform(glm::vec3(modelMatrix * glm::vec4(0.f, 0.f, 0.f, 1.f)));
            break;
        case core::graphics::UniformId::ModelXDirection:
            uniform = core::makeUniform(glm::normalize(glm::vec3(modelMatrix * glm::vec4(1.f, 0.f, 0.f, 0.f))));
            break;
        case core::graphics::UniformId::ModelYDirection:
            uniform = core::makeUniform(glm::normalize(glm::vec3(modelMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f))));
            break;
        case core::graphics::UniformId::ModelZDirection:
            uniform = core::makeUniform(glm::normalize(glm::vec3(modelMatrix * glm::vec4(0.f, 0.f, 1.f, 0.f))));
            break;
        case core::graphics::UniformId::ViewPosition:
            uniform = renderInfo.viewPositionUniform();
            break;
        case core::graphics::UniformId::ViewXDirection:
            uniform = renderInfo.viewXDirectionUniform();
            break;
        case core::graphics::UniformId::ViewYDirection:
            uniform = renderInfo.viewYDirectionUniform();
            break;
        case core::graphics::UniformId::ViewZDirection:
            uniform = renderInfo.viewZDirectionUniform();
            break;
        case core::graphics::UniformId::GBufferColor0Map:
            uniform = renderInfo.GBufferColor0TextureUniform();
            break;
        case core::graphics::UniformId::GBufferColor1Map:
            uniform = renderInfo.GBufferColor1TextureUniform();
            break;
        case core::graphics::UniformId::GBufferColor2Map:
            uniform = renderInfo.GBufferColor2TextureUniform();
            break;
        case core::graphics::UniformId::GBufferDepthMap:
            uniform = renderInfo.GBufferDepthTextureUniform();
            break;
        case core::graphics::UniformId::OITDepthImage:
            uniform = renderInfo.OITDepthImageUniform();
            break;
        case core::graphics::UniformId::OITIndicesImage:
            uniform = renderInfo.OITIndicesImageUniform();
            break;
        case core::graphics::UniformId::OITNodesCounter:
            uniform = renderInfo.OITNodesCounterUniform();
            break;
        case core::graphics::UniformId::LightBufferColorMap:
            uniform = renderInfo.lightBufferColorTextureUniform();
            break;
        case core::graphics::UniformId::FinalBufferColorMap:
            uniform = renderInfo.finalBufferColorTextureUniform();
            break;
        case core::graphics::UniformId::Undefined:
            uniform = drawable->userUniform(program->uniformNameByIndex(uniformInfo.index));
            break;
        default:
            uniform = drawable->uniform(uniformInfo.id);
            break;
        }

        if (!uniform)
            LOG_CRITICAL << "Uniform \"" << program->uniformNameByIndex(uniformInfo.index) << "\" has not set";

        if (uniformInfo.type != uniform->type())
            LOG_CRITICAL << "Uniform \"" << program->uniformNameByIndex(uniformInfo.index) << "\" has wrong type";

        setupUniform(programId, uniformInfo.location, uniform);
    }
}

void QtOpenGL_4_5_Renderer::setupSSBOs(const std::shared_ptr<ProgramBase_4_5> &program,
                                       const std::shared_ptr<const core::Drawable> &drawable,
                                       const core::RenderInfo &renderInfo)
{
    auto programId = program->id();
    m_bufferUnit = 0;

    for (const auto &ssboInfo : program->SSBOsInfo())
    {
        core::graphics::PConstBufferRange bufferRange;

        switch (ssboInfo.id)
        {
        case core::graphics::SSBOId::OITNodes:
            bufferRange = renderInfo.OITNodesBuffer();
            break;
        case core::graphics::SSBOId::Undefined:
//            if (drawable)
//                bufferTange = drawable->userSSBO(program->SSBONameByIndex(ssboInfo.index));
            break;
        default:
            if (drawable)
                bufferRange = drawable->SSBO(ssboInfo.id);
            break;
        }

        if (!bufferRange)
            LOG_CRITICAL << "SSBO \"" << program->SSBONameByIndex(ssboInfo.index) << "\" has not set";

        glShaderStorageBlockBinding(programId, ssboInfo.index, bindSSBO(bufferRange));
    }
}

bool QtOpenGL_4_5_Renderer::createProgram(std::shared_ptr<ProgramBase_4_5> program,
        const std::vector<std::pair<GLenum, std::reference_wrapper<const std::string>>> &shadersData) const
{
    static const std::unordered_map<GLenum, std::string> s_shaderTypesTable {
        {GL_COMPUTE_SHADER, "Compute"},
        {GL_VERTEX_SHADER, "Vertex"},
        {GL_TESS_CONTROL_SHADER, "TessControl"},
        {GL_TESS_EVALUATION_SHADER, "TessEvaluation"},
        {GL_GEOMETRY_SHADER, "Geometry"},
        {GL_FRAGMENT_SHADER, "Fragment"}
    };

    if (!program)
        LOG_CRITICAL << "Program can't be nullptr";

    auto renderer = QtOpenGL_4_5_Renderer::instance();

    std::vector<GLuint> shadersIds(shadersData.size(), 0u);
    bool isOk = true;

    for (size_t i = 0; i < shadersData.size(); ++i)
    {
        const auto &shader = shadersData[i];
        const char *data = shader.second.get().c_str();

        shadersIds[i] = renderer->glCreateShader(shader.first);
        renderer->glShaderSource(shadersIds[i], 1, &data, nullptr);
        renderer->glCompileShader(shadersIds[i]);
        GLint compiled;
        renderer->glGetShaderiv(shadersIds[i], GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            renderer->glGetShaderiv(shadersIds[i], GL_INFO_LOG_LENGTH, &infoLen);
            if(infoLen > 1)
            {
                char *infoLog = static_cast<char*>(malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
                renderer->glGetShaderInfoLog(shadersIds[i], infoLen, nullptr, infoLog);
                LOG_ERROR << s_shaderTypesTable.at(shader.first) << " shader compile error: " << infoLog;
                free(infoLog);
            }
            isOk = false;
        }
    }

    auto programId = program->id();

    if (isOk)
    {
        for (const auto &shaderId : shadersIds)
            renderer->glAttachShader(programId, shaderId);
    }

    if (isOk)
    {
        std::string logString;
        if (!program->preBuild(logString))
        {
            isOk = false;
            LOG_ERROR << "Program pre build: " << logString;
        }
    }

    if (isOk)
    {
        renderer->glLinkProgram(programId);
        GLint linked;
        renderer->glGetProgramiv(programId, GL_LINK_STATUS, &linked);
        if (!linked) {
            GLint infoLen = 0;
            renderer->glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLen);
            if(infoLen > 1) {
                char *infoLog = static_cast<char*>(malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
                renderer->glGetProgramInfoLog(programId, infoLen, nullptr, infoLog);
                LOG_ERROR << "Program link error: " << infoLog;
                free(infoLog);
            }
            isOk = false;
        }
    }

    if (isOk)
    {
        std::string logString;
        if (!program->postBuild(logString))
        {
            isOk = false;
            LOG_ERROR << "Program post build: " << logString;
        }
    }

    if (!isOk)
    {
        for (const auto &shaderId : shadersIds)
            if (renderer->glIsShader(shaderId) == GL_TRUE)
                renderer->glDeleteShader(shaderId);
    }

    return isOk;
}


}
}
