#include <array>
#include <functional>

#include <utils/glm/gtc/type_ptr.hpp>
#include <utils/logger.h>
#include <utils/types.h>
#include <utils/typeinfo.h>
#include <utils/mesh.h>

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

std::vector<std::string> QtOpenGL_4_5_Renderer::RenderProgram_4_5::attributesInfo()
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

std::vector<std::string> QtOpenGL_4_5_Renderer::RenderProgram_4_5::uniformsInfo()
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

int32_t QtOpenGL_4_5_Renderer::RenderProgram_4_5::attributeLocation(const std::string& name)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    return static_cast<int32_t>(renderer->glGetAttribLocation(m_id, name.c_str()));
}

int32_t QtOpenGL_4_5_Renderer::RenderProgram_4_5::uniformLocation(const std::string& name)
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

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat2x2& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix2fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat3x3& v)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glProgramUniformMatrix3fv(m_id, loc, 1, GL_FALSE, glm::value_ptr(v));
}

void QtOpenGL_4_5_Renderer::RenderProgram_4_5::setUniform(int32_t loc, const glm::mat4x4& v)
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

size_t QtOpenGL_4_5_Renderer::Buffer_4_5::size()
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
                renderer->glMapNamedBufferRange(m_id, static_cast<GLintptr>(offset), static_cast<GLsizei>(size), MapAccess2GL(access))
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

QtOpenGL_4_5_Renderer::VertexArray_4_5::VertexArray_4_5(utils::PrimitiveType primitiveType)
    : m_indexBuffer(nullptr, 0, utils::Type::Uint32)
    , m_primitiveType(primitiveType)
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
    auto bindingIndex = static_cast<uint32_t>(-1);

    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    if (!buffer_4_5)
        return bindingIndex;

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

std::shared_ptr<core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexBufferByBindingIndex(uint32_t bindingIndex)
{
    return std::get<0>(m_vertexBuffers[bindingIndex]);
}

size_t QtOpenGL_4_5_Renderer::VertexArray_4_5::offsetByBindingIndex(uint32_t bindingIndex)
{
    return std::get<1>(m_vertexBuffers[bindingIndex]);
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::strideByBindingIndex(uint32_t bindingIndex)
{
    return std::get<2>(m_vertexBuffers[bindingIndex]);
}


void QtOpenGL_4_5_Renderer::VertexArray_4_5::attachIndexBuffer(std::shared_ptr<Buffer> buffer, uint32_t count, utils::Type type)
{
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    if (!buffer_4_5)
        return;

    m_indexBuffer = std::make_tuple(buffer_4_5, count, type);

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayElementBuffer(m_id, buffer_4_5->id());
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::detachIndexBuffer()
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayElementBuffer(m_id, 0);

    m_indexBuffer = std::make_tuple(nullptr, 0, utils::Type::Uint32);
}

std::shared_ptr<core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::VertexArray_4_5::indexBuffer()
{
    return std::get<0>(m_indexBuffer);
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::elementsCount()
{
    return std::get<1>(m_indexBuffer);
}

utils::Type QtOpenGL_4_5_Renderer::VertexArray_4_5::indicesType()
{
    return std::get<2>(m_indexBuffer);
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::declareVertexAttribute(utils::VertexAttribute attrib, uint32_t bindingIndex, uint32_t numComponents, utils::Type type, uint32_t relativeOffset)
{
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

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::bindingIndexByVertexAttribute(utils::VertexAttribute attrib)
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<0>(it->second) : static_cast<uint32_t>(-1);
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::numComponentsByVertexAttribute(utils::VertexAttribute attrib)
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<1>(it->second) : 0;
}

utils::Type QtOpenGL_4_5_Renderer::VertexArray_4_5::typeByVertexAttribute(utils::VertexAttribute attrib)
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<2>(it->second) : utils::Type::Single;
}

uint32_t QtOpenGL_4_5_Renderer::VertexArray_4_5::relativeOffsetByVertexAttribute(utils::VertexAttribute attrib)
{
    auto it = m_vertexDeclarations.find(attrib);
    return (it != m_vertexDeclarations.end()) ? std::get<3>(it->second) : 0;
}

utils::PrimitiveType QtOpenGL_4_5_Renderer::VertexArray_4_5::primitiveType()
{
    return m_primitiveType;
}

std::shared_ptr<core::IGraphicsRenderer::RenderProgram> QtOpenGL_4_5_Renderer::createRenderProgram(const std::string &vertexShader, const std::string &fragmentShader)
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

std::shared_ptr<core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::createBuffer(size_t size, const void *data)
{
    return std::make_shared<Buffer_4_5>(size, data);
}

std::shared_ptr<core::IGraphicsRenderer::VertexArray> QtOpenGL_4_5_Renderer::createVertexArray(std::shared_ptr<utils::Mesh> mesh, bool uniteBuffers)
{
    auto vertexArray = std::make_shared<VertexArray_4_5>(mesh ? mesh->primitiveType() : utils::PrimitiveType::Triangles);

    if (mesh)
    {
        if (uniteBuffers)
        {
            uint64_t totalSize = 0u;
            uint32_t stride = 0u;
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                totalSize += buffer->sizeInBytes();
                stride += buffer->numComponents() * utils::TypeInfo::size(buffer->type());
            }

            auto buffer = createBuffer(totalSize);
            auto bindingIndex = vertexArray->attachVertexBuffer(buffer, 0u, stride);

            uint32_t offset = 0u;
            auto bufferData = buffer->map(Buffer::MapAccess::OnlyWrite);
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                uint32_t vertexSize = buffer->numComponents() * utils::TypeInfo::size(buffer->type());
                for (uint32_t i = 0; i < buffer->numVertices(); ++i)
                    std::memcpy(static_cast<uint8_t*>(bufferData->get()) + stride * i + offset, buffer->vertex(i), vertexSize);
                vertexArray->declareVertexAttribute(attrib, bindingIndex, buffer->numComponents(), buffer->type(), offset);
                offset += vertexSize;
            }
        }
        else
        {
            for (auto const &[attrib, buffer] : mesh->vertexBuffers())
            {
                auto bindingIndex = vertexArray->attachVertexBuffer(createBuffer(buffer->sizeInBytes(), buffer->data()),
                                                                    0u,
                                                                    buffer->numComponents() * utils::TypeInfo::size(buffer->type()));
                vertexArray->declareVertexAttribute(attrib, bindingIndex, buffer->numComponents(), buffer->type(), 0u);
            }
        }

        if (auto buffer = mesh->indexBuffer(); buffer)
        {
            vertexArray->attachIndexBuffer(createBuffer(buffer->sizeInBytes(), buffer->data()),
                                           buffer->numIndices(),
                                           buffer->type());
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

void QtOpenGL_4_5_Renderer::resize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void QtOpenGL_4_5_Renderer::render()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(.5f, .5f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void QtOpenGL_4_5_Renderer::render2(std::shared_ptr<RenderProgram> rp, std::shared_ptr<VertexArray> vao)
{
    auto rp2 = std::dynamic_pointer_cast<RenderProgram_4_5>(rp);
    auto vao2 = std::dynamic_pointer_cast<VertexArray_4_5>(vao);

    glUseProgram(rp2->id());
    glBindVertexArray(vao2->id());
    glDrawElements(GL_TRIANGLES, vao2->elementsCount(), Type2GL(vao2->indicesType()), nullptr);
}

QtOpenGL_4_5_Renderer::QtOpenGL_4_5_Renderer(const QOpenGLContext *context)
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

QtOpenGL_4_5_Renderer::Buffer_4_5::MappedData_4_5::MappedData_4_5(std::shared_ptr<Buffer_4_5> mappedBuffer, void *data)
    : m_mappedBuffer(mappedBuffer)
    , m_data(data)
{
}

QtOpenGL_4_5_Renderer::Buffer_4_5::MappedData_4_5::~MappedData_4_5()
{
    if (!m_mappedBuffer.expired())
    {
        auto buffer = m_mappedBuffer.lock();
        auto instance = QtOpenGL_4_5_Renderer::instance();
        instance->glUnmapNamedBuffer(buffer->m_id);
        buffer->m_isMapped = false;
    }
}

void *QtOpenGL_4_5_Renderer::Buffer_4_5::MappedData_4_5::get()
{
    return m_mappedBuffer.expired() ? nullptr : m_data;
}

}
}
