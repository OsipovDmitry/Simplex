﻿#include <array>
#include <functional>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/glm/gtx/texture.hpp>
#include <utils/logger.h>
#include <utils/mesh.h>
#include <utils/image.h>

#include <core/renderinfo.h>
#include <core/idrawable.h>

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
        GL_NONE,
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

core::graphics::PixelInternalFormat Conversions::PixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(
        uint32_t numComponents,
        utils::PixelComponentType type)
{
    core::graphics::PixelInternalFormat result = core::graphics::PixelInternalFormat::Undefined;

    switch (type)
    {
    case utils::PixelComponentType::Uint8:
    {
        switch (numComponents)
        {
        case 1: { result = core::graphics::PixelInternalFormat::R8; break; }
        case 2: { result = core::graphics::PixelInternalFormat::RG8; break; }
        case 3: { result = core::graphics::PixelInternalFormat::RGB8; break; }
        case 4: { result = core::graphics::PixelInternalFormat::RGBA8; break; }
        default: { break; }
        }
        break;
    }
    case utils::PixelComponentType::Uint16:
    {
        switch (numComponents)
        {
        case 1: { result = core::graphics::PixelInternalFormat::R16; break; }
        case 2: { result = core::graphics::PixelInternalFormat::RG16; break; }
        case 3: { result = core::graphics::PixelInternalFormat::RGB16; break; }
        case 4: { result = core::graphics::PixelInternalFormat::RGBA16; break; }
        default: { break; }
        }
        break;
    }
    case utils::PixelComponentType::Single:
    {
        switch (numComponents)
        {
        case 1: { result = core::graphics::PixelInternalFormat::R16F; break; }
        case 2: { result = core::graphics::PixelInternalFormat::RG16F; break; }
        case 3: { result = core::graphics::PixelInternalFormat::RGB16F; break; }
        case 4: { result = core::graphics::PixelInternalFormat::RGBA16F; break; }
        default: { break; }
        }
        break;
    }
    default: { break; }
    }

    return result;
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

GLbitfield Conversions::BufferMapAccess2GL(core::graphics::IBuffer::MapAccess value)
{
    static std::array<GLbitfield, core::graphics::IBuffer::numElementsMapAccess()> s_table {
        GL_MAP_READ_BIT,
        GL_MAP_WRITE_BIT,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
    };

    return s_table[core::graphics::IBuffer::castFromMapAccess(value)];
}

uint16_t Conversions::GL2PixelNumComponents(GLenum value)
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
    return (it == s_table.end()) ? utils::VertexComponentType::Undefined : it->second;
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
        GL_SAMPLER_2D
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
        { GL_SAMPLER_1D_ARRAY, core::graphics::UniformType::Sampler1D },
        { GL_SAMPLER_2D_ARRAY, core::graphics::UniformType::Sampler2D },
        { GL_SAMPLER_CUBE_MAP_ARRAY, core::graphics::UniformType::SamplerCubeArray },
        { GL_SAMPLER_2D_RECT, core::graphics::UniformType::SamplerRect },
//        { GL_SAMPLER_1D_SHADOW, core::graphics::UniformType::Sampler },
//        { GL_SAMPLER_2D_SHADOW, core::graphics::UniformType::Sampler },
//        { GL_SAMPLER_CUBE_SHADOW, core::graphics::UniformType::Sampler },
//        { GL_SAMPLER_1D_ARRAY_SHADOW, core::graphics::UniformType::Sampler },
//        { GL_SAMPLER_2D_ARRAY_SHADOW, core::graphics::UniformType::Sampler },
//        { GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, core::graphics::UniformType::Sampler },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? core::graphics::UniformType::Undefined : it->second;
}

GLenum Conversions::faceType2GL(core::graphics::FaceType value)
{
    static std::array<GLenum, core::graphics::numElementsFaceType()> s_table {
        GL_FRONT,
        GL_BACK,
        GL_FRONT_AND_BACK
    };
    return s_table[core::graphics::castFromFaceType(value)];
}

GLenum Conversions::depthFunc2GL(core::graphics::DepthFunc value)
{
    static std::array<GLenum, core::graphics::numElementsDepthFunc()> s_table {
        GL_NEVER,
        GL_LESS,
        GL_EQUAL,
        GL_LEQUAL,
        GL_GREATER,
        GL_NOTEQUAL,
        GL_GEQUAL,
        GL_ALWAYS
    };
    return s_table[core::graphics::castFromDepthFunc(value)];
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
                                                                      Conversions::BufferMapAccess2GL(access))
                                      )
                );
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
    assert(buffer_4_5);

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
    assert(numComponents <= 4);
    assert(type != utils::VertexComponentType::Undefined);

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
    return (it != m_vertexDeclarations.end()) ? std::get<1>(it->second) : 0;
}

utils::VertexComponentType VertexArray_4_5::vertexAttributeComponentType(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<2>(it->second) : utils::VertexComponentType::Undefined;
}

uint32_t VertexArray_4_5::vertexAttributeRelativeOffset(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<3>(it->second) : 0;
}

void VertexArray_4_5::attachIndexBuffer(std::shared_ptr<core::graphics::IBuffer> buffer)
{
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    assert(buffer_4_5);

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
    assert(primitiveSet);
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

glm::uvec2 TextureBase_4_5::size() const
{
    return mipmapSize(0u);
}

core::graphics::PixelInternalFormat TextureBase_4_5::internalFormat() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_INTERNAL_FORMAT, &result);

    return Conversions::GL2PixelInternalFormat(static_cast<GLenum>(result));
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

void TextureBase_4_5::setFiltering(core::graphics::TextureFiltering value)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    switch (value)
    {
    case core::graphics::TextureFiltering::Point: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    }
    case core::graphics::TextureFiltering::Linear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    }
    case core::graphics::TextureFiltering::Bilinear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    }
    case core::graphics::TextureFiltering::Trilinear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
    default: break;
    }
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
    assert((numComponents >= 1) && (numComponents <= 4));
    assert(type != utils::PixelComponentType::Undefined);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
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
    assert(level == 0);
    assert((numComponents >= 1) && (numComponents <= 4));
    assert(type != utils::PixelComponentType::Undefined);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::PixelNumComponents2GL(numComponents),
                                  Conversions::PixelComponentType2GL(type),
                                  data);
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
        m_clearColor[i] = { core::graphics::FrameBufferClearColorType::Single,
                            core::graphics::FrameBufferClearColorValue{glm::vec4(.5f, .5f, 1.f, 1.f)} };
    m_clearDepth = 1.f;
    m_clearStencil = 0;
}

FrameBufferBase_4_5::~FrameBufferBase_4_5()
{
}

GLuint FrameBufferBase_4_5::id() const
{
    return m_id;
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
    assert(index < core::graphics::FrameBufferColorAttachmentsCount());
    return m_clearColor[index];
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::vec4 &value)
{
    assert(index < core::graphics::FrameBufferColorAttachmentsCount());
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Single, core::graphics::FrameBufferClearColorValue{value} };
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::i32vec4 &value)
{
    assert(index < core::graphics::FrameBufferColorAttachmentsCount());
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Int32, core::graphics::FrameBufferClearColorValue{value} };
}

void FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::u32vec4 &value)
{
    assert(index < core::graphics::FrameBufferColorAttachmentsCount());
    m_clearColor[index] = { core::graphics::FrameBufferClearColorType::Uint32, core::graphics::FrameBufferClearColorValue{value} };
}

float FrameBufferBase_4_5::clearDepth() const
{
    return m_clearDepth;
}

int32_t FrameBufferBase_4_5::clearStencil() const
{
    return m_clearStencil;
}

void FrameBufferBase_4_5::setClearDepthStencil(float depth, int32_t stencil)
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
        if (core::graphics::IsFrameBufferColorAttachment(attachment))
            buffers.push_back(GL_COLOR_ATTACHMENT0 + core::graphics::FrameBufferColorAttachmentIndex(attachment));

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glNamedFramebufferDrawBuffers(m_id,
                                            static_cast<GLsizei>(buffers.size()),
                                            buffers.data());
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
                    renderer->glClearBufferfi(GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);//renderer->glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, m_clearDepth, m_clearStencil);
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
                    renderer->glClearBufferfi(GL_DEPTH_STENCIL, 0, m_clearDepth, m_clearStencil);//renderer->glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, m_clearDepth, m_clearStencil);
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
                             std::shared_ptr<core::graphics::ISurface> surface,
                             uint32_t level,
                             uint32_t layer)
{
    detach(key);

    if (auto texture = std::dynamic_pointer_cast<TextureBase_4_5>(surface); texture)
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
    else if (auto renderBuffer = std::dynamic_pointer_cast<RenderBuffer_4_5>(surface); renderBuffer)
    {
        auto renderer = QtOpenGL_4_5_Renderer::instance();
        renderer->glNamedFramebufferRenderbuffer(m_id,
                                                 Conversions::FrameBufferAttachment2GL(key),
                                                 GL_RENDERBUFFER,
                                                 renderBuffer->id());
        m_attchments[key] = { surface, level, layer };
    }
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

void DefaultFrameBuffer_4_5::attach(core::graphics::FrameBufferAttachment, std::shared_ptr<core::graphics::ISurface>, uint32_t, uint32_t)
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

void DefaultFrameBuffer_4_5::setClearDepthStencil(float, int32_t)
{
}

void DefaultFrameBuffer_4_5::setClearMask(const std::unordered_set<core::graphics::FrameBufferAttachment>&)
{
}

// RenderProgram_4_5

RenderProgram_4_5::RenderProgram_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    m_id = renderer->glCreateProgram();
}

RenderProgram_4_5::~RenderProgram_4_5()
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

GLuint RenderProgram_4_5::id() const
{
    return m_id;
}

bool RenderProgram_4_5::compile(std::string &log)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLint numAttributes;
    renderer->glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    m_attributesInfo.reserve(static_cast<size_t>(numAttributes));

    renderer->glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &m_attributeNameMaxLength);
    std::vector<GLchar> attributeName(static_cast<size_t>(m_attributeNameMaxLength));

    GLsizei length;
    GLint size;
    GLenum type;
    for (GLint i = 0; i < numAttributes; ++i)
    {
        renderer->glGetActiveAttrib(m_id,
                                    static_cast<GLuint>(i),
                                    static_cast<GLsizei>(m_attributeNameMaxLength),
                                    &length,
                                    &size,
                                    &type,
                                    attributeName.data());
        m_attributesInfo.push_back({ vertexAttributeByName(attributeName.data()),
                                     static_cast<uint16_t>(i),
                                     static_cast<int32_t>(renderer->glGetAttribLocation(m_id, attributeName.data())),
                                     Conversions::GL2PixelNumComponents(type),
                                     Conversions::GL2VertexComponentType(type)});
    }

    GLint numUniforms;
    renderer->glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &numUniforms);
    m_uniformsInfo.reserve(static_cast<size_t>(numUniforms));

    renderer->glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &m_uniformNameMaxLength);
    std::vector<GLchar> uniformName(static_cast<size_t>(m_uniformNameMaxLength));

    for (GLint i = 0; i < numUniforms; ++i)
    {
        renderer->glGetActiveUniform(m_id,
                                     static_cast<GLuint>(i),
                                     static_cast<GLsizei>(m_uniformNameMaxLength),
                                     &length,
                                     &size,
                                     &type,
                                     uniformName.data());
        m_uniformsInfo.push_back({ UniformIdByName(uniformName.data()),
                                   static_cast<uint16_t>(i),
                                   static_cast<int32_t>(renderer->glGetUniformLocation(m_id, uniformName.data())),
                                   Conversions::GL2UniformType(type) });
    }

    bool result = true;
    for (const auto &uniformInfo : m_uniformsInfo)
        if ((uniformInfo.id != core::graphics::UniformId::Undefined) && (uniformInfo.type != uniformTypeByUniformId(uniformInfo.id)))
        {
            renderer->glGetActiveUniform(m_id,
                                         static_cast<GLuint>(uniformInfo.index),
                                         static_cast<GLsizei>(m_uniformNameMaxLength),
                                         &length,
                                         &size,
                                         &type,
                                         uniformName.data());

            result = false;
            log = "Uniform variable \"" + std::string(uniformName.data()) + "\" has wrong type";
            break;
        }

    return result;
}

int32_t RenderProgram_4_5::attributeLocationByName(const std::string &value) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return renderer->glGetAttribLocation(m_id, value.data());
}

int32_t RenderProgram_4_5::uniformLocationByName(const std::string &value) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return renderer->glGetUniformLocation(m_id, value.data());
}

const std::vector<core::graphics::AttributeInfo> &RenderProgram_4_5::attributesInfo() const
{
    return m_attributesInfo;
}

const std::vector<core::graphics::UniformInfo> &RenderProgram_4_5::uniformsInfo() const
{
    return m_uniformsInfo;
}

std::string RenderProgram_4_5::attributeNameByIndex(uint16_t index) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLsizei length;
    GLint size;
    GLenum type;
    std::vector<GLchar> name(static_cast<size_t>(m_attributeNameMaxLength));

    renderer->glGetActiveAttrib(m_id,
                                static_cast<GLuint>(index),
                                static_cast<GLsizei>(m_attributeNameMaxLength),
                                &length,
                                &size,
                                &type,
                                name.data());

    return name.data();
}

std::string RenderProgram_4_5::uniformNameByIndex(uint16_t index) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLsizei length;
    GLint size;
    GLenum type;
    std::vector<GLchar> name(static_cast<size_t>(m_uniformNameMaxLength));

    renderer->glGetActiveUniform(m_id,
                                 static_cast<GLuint>(index),
                                 static_cast<GLsizei>(m_uniformNameMaxLength),
                                 &length,
                                 &size,
                                 &type,
                                 name.data());

    return name.data();
}

// QtOpenGL_4_5_Renderer

QtOpenGL_4_5_Renderer::~QtOpenGL_4_5_Renderer()
{
    auto context = owningContext();
    assert(context);
    assert(s_instances.count(context));
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

std::shared_ptr<core::graphics::IRenderProgram> QtOpenGL_4_5_Renderer::createRenderProgram(const std::string &vertexShader,
                                                                                           const std::string &fragmentShader) const
{
    static const size_t s_shadersCount = 2;
    static const std::array<std::string, s_shadersCount> s_shaderTypes { "Vertex", "Fragment" };
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    std::array<std::pair<GLenum, std::reference_wrapper<const std::string>>, s_shadersCount> shadersData {
        std::make_pair(GL_VERTEX_SHADER, std::cref(vertexShader)),
        std::make_pair(GL_FRAGMENT_SHADER, std::cref(fragmentShader))
    };
    std::array<GLuint, s_shadersCount> shadersIds {0u, 0u};
    std::shared_ptr<RenderProgram_4_5> renderProgram;
    bool isOk = true;

    for (size_t i = 0; i < s_shadersCount; ++i)
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
                LOG_ERROR << s_shaderTypes[i] << " shader compile error: " << infoLog;
                free(infoLog);
            }
            isOk = false;
        }
    }

    if (isOk)
    {
        renderProgram = std::make_shared<RenderProgram_4_5>();
        for (size_t i = 0; i < s_shadersCount; ++i)
            renderer->glAttachShader(renderProgram->id(), shadersIds[i]);
        renderer->glLinkProgram(renderProgram->id());
        GLint linked;
        renderer->glGetProgramiv(renderProgram->id(), GL_LINK_STATUS, &linked);
        if (!linked) {
            GLint infoLen = 0;
            renderer->glGetProgramiv(renderProgram->id(), GL_INFO_LOG_LENGTH, &infoLen);
            if(infoLen > 1) {
                char *infoLog = static_cast<char*>(malloc(sizeof(char) * static_cast<unsigned int>(infoLen)));
                renderer->glGetProgramInfoLog(renderProgram->id(), infoLen, nullptr, infoLog);
                LOG_ERROR << "Program link error: " << infoLog;
                free(infoLog);
            }
            isOk = false;
        }
    }

    if (isOk)
    {
        std::string logString;
        if (!renderProgram->compile(logString))
        {
            isOk = false;
            LOG_ERROR << "Program compile error: " << logString;
        }
    }

    if (!isOk)
    {
        renderProgram = nullptr;
        for (size_t i = 0; i < s_shadersCount; ++i)
            if (renderer->glIsShader(shadersIds[i]) == GL_TRUE)
                renderer->glDeleteShader(shadersIds[i]);
    }

    return renderProgram;
}

std::shared_ptr<core::graphics::IBuffer> QtOpenGL_4_5_Renderer::createBuffer(size_t size, const void *data) const
{
    return std::make_shared<Buffer_4_5>(size, data);
}

std::shared_ptr<core::graphics::IVertexArray> QtOpenGL_4_5_Renderer::createVertexArray(std::shared_ptr<utils::Mesh> mesh,
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
                assert(numVertices == buffer->numVertices());
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
                assert(numVertices == buffer->numVertices());
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

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture2DEmpty(uint32_t width,
                                                                                      uint32_t height,
                                                                                      core::graphics::PixelInternalFormat internalFormat,
                                                                                      uint32_t numLevels) const
{
    assert(width * height);
    assert(internalFormat != core::graphics::PixelInternalFormat::Undefined);

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec2(width, height)));
    if (numLevels == 0u)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    auto result = std::make_shared<Texture2D_4_5>(width, height, internalFormat, numLevels);
    result->setFiltering(numLevels > 1u ? core::graphics::TextureFiltering::Trilinear : core::graphics::TextureFiltering::Linear);

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTexture2D(std::shared_ptr<utils::Image> image,
                                                                                 core::graphics::PixelInternalFormat internalFormat,
                                                                                 uint32_t numLevels,
                                                                                 bool genMipmaps) const
{
    assert(image);

    if (internalFormat == core::graphics::PixelInternalFormat::Undefined)
        internalFormat = Conversions::PixelNumComponentsAndPixelComponentTypeToPixelInternalFormat(image->numComponents(), image->type());

    auto result = createTexture2DEmpty(image->width(), image->height(), internalFormat, numLevels);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), image->height(), 0u), image->numComponents(), image->type(), image->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::graphics::ITexture> QtOpenGL_4_5_Renderer::createTextureRectEmpty(uint32_t width,
                                                                                        uint32_t height,
                                                                                        core::graphics::PixelInternalFormat internalFormat) const
{
    assert(width * height);
    assert(internalFormat != core::graphics::PixelInternalFormat::Undefined);

    auto result = std::make_shared<TextureRect_4_5>(width, height, internalFormat);
    //result->setFiltering(core::graphics::TextureFiltering::Point);

    return result;
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

void QtOpenGL_4_5_Renderer::addRenderData(const glm::mat4 &transform, std::shared_ptr<core::IDrawable> drawable)
{
    m_renderData.push_back({transform, drawable});
}

void QtOpenGL_4_5_Renderer::render(const core::RenderInfo &renderInfo)
{
    auto fbo = std::dynamic_pointer_cast<FrameBufferBase_4_5>(renderInfo.frameBuffer());
    assert(fbo);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->id());
    fbo->clear();

    if (renderInfo.faceCulling())
    {
        glCullFace(Conversions::faceType2GL(renderInfo.cullFaceType()));
        glEnable(GL_CULL_FACE);
    }
    else
        glDisable(GL_CULL_FACE);

    for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    {
        GLboolean mask = renderInfo.colorMask(i);
        glColorMaski(i, mask, mask, mask, mask);
    }

    if (renderInfo.depthTest())
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(Conversions::depthFunc2GL(renderInfo.depthFunc()));
        glDepthMask(renderInfo.depthMask());
    }
    else
        glDisable(GL_DEPTH_TEST);

    if (renderInfo.stencilTest())
    {
        glEnable(GL_STENCIL_TEST);
    }
    else
        glDisable(GL_STENCIL_TEST);

    glViewport(static_cast<GLint>(renderInfo.viewport().x),
               static_cast<GLint>(renderInfo.viewport().y),
               static_cast<GLsizei>(renderInfo.viewport().z),
               static_cast<GLsizei>(renderInfo.viewport().w));

    for (auto &renderData : m_renderData)
    {
        auto rp = std::dynamic_pointer_cast<RenderProgram_4_5>(renderData.second->renderProgram());
        auto vao = std::dynamic_pointer_cast<VertexArray_4_5>(renderData.second->vertexArray());

        glUseProgram(rp->id());
        glBindVertexArray(vao->id());

        m_textureUnit = 0;

        setupUniforms(renderData.second, renderInfo, renderData.first);

        for (auto &primitiveSet : vao->primitiveSets())
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
}

int32_t QtOpenGL_4_5_Renderer::bindTexture(std::shared_ptr<const core::graphics::ITexture> texture)
{
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(texture);
    assert(textureBase);

    glBindTextureUnit(static_cast<GLuint>(m_textureUnit), textureBase->id());
    return m_textureUnit++;
}

std::shared_ptr<QtOpenGL_4_5_Renderer> QtOpenGL_4_5_Renderer::create(QOpenGLContext *context, GLuint defaultFbo)
{
    if (auto renderer = instance(context); renderer)
    {
        LOG_WARNING << "QtOpenGL_4_5_Renderer instance has already been created";
        return renderer;
    }

    auto renderer = std::shared_ptr<QtOpenGL_4_5_Renderer>(new QtOpenGL_4_5_Renderer(context, defaultFbo));

    s_instances.insert({context, renderer});
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
    {
        LOG_CRITICAL << "Can't initialize QOpenGLFunctions";
        assert(false);
    }

    makeDefaultFrameBuffer(defaultFbo);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    LOG_INFO << "GraphicsRenderer \"" << QtOpenGL_4_5_Renderer::name() << "\" has been created";
}

void QtOpenGL_4_5_Renderer::makeDefaultFrameBuffer(GLuint defaultFbo)
{
    m_defaultFrameBuffer = std::make_shared<DefaultFrameBuffer_4_5>(defaultFbo);
    auto &attachments = const_cast<core::graphics::IFrameBuffer::Attachments&>(m_defaultFrameBuffer->attachments());

    GLint objType = GL_NONE;
    glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    if (objType != GL_NONE)
        attachments.insert({core::graphics::FrameBufferAttachment::DepthStencil, {nullptr, 0, 0}});
    else
    {
        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments.insert({core::graphics::FrameBufferAttachment::Depth, {nullptr, 0, 0}});

        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments.insert({core::graphics::FrameBufferAttachment::Stencil, {nullptr, 0, 0}});
    }
    for (uint16_t i = 0; i < core::graphics::FrameBufferColorAttachmentsCount(); ++i)
    {
        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments.insert({core::graphics::FrameBufferColorAttachment(i), {nullptr, 0, 0}});
    }
}

void QtOpenGL_4_5_Renderer::setupUniform(GLuint rpId,
                                         GLint loc,
                                         std::shared_ptr<const core::AbstractUniform> uni)
{
    assert(uni);

    switch (uni->type())
    {
    case core::graphics::UniformType::Single:
    {
        glProgramUniform1f(rpId, loc, core::uniform_cast<float>(uni)->data());
        break;
    }
    case core::graphics::UniformType::SingleVec2:
    {
        glProgramUniform2fv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::vec2>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::SingleVec3:
    {
        glProgramUniform3fv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::vec3>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::SingleVec4:
    {
        glProgramUniform4fv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::vec4>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::SingleMat2:
    {
        glProgramUniformMatrix2fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::mat2x2>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::SingleMat3:
    {
        glProgramUniformMatrix3fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::mat3x3>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::SingleMat4:
    {
        glProgramUniformMatrix4fv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::mat4x4>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::Double:
    {
        glProgramUniform1d(rpId, loc, core::uniform_cast<double>(uni)->data());
        break;
    }
    case core::graphics::UniformType::DoubleVec2:
    {
        glProgramUniform2dv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::dvec2>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleVec3:
    {
        glProgramUniform3dv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::dvec3>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleVec4:
    {
        glProgramUniform4dv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::dvec4>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleMat2:
    {
        glProgramUniformMatrix2dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::dmat2x2>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleMat3:
    {
        glProgramUniformMatrix3dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::dmat3x3>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::DoubleMat4:
    {
        glProgramUniformMatrix4dv(rpId, loc, 1, GL_FALSE, glm::value_ptr(core::uniform_cast<glm::dmat4x4>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::Int32:
    {
        glProgramUniform1i(rpId, loc, core::uniform_cast<int32_t>(uni)->data());
        break;
    }
    case core::graphics::UniformType::Int32Vec2:
    {
        glProgramUniform2iv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::i32vec2>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::Int32Vec3:
    {
        glProgramUniform3iv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::i32vec3>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::Int32Vec4:
    {
        glProgramUniform4iv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::i32vec4>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::Uint32:
    {
        glProgramUniform1ui(rpId, loc, core::uniform_cast<uint32_t>(uni)->data());
        break;
    }
    case core::graphics::UniformType::Uint32Vec2:
    {
        glProgramUniform2uiv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::u32vec2>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::Uint32Vec3:
    {
        glProgramUniform3uiv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::u32vec3>(uni)->data()));
        break;
    }
    case core::graphics::UniformType::Uint32Vec4:
    {
        glProgramUniform4uiv(rpId, loc, 1, glm::value_ptr(core::uniform_cast<glm::u32vec4>(uni)->data()));
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
        auto uniform = core::uniform_cast<std::shared_ptr<const core::graphics::ITexture>>(uni)->data();
        glProgramUniform1i(rpId, loc, bindTexture(uniform));
        break;
    }
    default:
    {
        break;
    }
    }
}

void QtOpenGL_4_5_Renderer::setupUniforms(std::shared_ptr<core::IDrawable> drawable,
                                          const core::RenderInfo &renderInfo,
                                          const glm::mat4 &modelMatrix)
{
    auto renderProgram = std::dynamic_pointer_cast<RenderProgram_4_5>(drawable->renderProgram());
    assert(renderProgram);

    auto renderProgramId = renderProgram->id();

    for (const auto &uniformInfo : renderProgram->uniformsInfo())
    {
        switch (uniformInfo.id)
        {
        case core::graphics::UniformId::Undefined: {
            setupUniform(renderProgramId, uniformInfo.location, drawable->uniform(uniformInfo));
            break;
        }
        case core::graphics::UniformId::Viewport: {
            glProgramUniform2uiv(renderProgramId,
                                   uniformInfo.location,
                                   1,
                                   glm::value_ptr(glm::uvec2(renderInfo.viewport().z, renderInfo.viewport().w)));
            break;
        }
        case core::graphics::UniformId::ModelMatrix: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(modelMatrix));
            break;
        }
        case core::graphics::UniformId::NormalMatrix: {
            glProgramUniformMatrix3fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(glm::mat3x3(glm::inverseTranspose(modelMatrix))));
            break;
        }
        case core::graphics::UniformId::ViewMatrix: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.viewMatrix()));
            break;
        }
        case core::graphics::UniformId::ViewMatrixInverse: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.viewMatrixInverse()));
            break;
        }
        case core::graphics::UniformId::ProjectionMatrix: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.projectionMatrix()));
            break;
        }
        case core::graphics::UniformId::ProjectionMatrixInverse: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.projectionMatrixInverse()));
            break;
        }
        case core::graphics::UniformId::ViewProjectionMatrix: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.viewProjectionMatrix()));
            break;
        }
        case core::graphics::UniformId::ViewProjectionMatrixInverse: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.viewProjectionMatrixInverse()));
            break;
        }
        case core::graphics::UniformId::ModelViewMatrix: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.viewMatrix() * modelMatrix));
            break;
        }
        case core::graphics::UniformId::NormalViewMatrix: {
            glProgramUniformMatrix3fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(glm::mat3x3(glm::inverseTranspose(renderInfo.viewMatrix() * modelMatrix))));
            break;
        }
        case core::graphics::UniformId::ModelViewProjectionMatrix: {
            glProgramUniformMatrix4fv(renderProgramId,
                                                uniformInfo.location,
                                                1, GL_FALSE,
                                                glm::value_ptr(renderInfo.viewProjectionMatrix() * modelMatrix));
            break;
        }
        case core::graphics::UniformId::ViewPosition: {
            glProgramUniform3fv(renderProgramId,
                                          uniformInfo.location,
                                          1,
                                          glm::value_ptr(renderInfo.viewPosition()));
            break;
        }
        case core::graphics::UniformId::ViewXDirection: {
            glProgramUniform3fv(renderProgramId,
                                          uniformInfo.location,
                                          1,
                                          glm::value_ptr(renderInfo.viewXDirection()));
            break;
        }
        case core::graphics::UniformId::ViewYDirection: {
            glProgramUniform3fv(renderProgramId,
                                          uniformInfo.location,
                                          1,
                                          glm::value_ptr(renderInfo.viewYDirection()));
            break;
        }
        case core::graphics::UniformId::ViewZDirection: {
            glProgramUniform3fv(renderProgramId,
                                          uniformInfo.location,
                                          1,
                                          glm::value_ptr(renderInfo.viewZDirection()));
            break;
        }
        case core::graphics::UniformId::GBufferColor0Map: {
            glProgramUniform1i(renderProgramId,
                                uniformInfo.location,
                                bindTexture(renderInfo.gBufferColor0Map()));
            break;
        }
        case core::graphics::UniformId::GBufferColor1Map: {
            glProgramUniform1i(renderProgramId,
                                uniformInfo.location,
                                bindTexture(renderInfo.gBufferColor1Map()));
            break;
        }
        case core::graphics::UniformId::GBufferDepthMap: {
            glProgramUniform1i(renderProgramId,
                                uniformInfo.location,
                                bindTexture(renderInfo.gBufferDepthMap()));
            break;
        }
        default: {
            break;
        }
        }
    }
}


}
}
