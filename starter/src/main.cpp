#include <QApplication>
#include <QFile>
#include <QGridLayout>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/gtc/matrix_transform.hpp>

#include <logger/logger.h>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/shader.h>
#include <renderer/vertexarray.h>
#include <renderer/buffer.h>
#include <renderer/framebuffer.h>

#include "gleswidget.h"

char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec3 vPosition;  \n"
        "layout(location = 1) in vec3 vColor;     \n"
        "layout(location = 2) in vec2 vTexCoord;  \n"
        "uniform mat4 uTransform;  \n"
        "out vec3 color;                          \n"
        "out vec2 texCoord;                       \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = uTransform * vec4(vPosition, 1.0);  \n"
        "   color = 0.5 * vColor + vec3(0.5);                       \n"
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
        "   fragColor = vec4(color, 1.0);       \n"
        "}                                            \n";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    GLESWidget w1;
    w1.setWindowTitle("Window 1");
    w1.setTransform(
                glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.0f)) *
                glm::rotate(glm::mat4x4(), -0.5f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

    GLESWidget w2(nullptr, w1.context()->shareGroup());
    w2.setWindowTitle("Window 2");
    w2.setTransform(glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.0f)));

    GLESWidget w3(nullptr, w2.context()->shareGroup());
    w3.setWindowTitle("Window 3");
    w3.setTransform(
                glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.0f)) *
                glm::rotate(glm::mat4x4(), 0.5f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(glm::mat4x4(), -0.5f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

    GLESWidget w4(nullptr, w3.context()->shareGroup());
    w4.setWindowTitle("Window 4");
    w4.setTransform(
                glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.5f)) *
                glm::rotate(glm::mat4x4(), -0.25f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(glm::mat4x4(), -0.25f * glm::pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

    std::string sLog;
    auto vertexShader = renderer::Shader::create(w1.context(), renderer::ShaderType::Vertex);
    vertexShader->setSourceCode(std::string(vShaderStr));
    auto b = vertexShader->compile(&sLog);
    if (!sLog.empty())
        LOG_DEBUG(sLog);

    auto fragShader = renderer::Shader::create(w1.context(), renderer::ShaderType::Fragment);
    fragShader->setSourceCode(std::string(fShaderStr));
    b = fragShader->compile(&sLog);
    if (!sLog.empty())
        LOG_DEBUG(sLog);

    auto program = renderer::Program::create(w1.context());
    program->attachShader(vertexShader);
    program->attachShader(fragShader);
    b = program->link(&sLog);
    if (!sLog.empty())
        LOG_DEBUG(sLog);

    program->setUnifromBlockBindingPoint(program->uniformBlockIndexByName("UniformBlock"), 0);
    program->setUniform(program->uniformLocationByName("tex"), (int32_t)0);

    QFile modelFile(":/res/monkey.fbx");
    modelFile.open(QFile::ReadOnly);
    auto modelData = modelFile.readAll();

    Assimp::Importer Importer;
    auto pScene = Importer.ReadFileFromMemory(modelData.data(), modelData.size(), aiProcess_Triangulate | aiProcess_GenNormals);
    auto pMesh = pScene->mMeshes[0];

    float *vertices = new float[pMesh->mNumVertices * 6];
    unsigned int numVertices = pMesh->mNumVertices;
    for (int32_t i = 0; i < pMesh->mNumVertices; ++i) {
        vertices[6*i + 0] = pMesh->mVertices[i].x;
        vertices[6*i + 1] = pMesh->mVertices[i].y;
        vertices[6*i + 2] = pMesh->mVertices[i].z;
        vertices[6*i + 3] = pMesh->mNormals[i].x;
        vertices[6*i + 4] = pMesh->mNormals[i].y;
        vertices[6*i + 5] = pMesh->mNormals[i].z;
    }

    unsigned int numIndices = pMesh->mNumFaces * 3;
    unsigned int *indices = new unsigned int[numIndices];
    for (int32_t i = 0 ; i < pMesh->mNumFaces ; i++) {
        const aiFace& Face = pMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        indices[3*i + 0] = Face.mIndices[0];
        indices[3*i + 1] = Face.mIndices[1];
        indices[3*i + 2] = Face.mIndices[2];
    }

    auto verticesBuffer = renderer::Buffer::create(w1.context(), renderer::BufferUsage::StaticDraw, numVertices*6*sizeof(float), vertices);
    auto indicesBuffer = renderer::Buffer::create(w1.context(), renderer::BufferUsage::StaticDraw, numIndices*sizeof(int), indices);

    auto vertexArray1 = renderer::VertexArray::create(w1.context());
    vertexArray1->bindVertexBuffer(0, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 0*sizeof(float));
    vertexArray1->bindVertexBuffer(1, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 3*sizeof(float));
    vertexArray1->bindIndexBuffer(indicesBuffer);

    auto vertexArray2 = renderer::VertexArray::create(w2.context());
    vertexArray2->bindVertexBuffer(0, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 0*sizeof(float));
    vertexArray2->bindVertexBuffer(1, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 3*sizeof(float));
    vertexArray2->bindIndexBuffer(indicesBuffer);

    auto vertexArray3 = renderer::VertexArray::create(w3.context());
    vertexArray3->bindVertexBuffer(0, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 0*sizeof(float));
    vertexArray3->bindVertexBuffer(1, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 3*sizeof(float));
    vertexArray3->bindIndexBuffer(indicesBuffer);

    auto vertexArray4 = renderer::VertexArray::create(w4.context());
    vertexArray4->bindVertexBuffer(0, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 0*sizeof(float));
    vertexArray4->bindVertexBuffer(1, verticesBuffer, 3, renderer::VertexArrayAttributePointerType::Type_32f, false, 6*sizeof(float), 3*sizeof(float));
    vertexArray4->bindIndexBuffer(indicesBuffer);

    w1.setProgram(program);
    w2.setProgram(program);
    w3.setProgram(program);
    w4.setProgram(program);

    w1.setVAO(vertexArray1, numIndices);
    w2.setVAO(vertexArray2, numIndices);
    w3.setVAO(vertexArray3, numIndices);
    w4.setVAO(vertexArray3, numIndices);

    QWidget w;
    QGridLayout l;
    l.addWidget(&w1, 0, 0);
    l.addWidget(&w2, 0, 1);
    l.addWidget(&w3, 1, 0);
    l.addWidget(&w4, 1, 1);
    w.setLayout(&l);

    w.show();
    w.resize(800, 600);

    return app.exec();
}
