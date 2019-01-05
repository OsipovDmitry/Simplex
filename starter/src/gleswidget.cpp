#include <QDebug>
#include <QPaintEvent>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "gleswidget.h"

#include <logger/logger.h>
#include <types/image.h>
#include <types/geometry.h>
#include <renderer/display.h>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/shader.h>
#include <renderer/vertexarray.h>
#include <renderer/buffer.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>
#include <renderer/framebuffer.h>

GLESWidget::GLESWidget(QWidget *parent, renderer::ContextGroupPtr shareGroup) : QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);

    m_context = renderer::Context::createContext(winId(), 8, 8, 8, 8, 24, 8, shareGroup);

	glm::vec3 globalColor(0.6,0.5,0.3);
    m_uniformBuffer = renderer::Buffer::create(m_context, renderer::BufferUsage::StaticDraw, sizeof(glm::vec3), glm::value_ptr(globalColor));

	auto img = QImage(":/res/side1.png").convertToFormat(QImage::Format_RGB888);
    m_texture = renderer::Texture::create(m_context, renderer::TextureType::Type_2D, renderer::TextureInternalFormat::RGB8uiNorm, renderer::TextureSize(img.width(), img.height()));
	m_texture->setSubImage(types::ImageFormat::RGB, types::ImageType::Typy_8ui, img.bits());

    m_context->bindUniformBuffer(m_uniformBuffer, 0);
	m_context->bindTexture(m_texture, 0);

    m_framebufferTexture = renderer::Texture::create(m_context, renderer::TextureType::Type_2D, renderer::TextureInternalFormat::RGBA8uiNorm, renderer::TextureSize(512, 512), 1);

    m_framebuffer = renderer::Framebuffer::create(m_context);
	m_framebuffer->setViewport(0, 0, 512, 512);
    m_framebuffer->attachBuffer(renderer::FramebufferAttachment::Color0, m_framebufferTexture);
}

renderer::ContextPtr GLESWidget::context() const
{
    return m_context;
}

void GLESWidget::setContext(renderer::ContextPtr context)
{
    m_context = context;
}

void GLESWidget::setProgram(renderer::ProgramPtr program)
{
    m_program = program;
}

void GLESWidget::setVAO(renderer::VertexArrayPtr vao, uint32_t numIndices)
{
    m_vertexArray = vao;
    m_numIndices = numIndices;
}

void GLESWidget::setTransform(const glm::mat4x4 mat)
{
    m_transform = mat;
}

void GLESWidget::paintEvent(QPaintEvent *pEvent)
{
    m_context->enableDepthTest();

    auto pMainFramebuffer = renderer::Framebuffer::mainFramebuffer(m_context);
    pMainFramebuffer->setViewport(0, 0, width(), height());

    m_program->setUniform(m_program->uniformLocationByName("uTransform"),
                          glm::perspective(0.25f * glm::pi<float>(), (float)width()/(float)height(), 0.01f, 500.0f) *
                          m_transform);

    pMainFramebuffer->clearColorBuffer(0, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
    pMainFramebuffer->clearDepthBuffer(1.0f);
    pMainFramebuffer->renderIndexedGeometry(m_program, m_vertexArray, types::PrimitiveType::Triangles, m_numIndices, types::IndexType::Uint32);
//
//	pMainFramebuffer->setViewport(0, 0, width(), height());
//	pMainFramebuffer->clearColorBuffer(0, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
//	pMainFramebuffer->copyColorBuffer(glm::uvec4(10, 10, width()-10, height()-10), glm::uvec4(0, 0, 512, 512), m_framebuffer);

	m_context->windowSurface()->swapBuffers();
}
