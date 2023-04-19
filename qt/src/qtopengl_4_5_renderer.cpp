#include <array>
#include <functional>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/logger.h>
#include <utils/types.h>
#include <utils/typeinfo.h>
#include <utils/mesh.h>

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

std::vector<std::string> QtOpenGL_4_5_Renderer::RenderProgram_4_5::attributesInfo() const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLint numAttributes;
    renderer->glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    std::vector<std::string> result;
    result.reserve(static_cast<size_t>(numAttributes));

    GLint nameSize;
    renderer->glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nameSize);
    std::vector<GLchar> name(static_cast<size_t>(nameSize));

    GLsizei length;
    GLint size;
    GLenum type;
    for (GLint i = 0; i < numAttributes; ++i)
    {
        renderer->glGetActiveAttrib(m_id,
                                    static_cast<GLuint>(i),
                                    static_cast<GLsizei>(nameSize),
                                    &length,
                                    &size,
                                    &type,
                                    name.data());
        result.emplace_back(name.data());
    }

    return result;
}

std::vector<std::string> QtOpenGL_4_5_Renderer::RenderProgram_4_5::uniformsInfo() const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();

    GLint numAttributes;
    renderer->glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &numAttributes);
    std::vector<std::string> result;
    result.reserve(static_cast<size_t>(numAttributes));

    GLint nameSize;
    renderer->glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nameSize);
    std::vector<GLchar> name(static_cast<size_t>(nameSize));

    GLsizei length;
    GLint size;
    GLenum type;
    for (GLint i = 0; i < numAttributes; ++i)
    {
        renderer->glGetActiveUniform(m_id,
                                     static_cast<GLuint>(i),
                                     static_cast<GLsizei>(nameSize),
                                     &length,
                                     &size,
                                     &type,
                                     name.data());
        result.emplace_back(name.data());
    }

    return result;
}

int32_t QtOpenGL_4_5_Renderer::RenderProgram_4_5::attributeLocation(const std::string& name) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return static_cast<int32_t>(renderer->glGetAttribLocation(m_id, name.c_str()));
}

int32_t QtOpenGL_4_5_Renderer::RenderProgram_4_5::uniformLocation(const std::string& name) const
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return static_cast<int32_t>(renderer->glGetUniformLocation(m_id, name.c_str()));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, float v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform1fv(m_id, loc, 1, &v);
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::vec2& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform2fv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::vec3& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform3fv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::vec4& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform4fv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, int32_t v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform1iv(m_id, loc, 1, &v);
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::ivec2& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform2iv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::ivec3& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform3iv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::ivec4& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform4iv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, uint32_t v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform1uiv(m_id, loc, 1, &v);
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::uvec2& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform2uiv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::uvec3& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform3uiv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::uvec4& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniform4uiv(m_id, loc, 1, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat2& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix2fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat3& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix3fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat4& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix4fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
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
    assert(utils::TypeInfo::isDefined(type));

    m_vertexDeclarations[attrib] = std::make_tuple(bindingIndex, numComponents, type, relativeOffset);

    auto loc = static_cast<GLuint>(attrib);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayAttribBinding(m_id, loc, static_cast<GLuint>(bindingIndex));
    renderer->glEnableVertexArrayAttrib(m_id, loc);

    if (type == utils::Type::Single)
        renderer->glVertexArrayAttribFormat(m_id, loc, static_cast<GLint>(numComponents), Type2GL(type), GL_FALSE, static_cast<GLuint>(relativeOffset));
    else if (type == utils::Type::Double)
        renderer->glVertexArrayAttribLFormat(m_id, loc, static_cast<GLint>(numComponents), Type2GL(type), static_cast<GLuint>(relativeOffset));
    else if (utils::TypeInfo::isInt(type))
        renderer->glVertexArrayAttribIFormat(m_id, loc, static_cast<GLint>(numComponents), Type2GL(type), static_cast<GLuint>(relativeOffset));
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
        const auto& shader = shadersData[i];
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
            isOk= false;
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

QtOpenGL_4_5_Renderer::~QtOpenGL_4_5_Renderer()
{
    LOG_INFO << "GraphicsRenderer \"" << QtOpenGL_4_5_Renderer::name() << "\" has been destroyed";
}

std::shared_ptr<QtOpenGL_4_5_Renderer> QtOpenGL_4_5_Renderer::instance()
{
    return s_instance.expired() ? nullptr : s_instance.lock();
}

const std::string &QtOpenGL_4_5_Renderer::name() const
{
    static const std::string s_name = "QtOpenGL_1_0_Renderer";
    return s_name;
}

void QtOpenGL_4_5_Renderer::resize(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
}

uint32_t QtOpenGL_4_5_Renderer::width() const
{
    return m_width;
}

uint32_t QtOpenGL_4_5_Renderer::height() const
{
    return m_height;
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
    glViewport(0, 0, static_cast<GLsizei>(renderInfo.viewport().x), static_cast<GLsizei>(renderInfo.viewport().y));
    glEnable(GL_DEPTH_TEST);
    glClearColor(.5f, .5f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &renderData : m_renderData)
    {
        auto rp2 = std::dynamic_pointer_cast<RenderProgram_4_5>(renderData.second->renderProgram());
        auto vao2 = std::dynamic_pointer_cast<VertexArray_4_5>(renderData.second->vertexArray());

        rp2->setUniform(rp2->uniformLocation("u_modelMatrix"), renderData.first);
        rp2->setUniform(rp2->uniformLocation("u_viewMatrix"), renderInfo.viewMatrix());
        rp2->setUniform(rp2->uniformLocation("u_projectionMatrix"), renderInfo.projectionMatrix());

        glUseProgram(rp2->id());
        glBindVertexArray(vao2->id());

        for (auto &primitiveSet : vao2->primitiveSets())
        {
            if (auto drawArrays = primitiveSet->asDrawArrays(); drawArrays)
                glDrawArrays(PrimitiveType2GL(drawArrays->primitiveType()),
                             static_cast<GLint>(drawArrays->first()),
                             static_cast<GLint>(drawArrays->count()));
            else if (auto drawElements = primitiveSet->asDrawElements(); drawElements)
                glDrawElementsBaseVertex(PrimitiveType2GL(drawElements->primitiveType()),
                                         static_cast<GLsizei>(drawElements->count()),
                                         Type2GL(drawElements->type()),
                                         reinterpret_cast<const void*>(drawElements->offset()),
                                         static_cast<GLint>(drawElements->baseVertex()));
        }
    }
}

QtOpenGL_4_5_Renderer::QtOpenGL_4_5_Renderer(const QOpenGLContext *context)
    : m_width(0u)
    , m_height(0u)
{
    if (!s_instance.expired())
    {
        LOG_CRITICAL << "QtOpenGL_4_5_Renderer instance has already been created";
        assert(false);
    }

    setOwningContext(context);
    if (!initializeOpenGLFunctions())
    {
        LOG_CRITICAL << "Can't initialize QOpenGLFunctionsW";
        assert(false);
    }

    LOG_INFO << "GraphicsRenderer \"" << QtOpenGL_4_5_Renderer::name() << "\" has been created";
}

void QtOpenGL_4_5_Renderer::setInstance(std::shared_ptr<QtOpenGL_4_5_Renderer> instance)
{
    s_instance = instance;
}

GLenum QtOpenGL_4_5_Renderer::Type2GL(utils::Type value)
{
    static std::array<GLbitfield, utils::numElementsType()> s_table {
        GL_NONE,
        GL_FLOAT,
        GL_DOUBLE,
        GL_BYTE,
        GL_SHORT,
        GL_INT,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT,
        GL_UNSIGNED_INT,
    };

    return s_table[utils::castFromType(value)];
}

GLenum QtOpenGL_4_5_Renderer::PrimitiveType2GL(utils::PrimitiveType value)
{
    static std::array<GLbitfield, utils::numElementsPrimitiveType()> s_table {
        GL_POINTS,
        GL_LINES,
        GL_LINE_STRIP,
        GL_TRIANGLES,
        GL_TRIANGLE_STRIP,
        GL_TRIANGLE_FAN,
    };

    return s_table[utils::castFromPrimitiveType(value)];
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

}
}
