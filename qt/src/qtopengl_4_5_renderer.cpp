#include <array>
#include <functional>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/glm/gtc/matrix_inverse.hpp>
#include <utils/glm/gtx/texture.hpp>
#include <utils/logger.h>
#include <utils/typeinfo.h>
#include <utils/mesh.h>
#include <utils/image.h>

#include <core/idrawable.h>
#include <core/renderinfo.h>

#include "qtopengl_4_5_renderer.h"

namespace simplex
{
namespace qt
{

std::weak_ptr<QtOpenGL_4_5_Renderer> QtOpenGL_4_5_Renderer::s_instance;

QtOpenGL_4_5_Renderer::RenderProgram_4_5::RenderProgram_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    m_id = renderer->glCreateProgram();
}

QtOpenGL_4_5_Renderer::RenderProgram_4_5::~RenderProgram_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLint count;
    renderer->glGetProgramiv(m_id, GL_ATTACHED_SHADERS, &count);

    std::vector<GLuint> shaders(static_cast<size_t>(count));
    renderer->glGetAttachedShaders(m_id, 2, &count, shaders.data());

    for (auto shaderId : shaders)
    {
        renderer->glDetachShader(m_id, shaderId);
        renderer->glDeleteShader(shaderId);
    }

    renderer->glDeleteProgram(m_id);
}

GLuint QtOpenGL_4_5_Renderer::RenderProgram_4_5::id() const
{
    return m_id;
}

bool QtOpenGL_4_5_Renderer::RenderProgram_4_5::compile(std::string &log)
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
                                     Conversions::GL2Type(type) });
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
                                   Conversions::GL2Type(type) });
    }

    bool result = true;
    for (const auto &uniformInfo : m_uniformsInfo)
        if ((uniformInfo.id != UniformId::Count) && (uniformInfo.type != typeByUniformId(uniformInfo.id)))
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

const std::vector<core::IGraphicsRenderer::RenderProgram::AttributeInfo> &QtOpenGL_4_5_Renderer::RenderProgram_4_5::attributesInfo() const
{
    return m_attributesInfo;
}

const std::vector<core::IGraphicsRenderer::RenderProgram::UniformInfo> &QtOpenGL_4_5_Renderer::RenderProgram_4_5::uniformsInfo() const
{
    return m_uniformsInfo;
}

std::string QtOpenGL_4_5_Renderer::RenderProgram_4_5::attributeNameByIndex(uint16_t index) const
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

std::string QtOpenGL_4_5_Renderer::RenderProgram_4_5::uniformNameByIndex(uint16_t index) const
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

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, float v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform1fv(m_id, loc, 1, &v);
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::vec2 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform2fv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::vec3 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform3fv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::vec4 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform4fv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, int32_t v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform1iv(m_id, loc, 1, &v);
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::ivec2 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform2iv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::ivec3 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform3iv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::ivec4 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform4iv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, uint32_t v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform1uiv(m_id, loc, 1, &v);
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::uvec2 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform2uiv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::uvec3 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform3uiv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::uvec4 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform4uiv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat2 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix2fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat3 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix3fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat4 &v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix4fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, std::shared_ptr<const Texture> v)
{
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(v);
    if (!textureBase)
        return;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    auto unit = renderer->bindTexture(v);
    renderer->glProgramUniform1uiv(m_id, loc, 1, &unit);
}

QtOpenGL_4_5_Renderer::Buffer_4_5::Buffer_4_5(uint64_t size, const void *data)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateBuffers(1, &m_id);
    Buffer_4_5::resize(size, data);
}

QtOpenGL_4_5_Renderer::Buffer_4_5::~Buffer_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteBuffers(1, &m_id);
}

GLuint QtOpenGL_4_5_Renderer::Buffer_4_5::id() const
{
    return m_id;
}

size_t QtOpenGL_4_5_Renderer::Buffer_4_5::size() const
{
    GLint result;
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetNamedBufferParameteriv(m_id, GL_BUFFER_SIZE, &result);
    return static_cast<size_t>(result);
}

void QtOpenGL_4_5_Renderer::Buffer_4_5::resize(size_t size, const void *data)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glNamedBufferData(m_id, static_cast<GLsizei>(size), data, GL_STATIC_DRAW);
}

std::unique_ptr<const core::IGraphicsRenderer::Buffer::MappedData> QtOpenGL_4_5_Renderer::Buffer_4_5::map(MapAccess access, size_t offset, size_t size) const
{
    return const_cast<Buffer_4_5*>(this)->map(access, offset, size);
}

std::unique_ptr<core::IGraphicsRenderer::Buffer::MappedData> QtOpenGL_4_5_Renderer::Buffer_4_5::map(MapAccess access, size_t offset, size_t size)
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
                static_cast<uint8_t*>(renderer->glMapNamedBufferRange(m_id, static_cast<GLintptr>(offset), static_cast<GLsizei>(size), MapAccess2GL(access)))
                );
}

GLbitfield QtOpenGL_4_5_Renderer::Buffer_4_5::MapAccess2GL(MapAccess value)
{
    static std::array<GLbitfield, numElementsMapAccess()> s_table {
        GL_MAP_READ_BIT,
        GL_MAP_WRITE_BIT,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT
    };

    return s_table[castFromMapAccess(value)];
}

QtOpenGL_4_5_Renderer::VertexArray_4_5::VertexArray_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateVertexArrays(1, &m_id);
}

QtOpenGL_4_5_Renderer::VertexArray_4_5::~VertexArray_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteVertexArrays(1, &m_id);
}

GLuint QtOpenGL_4_5_Renderer::VertexArray_4_5::id() const
{
    return m_id;
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::attachVertexBuffer(std::shared_ptr<Buffer> buffer, size_t offset, uint32_t stride)
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

void QtOpenGL_4_5_Renderer::VertexArray_4_5::detachVertexBuffer(uint32_t bindingIndex)
{
    m_vertexBuffers[bindingIndex] = std::make_tuple(nullptr, static_cast<size_t>(0), static_cast<uint32_t>(0));

    for (const auto &[attrib, tuple]: m_vertexDeclarations)
        if (std::get<0>(tuple) == bindingIndex)
            undeclareVertexAttribute(attrib);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayVertexBuffer(m_id, static_cast<GLuint>(bindingIndex), 0, 0, 0);

}

std::shared_ptr<const core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexBuffer(uint32_t bindingIndex) const
{
    return std::get<0>(m_vertexBuffers[bindingIndex]);
}

size_t QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexBufferOffset(uint32_t bindingIndex) const
{
    return std::get<1>(m_vertexBuffers[bindingIndex]);
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexBufferStride(uint32_t bindingIndex) const
{
    return std::get<2>(m_vertexBuffers[bindingIndex]);
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::declareVertexAttribute(utils::VertexAttribute attrib,
                                                                    uint32_t bindingIndex,
                                                                    uint32_t numComponents,
                                                                    utils::Type type,
                                                                    uint32_t relativeOffset)
{
    assert(numComponents <= 4);
    assert(utils::TypeInfo::isScalar(type));

    m_vertexDeclarations[attrib] = std::make_tuple(bindingIndex, numComponents, type, relativeOffset);

    auto loc = static_cast<GLuint>(attrib);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayAttribBinding(m_id, loc, static_cast<GLuint>(bindingIndex));
    renderer->glEnableVertexArrayAttrib(m_id, loc);

    if (utils::TypeInfo::isSingleScalar(type))
        renderer->glVertexArrayAttribFormat(m_id, loc, static_cast<GLint>(numComponents), Conversions::Type2GL(type), GL_FALSE, static_cast<GLuint>(relativeOffset));
    else if (utils::TypeInfo::isDoubleScalar(type))
        renderer->glVertexArrayAttribLFormat(m_id, loc, static_cast<GLint>(numComponents), Conversions::Type2GL(type), static_cast<GLuint>(relativeOffset));
    else if (utils::TypeInfo::isIntScalar(type))
        renderer->glVertexArrayAttribIFormat(m_id, loc, static_cast<GLint>(numComponents), Conversions::Type2GL(type), static_cast<GLuint>(relativeOffset));
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::undeclareVertexAttribute(utils::VertexAttribute attrib)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDisableVertexArrayAttrib(m_id, static_cast<GLuint>(attrib));

    m_vertexDeclarations.erase(attrib);
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexAttributeBindingIndex(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<0>(it->second) : static_cast<uint32_t>(-1);
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexAttributeNumComponents(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<1>(it->second) : 0;
}

utils::Type QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexAttributeType(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<2>(it->second) : utils::Type::Single;
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexAttributeRelativeOffset(utils::VertexAttribute attrib) const
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<3>(it->second) : 0;
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::attachIndexBuffer(std::shared_ptr<Buffer> buffer)
{
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    assert(buffer_4_5);

    m_indexBuffer = buffer_4_5;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayElementBuffer(m_id, buffer_4_5->id());
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::detachIndexBuffer()
{
    m_indexBuffer = nullptr;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayElementBuffer(m_id, 0);
}

std::shared_ptr<const core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::VertexArray_4_5::indexBuffer() const
{
    return m_indexBuffer;
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::addPrimitiveSet(std::shared_ptr<utils::PrimitiveSet> primitiveSet)
{
    assert(primitiveSet);
    m_primitiveSets.insert(primitiveSet);
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::removePrimitiveSet(std::shared_ptr<utils::PrimitiveSet> primitiveSet)
{
    m_primitiveSets.erase(primitiveSet);
}

const std::unordered_set<std::shared_ptr<utils::PrimitiveSet> > QtOpenGL_4_5_Renderer::VertexArray_4_5::primitiveSets() const
{
    return m_primitiveSets;
}

std::shared_ptr<core::IGraphicsRenderer::FrameBuffer> QtOpenGL_4_5_Renderer::defaultFrameBuffer()
{
    return m_defaultFrameBuffer;
}

std::shared_ptr<const core::IGraphicsRenderer::FrameBuffer> QtOpenGL_4_5_Renderer::defaultFrameBuffer() const
{
    return const_cast<QtOpenGL_4_5_Renderer*>(this)->defaultFrameBuffer();
}

std::shared_ptr<core::IGraphicsRenderer::RenderProgram> QtOpenGL_4_5_Renderer::createRenderProgram(const std::string &vertexShader, const std::string &fragmentShader) const
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

std::shared_ptr<core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::createBuffer(size_t size, const void *data) const
{
    return std::make_shared<Buffer_4_5>(size, data);
}

std::shared_ptr<core::IGraphicsRenderer::VertexArray> QtOpenGL_4_5_Renderer::createVertexArray(std::shared_ptr<utils::Mesh> mesh,
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
                stride += buffer->numComponents() * utils::TypeInfo::size(buffer->type());
            }

            auto buffer = createBuffer(totalSize);
            auto bindingIndex = vertexArray->attachVertexBuffer(buffer, 0u, stride);

            uint32_t relativeOffset = 0u;
            auto bufferData = buffer->map(Buffer::MapAccess::WriteOnly);
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                uint32_t vertexSize = buffer->numComponents() * utils::TypeInfo::size(buffer->type());
                for (uint32_t i = 0; i < buffer->numVertices(); ++i)
                    std::memcpy(static_cast<uint8_t*>(bufferData->get()) + stride * i + relativeOffset, buffer->vertex(i), vertexSize);
                vertexArray->declareVertexAttribute(attrib, bindingIndex, buffer->numComponents(), buffer->type(), relativeOffset);
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
                                                                    buffer->numComponents() * utils::TypeInfo::size(buffer->type()));
                vertexArray->declareVertexAttribute(attrib, bindingIndex, buffer->numComponents(), buffer->type(), 0u);
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

            auto bufferData = buffer->map(Buffer::MapAccess::WriteOnly);
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

std::shared_ptr<core::IGraphicsRenderer::Texture> QtOpenGL_4_5_Renderer::createTexture2DEmpty(uint32_t width,
                                                                                              uint32_t height,
                                                                                              RenderSurface::InternalFormat internalFormat,
                                                                                              uint32_t numLevels) const
{
    assert(width * height);
    assert(internalFormat != RenderSurface::InternalFormat::Undefined);

    auto numMipmapLevels = static_cast<uint32_t>(glm::levels(glm::uvec2(width, height)));
    if (numLevels == 0)
        numLevels = numMipmapLevels;
    numLevels = glm::min(numLevels, numMipmapLevels);

    return std::make_shared<Texture2D_4_5>(width, height, internalFormat, numLevels);
}

std::shared_ptr<core::IGraphicsRenderer::Texture> QtOpenGL_4_5_Renderer::createTexture2D(std::shared_ptr<utils::Image> image,
                                                                                         RenderSurface::InternalFormat internalFormat,
                                                                                         uint32_t numLevels, bool genMipmaps) const
{
    assert(image);

    if (internalFormat == RenderSurface::InternalFormat::Undefined)
        internalFormat = Conversions::NumComponentsAndTypeToInternalFormat(image->numComponents(), image->type());

    auto result = createTexture2DEmpty(image->width(), image->height(), internalFormat, numLevels);
    result->setSubImage(0u, glm::uvec3(0u), glm::uvec3(image->width(), image->height(), 0u), image->numComponents(), image->type(), image->data());

    if (genMipmaps)
        result->generateMipmaps();

    return result;
}

std::shared_ptr<core::IGraphicsRenderer::RenderBuffer> QtOpenGL_4_5_Renderer::createRenderBuffer(uint32_t width,
                                                                                                 uint32_t height,
                                                                                                 RenderSurface::InternalFormat internalFormat) const
{
    return std::make_shared<RenderBuffer_4_5>(width, height, internalFormat);
}

std::shared_ptr<core::IGraphicsRenderer::FrameBuffer> QtOpenGL_4_5_Renderer::createFrameBuffer() const
{
    return std::make_shared<FrameBuffer_4_5>();
}

QtOpenGL_4_5_Renderer::QtOpenGL_4_5_Renderer(const QOpenGLContext *context, GLuint defaultFbo)
    : m_viewportSize(0u, 0u)
{
    if (!s_instance.expired())
    {
        LOG_CRITICAL << "QtOpenGL_4_5_Renderer instance has already been created";
        assert(false);
    }

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

QtOpenGL_4_5_Renderer::~QtOpenGL_4_5_Renderer()
{
    LOG_INFO << "GraphicsRenderer \"" << QtOpenGL_4_5_Renderer::name() << "\" has been destroyed";
}

void QtOpenGL_4_5_Renderer::makeDefaultFrameBuffer(GLuint defaultFbo)
{
    m_defaultFrameBuffer = std::make_shared<DefaultFrameBuffer_4_5>(defaultFbo);
    auto &attachments = const_cast<FrameBuffer::Attachments&>(m_defaultFrameBuffer->attachments());

    GLint objType = GL_NONE;
    glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
    if (objType != GL_NONE)
        attachments.insert({FrameBuffer::Attachment::DepthStencil, {nullptr, 0, 0}});
    else
    {
        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments.insert({FrameBuffer::Attachment::Depth, {nullptr, 0, 0}});

        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments.insert({FrameBuffer::Attachment::Stencil, {nullptr, 0, 0}});
    }
    for (uint32_t i = 0; i < FrameBuffer::ColorAttachmentsCount(); ++i)
    {
        objType = GL_NONE;
        glGetNamedFramebufferAttachmentParameteriv(defaultFbo, GL_COLOR_ATTACHMENT0 + i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &objType);
        if (objType != GL_NONE)
            attachments.insert({FrameBuffer::ColorAttachment(i), {nullptr, 0, 0}});
    }

}

void QtOpenGL_4_5_Renderer::setInstance(std::shared_ptr<QtOpenGL_4_5_Renderer> instance)
{
    s_instance = instance;
}

std::shared_ptr<QtOpenGL_4_5_Renderer> QtOpenGL_4_5_Renderer::instance()
{
    return s_instance.expired() ? nullptr : s_instance.lock();
}

const std::string &QtOpenGL_4_5_Renderer::name() const
{
    static const std::string s_name = "QtOpenGL_4_5_Renderer";
    return s_name;
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

    fbo->clear();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->id());

    glViewport(static_cast<GLint>(renderInfo.viewport().x),
               static_cast<GLint>(renderInfo.viewport().y),
               static_cast<GLsizei>(renderInfo.viewport().z),
               static_cast<GLsizei>(renderInfo.viewport().w));

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    for (auto &renderData : m_renderData)
    {
        auto rp = std::dynamic_pointer_cast<RenderProgram_4_5>(renderData.second->renderProgram());
        auto vao = std::dynamic_pointer_cast<VertexArray_4_5>(renderData.second->vertexArray());

        glUseProgram(rp->id());
        glBindVertexArray(vao->id());

        m_textureUnit = 0u;

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
                                         Conversions::Type2GL(drawElements->type()),
                                         reinterpret_cast<const void*>(drawElements->offset()),
                                         static_cast<GLint>(drawElements->baseVertex()));
        }
    }
}

uint32_t QtOpenGL_4_5_Renderer::bindTexture(std::shared_ptr<const Texture> texture)
{
    auto textureBase = std::dynamic_pointer_cast<const TextureBase_4_5>(texture);
    assert(textureBase);

    glBindTextureUnit(m_textureUnit, textureBase->id());
    return m_textureUnit++;
}

void QtOpenGL_4_5_Renderer::setupUniforms(std::shared_ptr<core::IDrawable> drawable,
                                          const core::RenderInfo &renderInfo,
                                          const glm::mat4 &modelMatrix)
{
    auto renderProgram = drawable->renderProgram();

    for (const auto &uniformInfo : renderProgram->uniformsInfo())
    {
        switch (uniformInfo.id)
        {
        case RenderProgram::UniformId::Viewport: {
            renderProgram->setUniform(uniformInfo.location, glm::uvec2(renderInfo.viewport().z, renderInfo.viewport().w));
            break;
        }
        case RenderProgram::UniformId::ModelMatrix: {
            renderProgram->setUniform(uniformInfo.location, modelMatrix);
            break;
        }
        case RenderProgram::UniformId::NormalMatrix: {
            renderProgram->setUniform(uniformInfo.location, glm::inverseTranspose(modelMatrix));
            break;
        }
        case RenderProgram::UniformId::ViewMatrix: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewMatrix());
            break;
        }
        case RenderProgram::UniformId::ViewMatrixInverse: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewMatrixInverse());
            break;
        }
        case RenderProgram::UniformId::ProjectionMatrix: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.projectionMatrix());
            break;
        }
        case RenderProgram::UniformId::ProjectionMatrixInverse: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.projectionMatrixInverse());
            break;
        }
        case RenderProgram::UniformId::ViewProjectionMatrix: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewProjectionMatrix());
            break;
        }
        case RenderProgram::UniformId::ViewProjectionMatrixInverse: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewProjectionMatrixInverse());
            break;
        }
        case RenderProgram::UniformId::ModelViewMatrix: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewMatrix() * modelMatrix);
            break;
        }
        case RenderProgram::UniformId::NormalViewMatrix: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewMatrix() * glm::inverseTranspose(modelMatrix));
            break;
        }
        case RenderProgram::UniformId::ModelViewProjectionMatrix: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewProjectionMatrix() * modelMatrix);
            break;
        }
        case RenderProgram::UniformId::ViewPosition: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewPosition());
            break;
        }
        case RenderProgram::UniformId::ViewXDirection: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewXDirection());
            break;
        }
        case RenderProgram::UniformId::ViewYDirection: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewYDirection());
            break;
        }
        case RenderProgram::UniformId::ViewZDirection: {
            renderProgram->setUniform(uniformInfo.location, renderInfo.viewZDirection());
            break;
        }
        case RenderProgram::UniformId::Count: {
            drawable->setupUniform(uniformInfo);
            break;
        }
        }
    }
}

QtOpenGL_4_5_Renderer::Buffer_4_5::MappedData_4_5::MappedData_4_5(std::shared_ptr<const Buffer_4_5> mappedBuffer, uint8_t *data)
    : m_mappedBuffer(mappedBuffer)
    , m_data(data)
{
}

QtOpenGL_4_5_Renderer::Buffer_4_5::MappedData_4_5::~MappedData_4_5()
{
    if (!m_mappedBuffer.expired())
    {
        auto buffer_4_5 = m_mappedBuffer.lock();
        auto instance = QtOpenGL_4_5_Renderer::instance();
        instance->glUnmapNamedBuffer(buffer_4_5->id());
        buffer_4_5->m_isMapped = false;
    }
}

const uint8_t *QtOpenGL_4_5_Renderer::Buffer_4_5::MappedData_4_5::get() const
{
    return const_cast<MappedData_4_5*>(this)->get();
}

uint8_t *QtOpenGL_4_5_Renderer::Buffer_4_5::MappedData_4_5::get()
{
    return m_mappedBuffer.expired() ? nullptr : m_data;
}

QtOpenGL_4_5_Renderer::TextureBase_4_5::TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
}

QtOpenGL_4_5_Renderer::TextureBase_4_5::~TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteTextures(1, &m_id);
}

GLuint QtOpenGL_4_5_Renderer::TextureBase_4_5::id() const
{
    return m_id;
}

glm::uvec2 QtOpenGL_4_5_Renderer::TextureBase_4_5::size() const
{
    return mipmapSize(0u);
}

glm::uvec3 QtOpenGL_4_5_Renderer::TextureBase_4_5::mipmapSize(uint32_t level) const
{
    GLint w, h, d;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_WIDTH, &w);
    renderer->glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_HEIGHT, &h);
    renderer->glGetTextureLevelParameteriv(m_id, static_cast<GLint>(level), GL_TEXTURE_DEPTH, &d);

    return glm::uvec3(w, h, d);
}

uint32_t QtOpenGL_4_5_Renderer::TextureBase_4_5::numMipmapLevels() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureParameteriv(m_id, GL_TEXTURE_MAX_LEVEL, &result);

    return static_cast<uint32_t>(result);
}

core::IGraphicsRenderer::Texture::InternalFormat QtOpenGL_4_5_Renderer::TextureBase_4_5::internalFormat() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetTextureLevelParameteriv(m_id, 0, GL_TEXTURE_INTERNAL_FORMAT, &result);

    return Conversions::GL2InternalFormat(static_cast<GLenum>(result));
}

void QtOpenGL_4_5_Renderer::TextureBase_4_5::generateMipmaps()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGenerateTextureMipmap(m_id);
}

void QtOpenGL_4_5_Renderer::TextureBase_4_5::setBorderColor(const glm::vec4 &value)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
}

void QtOpenGL_4_5_Renderer::TextureBase_4_5::setWrapMode(WrapMode value)
{
    auto glValue = static_cast<GLint>(Conversions::WrapMode2GL(value));

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, glValue);
    renderer->glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, glValue);
    renderer->glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, glValue);
}

void QtOpenGL_4_5_Renderer::TextureBase_4_5::setFiltering(Filtering value)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    switch (value)
    {
    case Texture::Filtering::Point: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    }
    case Texture::Filtering::Linear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    }
    case Texture::Filtering::Bilinear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;
    }
    case Texture::Filtering::Trilinear: {
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        renderer->glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;
    }
    default: break;
    }
}

QtOpenGL_4_5_Renderer::Texture2D_4_5::Texture2D_4_5(uint32_t width, uint32_t height, InternalFormat internalFormat, uint32_t numLevels)
    : TextureBase_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureStorage2D(m_id,
                                 static_cast<GLsizei>(numLevels),
                                 Conversions::InternalFormat2GL(internalFormat),
                                 static_cast<GLsizei>(width),
                                 static_cast<GLsizei>(height));
}

void QtOpenGL_4_5_Renderer::Texture2D_4_5::setSubImage(uint32_t level, const glm::uvec3 &offset, const glm::uvec3 &size, uint32_t numComponents, utils::Type type, const void *data)
{
    assert((numComponents >= 1) && (numComponents <= 4));
    assert(utils::TypeInfo::isScalar(type));

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glTextureSubImage2D(m_id,
                                  static_cast<GLint>(level),
                                  static_cast<GLint>(offset.x), static_cast<GLint>(offset.y),
                                  static_cast<GLint>(size.x), static_cast<GLint>(size.y),
                                  Conversions::NumComponents2GL(numComponents),
                                  Conversions::Type2GL(type),
                                  data);
}

QtOpenGL_4_5_Renderer::RenderBuffer_4_5::RenderBuffer_4_5(uint32_t width, uint32_t height, InternalFormat internalFormat)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateRenderbuffers(1, &m_id);
    renderer->glNamedRenderbufferStorage(m_id,
                                         Conversions::InternalFormat2GL(internalFormat),
                                         static_cast<GLsizei>(width),
                                         static_cast<GLsizei>(height));
}

QtOpenGL_4_5_Renderer::RenderBuffer_4_5::~RenderBuffer_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteRenderbuffers(1, &m_id);
}

GLuint QtOpenGL_4_5_Renderer::RenderBuffer_4_5::id() const
{
    return m_id;
}

glm::uvec2 QtOpenGL_4_5_Renderer::RenderBuffer_4_5::size() const
{
    GLint width, height;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_WIDTH, &width);
    renderer->glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_HEIGHT, &height);

    return glm::uvec2(width, height);
}

core::IGraphicsRenderer::RenderSurface::InternalFormat QtOpenGL_4_5_Renderer::RenderBuffer_4_5::internalFormat() const
{
    GLint result;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGetNamedRenderbufferParameteriv(m_id, GL_RENDERBUFFER_INTERNAL_FORMAT, &result);

    return Conversions::GL2InternalFormat(static_cast<GLenum>(result));
}

QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::FrameBufferBase_4_5(GLuint id)
    : m_id(id)
{
    for (uint32_t i = 0; i < ColorAttachmentsCount(); ++i)
        m_clearColor[i] = { utils::Type::Single, ClearColorValue{glm::vec4(.5f, .5f, 1.f, 1.f)} };
    m_clearDepth = 1.f;
    m_clearStencil = 0.f;
}

QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::~FrameBufferBase_4_5()
{
}

GLuint QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::id() const
{
    return m_id;
}

bool QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::isComplete() const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return renderer->glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

const core::IGraphicsRenderer::FrameBuffer::Attachments &QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::attachments() const
{
    return m_attchments;
}

const core::IGraphicsRenderer::FrameBuffer::ClearColor &QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::clearColor(uint32_t index) const
{
    assert(index < ColorAttachmentsCount());
    return m_clearColor[index];
}

void QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::vec4 &value)
{
    assert(index < ColorAttachmentsCount());
    m_clearColor[index] = { utils::Type::Single, ClearColorValue{value} };
}

void QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::i32vec4 &value)
{
    assert(index < ColorAttachmentsCount());
    m_clearColor[index] = { utils::Type::Int32, ClearColorValue{value} };
}

void QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::setClearColor(uint32_t index, const glm::u32vec4 &value)
{
    assert(index < ColorAttachmentsCount());
    m_clearColor[index] = { utils::Type::Uint32, ClearColorValue{value} };
}

float QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::clearDepth() const
{
    return m_clearDepth;
}

int32_t QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::clearStencil() const
{
    return m_clearStencil;
}

void QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::setClearDepthStencil(float depth, int32_t stencil)
{
    m_clearDepth = depth;
    m_clearStencil = stencil;
}

void QtOpenGL_4_5_Renderer::FrameBufferBase_4_5::clear()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    for (const auto &attachment : m_attchments)
    {
        switch (attachment.first)
        {
        case FrameBuffer::Attachment::Depth:
        {
            renderer->glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &m_clearDepth);
            break;
        }
        case FrameBuffer::Attachment::Stencil:
        {
            renderer->glClearNamedFramebufferiv(m_id, GL_STENCIL, 0, &m_clearStencil);
            break;
        }
        case FrameBuffer::Attachment::DepthStencil:
        {
            renderer->glClearNamedFramebufferfi(m_id, GL_DEPTH_STENCIL, m_clearDepth, m_clearStencil);
            break;
        }
        default:
        {
            auto drawBuffer = FrameBuffer::ColorAttachmentIndex(attachment.first);
            const auto &clearColor = m_clearColor[drawBuffer];
            switch (clearColor.first)
            {
            case utils::Type::Single: {
                renderer->glClearNamedFramebufferfv(m_id, GL_COLOR, static_cast<GLint>(drawBuffer), glm::value_ptr(clearColor.second.floatColor));
                break;
            }
            case utils::Type::Int32: {
                renderer->glClearNamedFramebufferiv(m_id, GL_COLOR, static_cast<GLint>(drawBuffer), glm::value_ptr(clearColor.second.intColor));
                break;
            }
            case utils::Type::Uint32: {
                renderer->glClearNamedFramebufferuiv(m_id, GL_COLOR, static_cast<GLint>(drawBuffer), glm::value_ptr(clearColor.second.uintColor));
                break;
            }
            default:
                break;
            }
            break;
        }
        }
    }
}

QtOpenGL_4_5_Renderer::FrameBuffer_4_5::FrameBuffer_4_5()
    : FrameBufferBase_4_5(0u)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glCreateFramebuffers(1, &m_id);
}

QtOpenGL_4_5_Renderer::FrameBuffer_4_5::~FrameBuffer_4_5()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glDeleteFramebuffers(1, &m_id);
}

void QtOpenGL_4_5_Renderer::FrameBuffer_4_5::attach(Attachment key, std::shared_ptr<RenderSurface> surface, uint32_t level, uint32_t layer)
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
                                                     Conversions::Attachment2GL(key),
                                                     texture->id(),
                                                     static_cast<GLint>(level),
                                                     static_cast<GLint>(layer));
        }
        else
        {
            renderer->glNamedFramebufferTexture(m_id, Conversions::Attachment2GL(key), texture->id(), static_cast<GLint>(level));
        }
        m_attchments[key] = { surface, level, layer };
    }
    else if (auto renderBuffer = std::dynamic_pointer_cast<RenderBuffer_4_5>(surface); renderBuffer)
    {
        auto renderer = QtOpenGL_4_5_Renderer::instance();
        renderer->glNamedFramebufferRenderbuffer(m_id,
                                                 Conversions::Attachment2GL(key),
                                                 GL_RENDERBUFFER,
                                                 renderBuffer->id());
        m_attchments[key] = { surface, level, layer };
    }
}

void QtOpenGL_4_5_Renderer::FrameBuffer_4_5::detach(Attachment key)
{
    auto it = m_attchments.find(key);
    if (it != m_attchments.end())
    {
        auto renderer = QtOpenGL_4_5_Renderer::instance();
        renderer->glNamedFramebufferRenderbuffer(m_id,
                                                 Conversions::Attachment2GL(key),
                                                 GL_RENDERBUFFER,
                                                 0u);
        m_attchments.erase(it);
    }
}

QtOpenGL_4_5_Renderer::DefaultFrameBuffer_4_5::DefaultFrameBuffer_4_5(GLuint defaultFbo)
    : FrameBufferBase_4_5(defaultFbo)
{
}

QtOpenGL_4_5_Renderer::DefaultFrameBuffer_4_5::~DefaultFrameBuffer_4_5()
{
}

void QtOpenGL_4_5_Renderer::DefaultFrameBuffer_4_5::attach(Attachment, std::shared_ptr<RenderSurface>, uint32_t, uint32_t)
{
}

void QtOpenGL_4_5_Renderer::DefaultFrameBuffer_4_5::detach(Attachment)
{
}

GLenum QtOpenGL_4_5_Renderer::Conversions::Type2GL(utils::Type value)
{
    static std::array<GLenum, utils::numElementsType()> s_table {
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
        GL_BYTE,
        GL_SHORT,
        GL_INT,
        GL_INT_VEC2,
        GL_INT_VEC3,
        GL_INT_VEC4,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
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
        GL_SAMPLER_1D_SHADOW,
        GL_SAMPLER_2D_SHADOW,
        GL_SAMPLER_CUBE_SHADOW,
        GL_SAMPLER_1D_ARRAY_SHADOW,
        GL_SAMPLER_2D_ARRAY_SHADOW,
        GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW
    };

    return s_table[utils::castFromType(value)];
}

utils::Type QtOpenGL_4_5_Renderer::Conversions::GL2Type(GLenum value)
{
    static std::unordered_map<GLenum, utils::Type> s_table {
        { GL_FLOAT, utils::Type::Single },
        { GL_FLOAT_VEC2, utils::Type::SingleVec2 },
        { GL_FLOAT_VEC3, utils::Type::SingleVec3 },
        { GL_FLOAT_VEC4, utils::Type::SingleVec4 },
        { GL_FLOAT_MAT2, utils::Type::SingleMat2 },
        { GL_FLOAT_MAT3, utils::Type::SingleMat3 },
        { GL_FLOAT_MAT4, utils::Type::SingleMat4 },
        { GL_DOUBLE, utils::Type::Double },
        { GL_DOUBLE_VEC2, utils::Type::DoubleVec2 },
        { GL_DOUBLE_VEC3, utils::Type::DoubleVec3 },
        { GL_DOUBLE_VEC4, utils::Type::DoubleVec4 },
        { GL_DOUBLE_MAT2, utils::Type::DoubleMat2 },
        { GL_DOUBLE_MAT3, utils::Type::DoubleMat3 },
        { GL_DOUBLE_MAT4, utils::Type::DoubleMat4 },
        { GL_BYTE, utils::Type::Int8 },
        { GL_SHORT, utils::Type::Int16 },
        { GL_INT, utils::Type::Int32 },
        { GL_INT_VEC2, utils::Type::Int32Vec2 },
        { GL_INT_VEC3, utils::Type::Int32Vec3 },
        { GL_INT_VEC4, utils::Type::Int32Vec4 },
        { GL_UNSIGNED_BYTE, utils::Type::Uint8 },
        { GL_UNSIGNED_SHORT, utils::Type::Uint16 },
        { GL_UNSIGNED_INT, utils::Type::Uint32 },
        { GL_UNSIGNED_INT_VEC2, utils::Type::Uint32Vec2 },
        { GL_UNSIGNED_INT_VEC3, utils::Type::Uint32Vec3 },
        { GL_UNSIGNED_INT_VEC4, utils::Type::Uint32Vec4 },
        { GL_SAMPLER_1D, utils::Type::Sampler1D },
        { GL_SAMPLER_2D, utils::Type::Sampler2D },
        { GL_SAMPLER_3D, utils::Type::Sampler3D },
        { GL_SAMPLER_CUBE, utils::Type::SamplerCube },
        { GL_SAMPLER_1D_ARRAY, utils::Type::Sampler1DArray },
        { GL_SAMPLER_2D_ARRAY, utils::Type::Sampler2DArray },
        { GL_SAMPLER_CUBE_MAP_ARRAY, utils::Type::SamplerCubeArray },
        { GL_SAMPLER_2D_RECT, utils::Type::SamplerRect },
        { GL_SAMPLER_1D_SHADOW, utils::Type::Sampler1DShadow },
        { GL_SAMPLER_2D_SHADOW, utils::Type::Sampler2DShadow },
        { GL_SAMPLER_CUBE_SHADOW, utils::Type::SamplerCubeShadow },
        { GL_SAMPLER_1D_ARRAY_SHADOW, utils::Type::Sampler1DArrayShadow },
        { GL_SAMPLER_2D_ARRAY_SHADOW, utils::Type::Sampler2DArrayShadow },
        { GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, utils::Type::SamplerCubeArrayShadow },
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? utils::Type::Undefined : it->second;
}

GLenum QtOpenGL_4_5_Renderer::Conversions::PrimitiveType2GL(utils::PrimitiveType value)
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

GLenum QtOpenGL_4_5_Renderer::Conversions::InternalFormat2GL(RenderSurface::InternalFormat value)
{
    static std::array<GLenum, RenderSurface::numElementsInternalFormat()> s_table {
        GL_NONE,
        GL_R8, GL_R8_SNORM, GL_R16, GL_R16_SNORM,
        GL_RG8, GL_RG8_SNORM, GL_RG16, GL_RG16_SNORM,
        GL_R3_G3_B2, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB8_SNORM, GL_RGB10, GL_RGB12, GL_RGB16, GL_RGB16_SNORM,
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

    return s_table[RenderSurface::castFromInternalFormat(value)];
}

core::IGraphicsRenderer::RenderSurface::InternalFormat QtOpenGL_4_5_Renderer::Conversions::GL2InternalFormat(GLenum value)
{
    static std::unordered_map<GLenum, RenderSurface::InternalFormat> s_table {
    };

    auto it = s_table.find(value);
    return (it == s_table.end()) ? RenderSurface::InternalFormat::Undefined : it->second;
}

GLenum QtOpenGL_4_5_Renderer::Conversions::NumComponents2GL(uint32_t value)
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

core::IGraphicsRenderer::RenderSurface::InternalFormat QtOpenGL_4_5_Renderer::Conversions::NumComponentsAndTypeToInternalFormat(
        uint32_t numComponents,
        utils::Type type)
{
    RenderSurface::InternalFormat result = RenderSurface::InternalFormat::Undefined;

    switch (type)
    {
    case utils::Type::Uint8:
    {
        switch (numComponents)
        {
        case 1: { result = RenderSurface::InternalFormat::R8; break; }
        case 2: { result = RenderSurface::InternalFormat::RG8; break; }
        case 3: { result = RenderSurface::InternalFormat::RGB8; break; }
        case 4: { result = RenderSurface::InternalFormat::RGBA8; break; }
        default: { break; }
        }
        break;
    }
    case utils::Type::Uint16:
    {
        switch (numComponents)
        {
        case 1: { result = RenderSurface::InternalFormat::R16; break; }
        case 2: { result = RenderSurface::InternalFormat::RG16; break; }
        case 3: { result = RenderSurface::InternalFormat::RGB16; break; }
        case 4: { result = RenderSurface::InternalFormat::RGBA16; break; }
        default: { break; }
        }
        break;
    }
    case utils::Type::Single:
    {
        switch (numComponents)
        {
        case 1: { result = RenderSurface::InternalFormat::R16F; break; }
        case 2: { result = RenderSurface::InternalFormat::RG16F; break; }
        case 3: { result = RenderSurface::InternalFormat::RGB16F; break; }
        case 4: { result = RenderSurface::InternalFormat::RGBA16F; break; }
        default: { break; }
        }
        break;
    }
    default: { break; }
    }

    return result;
}

GLenum QtOpenGL_4_5_Renderer::Conversions::Attachment2GL(FrameBuffer::Attachment value)
{
    static std::array<GLenum, FrameBuffer::numElementsAttachment()> s_table {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_DEPTH_ATTACHMENT,
        GL_STENCIL_ATTACHMENT,
        GL_DEPTH_STENCIL_ATTACHMENT
    };

    return s_table[FrameBuffer::castFromAttachment(value)];
}

GLenum QtOpenGL_4_5_Renderer::Conversions::WrapMode2GL(Texture::WrapMode value)
{
    static std::array<GLenum, Texture::numElementsWrapMode()> s_table {
        GL_CLAMP_TO_EDGE,
        GL_CLAMP_TO_BORDER,
        GL_MIRRORED_REPEAT,
        GL_REPEAT,
        GL_MIRROR_CLAMP_TO_EDGE
    };

    return s_table[Texture::castFromWrapMode(value)];
}

}
}
