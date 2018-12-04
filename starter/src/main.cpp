#include <QApplication>
#include "gleswidget.h"

#include <logger/logger.h>
#include <renderer/context.h>
#include <renderer/shader.h>
#include <renderer/program.h>

char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec3 vPosition;  \n"
        "layout(location = 1) in vec3 vColor;     \n"
        "layout(location = 2) in vec2 vTexCoord;  \n"
        "out vec3 color;                          \n"
        "out vec2 texCoord;                       \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vec4(vPosition, 1);  \n"
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
        "   fragColor = vec4(globalColor, 1.0);       \n"
        "}                                            \n";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    GLESWidget w1;
    w1.setWindowTitle("Window 1");

    GLESWidget w2(nullptr, w1.context()->shareGroup());
    w2.setWindowTitle("Window 2");

    GLESWidget w3(nullptr, w2.context()->shareGroup());
    w3.setWindowTitle("Window 3");

    std::string sLog;
    auto vertexShader = w1.context()->createShader(renderer::ShaderType::Vertex);
    vertexShader->setSourceCode(std::string(vShaderStr));
    auto b = vertexShader->compile(&sLog);
    if (!sLog.empty())
        LOG_DEBUG(sLog);

    auto fragShader = w1.context()->createShader(renderer::ShaderType::Fragment);
    fragShader->setSourceCode(std::string(fShaderStr));
    b = fragShader->compile(&sLog);
    if (!sLog.empty())
        LOG_DEBUG(sLog);

    auto program = w1.context()->createProgram();
    program->attachShader(vertexShader);
    program->attachShader(fragShader);
    b = program->link(&sLog);
    if (!sLog.empty())
        LOG_DEBUG(sLog);

    program->setUnifromBlockBindingPoint(program->uniformBlockIndexByName("UniformBlock"), 0);
    program->setUniform(program->uniformLocationByName("tex"), (int32_t)0);

    w1.setProgram(program);
    w2.setProgram(program);
    w3.setProgram(program);

    w1.show();
    w2.show();
    w3.show();

    return app.exec();
}
