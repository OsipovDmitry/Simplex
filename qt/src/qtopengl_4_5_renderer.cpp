#include <array>
#include <functional>

#include <ostream>
#include <utils/logger.h>
#include <utils/types.h>
#include <utils/typeinfo.h>

#include "qtopengl_4_5_renderer.h"

namespace simplex
{
namespace qt
{

std::weak_ptr<QtOpenGL_4_5_Renderer> QtOpenGL_4_5_Renderer::s_instance;

QtOpenGL_4_5_Renderer::RenderProgram_4_5::RenderProgram_4_5(GLuint id)
    : m_id(id)
{
}

QtOpenGL_4_5_Renderer::RenderProgram_4_5::~RenderProgram_4_5()
{

}

GLuint QtOpenGL_4_5_Renderer::RenderProgram_4_5::id() const
{
    return m_id;
}

QtOpenGL_4_5_Renderer::Buffer_4_5::Buffer_4_5(uint64_t size, const void *data)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGenBuffers(1, &m_id);
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

QtOpenGL_4_5_Renderer::VertexArray_4_5::VertexArray_4_5()
    : m_drawDataDescription(utils::PrimitiveType::Triangles, 0u, nullptr, utils::Type::Uint32)
{
    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glGenVertexArrays(1, &m_id);
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

void QtOpenGL_4_5_Renderer::VertexArray_4_5::attachVertexBuffer(utils::VertexAttribute attribIndex,
                                                                std::shared_ptr<Buffer> buffer,
                                                                uint8_t numComponents,
                                                                utils::Type type,
                                                                size_t offset,
                                                                size_t stride,
                                                                uint32_t relativeOffset)
{
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    if (!buffer_4_5)
        return;

    auto loc = static_cast<GLuint>(utils::castFromVertexAttribute(attribIndex));

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayVertexBuffer(m_id, loc, buffer_4_5->id(), static_cast<GLintptr>(offset), static_cast<GLsizei>(stride));
    renderer->glVertexArrayAttribBinding(m_id, loc, loc);
    renderer->glEnableVertexArrayAttrib(m_id, loc);

    if (type == utils::Type::Single)
        renderer->glVertexArrayAttribFormat(m_id, loc, static_cast<GLint>(numComponents), Type2GL(type), GL_FALSE, static_cast<GLuint>(relativeOffset));
    else if (type == utils::Type::Double)
        renderer->glVertexArrayAttribLFormat(m_id, loc, static_cast<GLint>(numComponents), Type2GL(type), static_cast<GLuint>(relativeOffset));
    else if (utils::TypeInfo::isInt(type))
        renderer->glVertexArrayAttribIFormat(m_id, loc, static_cast<GLint>(numComponents), Type2GL(type), static_cast<GLuint>(relativeOffset));

    m_vertexBuffers[attribIndex] = buffer_4_5;
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::detachVertexBuffer(utils::VertexAttribute attribIndex)
{
    auto loc = static_cast<GLuint>(utils::castFromVertexAttribute(attribIndex));

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayVertexBuffer(m_id, loc, 0u, static_cast<GLintptr>(0), static_cast<GLsizei>(0));
    renderer->glDisableVertexArrayAttrib(m_id, loc);

    m_vertexBuffers.erase(attribIndex);
}

std::shared_ptr<core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::VertexArray_4_5::vertexBuffer(utils::VertexAttribute attribIndex) const
{
    auto it = m_vertexBuffers.find(attribIndex);
    return (it != m_vertexBuffers.end()) ? it->second : nullptr;
}

void QtOpenGL_4_5_Renderer::VertexArray_4_5::setDrawDataDescription(utils::PrimitiveType primitiveType,
                                                                    size_t elementsCount,
                                                                    std::shared_ptr<Buffer> buffer,
                                                                    utils::Type type)
{
    auto buffer_4_5 = std::dynamic_pointer_cast<Buffer_4_5>(buffer);
    if (!buffer_4_5)
        return;

    auto renderer = QtOpenGL_4_5_Renderer::instance();
    renderer->glVertexArrayElementBuffer(m_id, buffer_4_5->id());

    m_drawDataDescription = { primitiveType, elementsCount, buffer_4_5, type };
}

utils::PrimitiveType QtOpenGL_4_5_Renderer::VertexArray_4_5::primitiveType() const
{
    return std::get<0>(m_drawDataDescription);
}

size_t QtOpenGL_4_5_Renderer::VertexArray_4_5::elementsCount() const
{
    return std::get<1>(m_drawDataDescription);
}

std::shared_ptr<core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::VertexArray_4_5::indexBuffer() const
{
    return std::get<2>(m_drawDataDescription);
}

utils::Type QtOpenGL_4_5_Renderer::VertexArray_4_5::indicesType() const
{
    return std::get<3>(m_drawDataDescription);
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
    GLuint programId = 0u;
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
        programId = renderer->glCreateProgram();
        for (size_t i = 0; i < s_shadersCount; ++i)
            renderer->glAttachShader(programId, shadersIds[i]);
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
            isOk= false;
        }
    }

    if (!isOk)
    {
        renderer->glDeleteShader(programId);
        for (size_t i = 0; i < s_shadersCount; ++i)
            renderer->glDeleteShader(shadersIds[i]);
        return nullptr;
    }

    return isOk ? std::make_shared<RenderProgram_4_5>(programId) : nullptr;
}

std::shared_ptr<core::IGraphicsRenderer::Buffer> QtOpenGL_4_5_Renderer::createBuffer(size_t size, const void *data)
{
    return std::make_shared<Buffer_4_5>(size, data);
}

std::shared_ptr<core::IGraphicsRenderer::VertexArray> QtOpenGL_4_5_Renderer::createVertexArray()
{
    return std::make_shared<VertexArray_4_5>();
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
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
}

void QtOpenGL_4_5_Renderer::render()
{
    glClearColor(.5f, .5f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
//    glBegin(GL_TRIANGLES);
//    glColor3f(1.f, 0.f, 0.f);
//    glVertex2f(-.5f, -.7f);
//    glColor3f(0.f, 1.f, 0.f);
//    glVertex2f(.5f, -.7f);
//    glColor3f(0.f, 0.f, 1.f);
//    glVertex2f(0.f, .5f);
//    glEnd();
}

void QtOpenGL_4_5_Renderer::render2(std::shared_ptr<RenderProgram> r, std::shared_ptr<VertexArray> v)
{
    auto rr = std::dynamic_pointer_cast<RenderProgram_4_5>(r);
    auto vv = std::dynamic_pointer_cast<VertexArray_4_5>(v);

    glBindVertexArray(vv->id());
    glUseProgram(rr->id());
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
        GL_UNSIGNED_INT
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
    return m_data;
}

}
}
