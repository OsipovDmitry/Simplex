#include <QApplication>
#include <QWidget>

#include <logger/logger.h>
#include <renderer/context.h>

#include "gleswidget.h"

char vShaderStr[] =
   "#version 300 es                          \n"
   "layout(location = 0) in vec4 vPosition;  \n"
   "void main()                              \n"
   "{                                        \n"
   "   gl_Position = vPosition;              \n"
   "}                                        \n";
char fShaderStr[] =
   "#version 300 es                              \n"
   "precision mediump float;                     \n"
   "uniform vec4 color;                          \n"
   "out vec4 fragColor;                          \n"
   "void main()                                  \n"
   "{                                            \n"
   "   fragColor = color;  \n"
   "}                                            \n";


int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	GLESWidget w1;
	w1.setWindowTitle("Window 1");
	auto c1 = renderer::Context::createContext(w1.winId());

	std::string sLog;
	auto vertexShader = c1->createShader(renderer::ShaderType::Vertex);
	vertexShader->setSourceCode(std::string(vShaderStr));
	vertexShader->compile(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	auto fragShader = c1->createShader(renderer::ShaderType::Fragment);
	fragShader->setSourceCode(std::string(fShaderStr));
	fragShader->compile(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	auto program = c1->createProgram();
	program->attachShader(vertexShader);
	program->attachShader(fragShader);
	program->link(&sLog);
	if (!sLog.empty())
		LOG_DEBUG(sLog);

	program->setUniform(program->uniformLocation("color"), glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));

	for (int i = 0; i < program->activeUniformsCount(); ++i)
		program->activeUniformInfo(i);

	w1.setContext(c1);
	w1.setProgram(program);
	w1.show();

//	GLESWidget w2;
//	w2.setWindowTitle("Window 2");
//	auto c2 = renderer::Context::createContext(w2.winId(), 8,8,8,0,24,8, c1);
//	w2.setContext(c2);
//	w2.setProgram(program);
//	w2.show();

	return app.exec();
}
