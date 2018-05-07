#include <QDebug>
#include <QPaintEvent>

#include <glm/gtc/type_ptr.hpp>

#include <GLES3/gl3.h>
#include "gleswidget.h"

#include <logger/logger.h>
#include <types/image.h>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/shader.h>
#include <renderer/vertexarray.h>
#include <renderer/buffer.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>
#include <renderer/framebuffer.h>

namespace {

char vShaderStr[] =
		"#version 300 es                          \n"
		"layout(location = 0) in vec2 vPosition;  \n"
		"layout(location = 1) in vec3 vColor;     \n"
		"layout(location = 2) in vec2 vTexCoord;  \n"
		"out vec3 color;                          \n"
		"out vec2 texCoord;                       \n"
		"void main()                              \n"
		"{                                        \n"
		"   gl_Position = vec4(vPosition, 0, 1);  \n"
		"   color = vColor;                       \n"
		"   texCoord = vTexCoord;                 \n"
		"}                                        \n";

char fShaderStr[] =
		"#version 300 es                              \n"
		"precision mediump float;                     \n"
		"uniform sampler2D tex;                       \n"
		"in vec3 color;                               \n"
		"in vec2 texCoord;                            \n"
		"out vec4 fragColor;                          \n"
		"layout(shared) uniform UniformBlock {        \n"
		"	vec3 globalColor;                         \n"
		"};                                           \n"
		"void main()                                  \n"
		"{                                            \n"
		"   fragColor = texture(tex, texCoord);       \n"
		"}                                            \n";

const float vertices[] = { -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
						   -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
							0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
							0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
					  };
const uint32_t indices[] = {0, 1, 2, 2, 3, 0};

}

GLESWidget::GLESWidget(QWidget *parent, renderer::ContextPtr sharedContext) : QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);

	m_context = renderer::Context::createContext(winId(), 8, 8, 8, 8, 24, 8, sharedContext);

	std::string sLog;
	auto vertexShader = m_context->createShader(renderer::ShaderType::Vertex);
	vertexShader->setSourceCode(std::string(vShaderStr));
	auto b = vertexShader->compile(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	auto fragShader = m_context->createShader(renderer::ShaderType::Fragment);
	fragShader->setSourceCode(std::string(fShaderStr));
	b = fragShader->compile(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	m_program = m_context->createProgram();
	m_program->attachShader(vertexShader);
	m_program->attachShader(fragShader);
	b = m_program->link(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	m_program->setUnifromBlockBindingPoint(m_program->uniformBlockIndexByName("UniformBlock"), 1);
	m_program->setUniform(m_program->uniformLocationByName("tex"), (int32_t)0);

	glm::vec3 globalColor(0.6,0.5,0.3);
	m_uniformBuffer = m_context->createBuffer(renderer::BufferUsage::StaticDraw, sizeof(glm::vec3), glm::value_ptr(globalColor));
	m_vertexArray = m_context->createVertexArray();
	m_vertexArray->bindVertexBuffer(0,
									m_context->createBuffer(renderer::BufferUsage::StaticDraw, sizeof(vertices), vertices),
									2, renderer::VertexArrayAttributePointerType::Type_32f, false, 7*sizeof(float), 0*sizeof(float));
	m_vertexArray->bindVertexBuffer(1,
									m_vertexArray->vertexBuffer(0),
									3, renderer::VertexArrayAttributePointerType::Type_32f, false, 7*sizeof(float), 2*sizeof(float));
	m_vertexArray->bindVertexBuffer(2,
									m_vertexArray->vertexBuffer(0),
									2, renderer::VertexArrayAttributePointerType::Type_32f, false, 7*sizeof(float), 5*sizeof(float));
	m_vertexArray->bindIndexBuffer(m_context->createBuffer(renderer::BufferUsage::StaticDraw, sizeof(indices), indices));

	auto img = QImage(":/res/side1.png").convertToFormat(QImage::Format_RGB888);
	m_texture = m_context->createTexture(renderer::TextureType::Type_2D, renderer::TextureInternalFormat::RGB8uiNorm, renderer::TextureSize(img.width(), img.height()));
	m_texture->setSubImage(types::ImageFormat::RGB, types::ImageType::Typy_8ui, img.bits());

	m_context->bindUniformBuffer(m_uniformBuffer, 1);
	m_context->bindTexture(m_texture, 0);

	m_framebufferTexture = m_context->createTexture(renderer::TextureType::Type_2D, renderer::TextureInternalFormat::RGBA8uiNorm, renderer::TextureSize(512, 512), 1);

	m_framebuffer = m_context->createFramebuffer();
	m_framebuffer->setViewport(0, 0, 512, 512);
	m_framebuffer->attachBuffer(renderer::FramebufferAttachment::Color0, m_framebufferTexture);
}

void GLESWidget::paintEvent(QPaintEvent *pEvent)
{
	m_framebuffer->clearColorBuffer(0, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	m_framebuffer->renderIndexedGeometry(m_program, m_vertexArray, renderer::PrimitiveType::Triangles, sizeof(indices)/sizeof(indices[0]), renderer::GeometryIndexType::Type_32ui);

	auto pMainFramebuffer = m_context->mainFramebuffer();
	pMainFramebuffer->setViewport(0, 0, width(), height());
	pMainFramebuffer->clearColorBuffer(0, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
	pMainFramebuffer->copyColorBuffer(glm::uvec4(10, 10, width()-10, height()-10), glm::uvec4(0, 0, 512, 512), m_framebuffer);

	m_context->windowSurface()->swapBuffers();
}
