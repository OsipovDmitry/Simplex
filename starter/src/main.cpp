#include <QApplication>
#include <QWidget>

#include <logger/logger.h>
#include <types/image.h>

#include <glm/gtc/type_ptr.hpp>

#include "gleswidget.h"

char vShaderStr[] =
   "#version 300 es                          \n"
   "layout(location = 0) in vec2 vPosition;  \n"
   "layout(location = 1) in vec3 vColor;     \n"
   "layout(location = 2) in vec2 vTexCoord;  \n"
   "out vec3 color;                          \n"
   "out vec2 texCoord;                        \n"
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
   "	vec3 globalColor;                        \n"
   "};                                           \n"

   "void main()                                  \n"
   "{                                            \n"
   "   fragColor = texture(tex, texCoord);   \n"
   "}                                            \n";

const unsigned char texture[] = {
	255,0,0,     0,255,0,
	0,0,255,     255,255,0,

};

const float vertices[] = {  0.0f,  0.5f,  1.0f, 0.0f, 0.0f,  0.5f, 1.0f,
						   -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
							0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f
					  };
const uint32_t indices[] = {0, 1, 2};


int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	GLESWidget w1, w2;

	w1.setWindowTitle("Window 1");
	auto c1 = renderer::Context::createContext(w1.winId());

	std::string sLog;
	auto vertexShader = c1->createShader(renderer::ShaderType::Vertex);
	vertexShader->setSourceCode(std::string(vShaderStr));
	auto b = vertexShader->compile(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	auto fragShader = c1->createShader(renderer::ShaderType::Fragment);
	fragShader->setSourceCode(std::string(fShaderStr));
	b = fragShader->compile(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	auto program1 = c1->createProgram();
	program1->attachShader(vertexShader);
	program1->attachShader(fragShader);
	b = program1->link(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	program1->setUnifromBlockBindingPoint(program1->uniformBlockIndexByName("UniformBlock"), 1);
	program1->setUniform(program1->uniformLocationByName("tex"), (int32_t)0);

	glm::vec3 globalColor(0.6,0.5,0.3);
	auto pUniformBuffer1 = c1->createBuffer(renderer::BufferUsage::StaticDraw, sizeof(glm::vec3), glm::value_ptr(globalColor));
	auto pVertexArray1 = c1->createVertexArray();
	pVertexArray1->bindVertexBuffer(0,
									c1->createBuffer(renderer::BufferUsage::StaticDraw, sizeof(vertices), vertices),
									2, renderer::VertexArrayAttributePointerType::Type_32f, false, 7*sizeof(float), 0*sizeof(float));
	pVertexArray1->bindVertexBuffer(1,
									pVertexArray1->vertexBuffer(0),
									3, renderer::VertexArrayAttributePointerType::Type_32f, false, 7*sizeof(float), 2*sizeof(float));
	pVertexArray1->bindVertexBuffer(2,
									pVertexArray1->vertexBuffer(0),
									2, renderer::VertexArrayAttributePointerType::Type_32f, false, 7*sizeof(float), 5*sizeof(float));
	pVertexArray1->bindIndexBuffer(c1->createBuffer(renderer::BufferUsage::StaticDraw, sizeof(indices), indices));

	auto pTexture1 = c1->createTexture(renderer::TextureType::Type_2D, renderer::TextureInternalFormat::RGB8uiNorm, renderer::TextureSize(2, 2), 1);
	pTexture1->setSubImage(types::ImageFormat::RGB, types::ImageType::Typy_8ui, texture);

	c1->bindUniformBuffer(pUniformBuffer1, 1);
	c1->bindTexture(pTexture1, 0);

	w1.setContext(c1);
	w1.setProgram(program1);
	w1.setVertexArray(pVertexArray1);
	w1.show();

//	w2.setWindowTitle("Window 2");
//	auto c2 = renderer::Context::createContext(w2.winId(), 8,8,8,0,24,8, c1);

//	auto pUniformBuffer2 = c2->createSharedBuffer(pUniformBuffer1);
//	auto program2 = c2->createSharedProgram(program1);
//	auto pVertexArray2 = c2->createSharedVertexArray(pVertexArray1);

//	c2->bindUniformBuffer(pUniformBuffer2, 1);
//	w2.setContext(c2);
//	w2.setProgram(program2);
//	w2.setVertexArray(pVertexArray2);
//	w2.show();

	vertexShader.reset();
	fragShader.reset();
	program1.reset();
	pVertexArray1.reset();
	c1.reset();

//	program2.reset();
//	pVertexArray2.reset();
//	c2.reset();

	return app.exec();
}
