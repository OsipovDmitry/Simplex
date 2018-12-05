#include <QDebug>
#include <QPaintEvent>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>

#include "gleswidget.h"

#include <logger/logger.h>
#include <types/image.h>
#include <renderer/display.h>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/shader.h>
#include <renderer/vertexarray.h>
#include <renderer/buffer.h>
#include <renderer/texture.h>
#include <renderer/renderbuffer.h>
#include <renderer/framebuffer.h>

namespace {

//const float vertices[] = { -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
//						   -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
//							0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
//							0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
//					  };
//const uint32_t indices[] = {0, 1, 2, 2, 3, 0};
unsigned int numIndices;

}

GLESWidget::GLESWidget(QWidget *parent, renderer::ContextGroupPtr shareGroup) : QWidget(parent)
{
	QFile modelFile(":/res/monkey.fbx");
	modelFile.open(QFile::ReadOnly);
	auto modelData = modelFile.readAll();

	Assimp::Importer Importer;
	auto pScene = Importer.ReadFileFromMemory(modelData.data(), modelData.size(), aiProcess_Triangulate);
	auto pMesh = pScene->mMeshes[0];

	float *vertices = new float[pMesh->mNumVertices * 8];
	unsigned int numVertices = pMesh->mNumVertices;
	for (int32_t i = 0; i < pMesh->mNumVertices; ++i) {
		vertices[8*i + 0] = pMesh->mVertices[i].x;
		vertices[8*i + 1] = pMesh->mVertices[i].y;
		vertices[8*i + 2] = pMesh->mVertices[i].z;
		vertices[8*i + 3] = pMesh->mNormals[i].x;
		vertices[8*i + 4] = pMesh->mNormals[i].y;
		vertices[8*i + 5] = pMesh->mNormals[i].z;
//		vertices[8*i + 6] = pMesh->mTextureCoords[0][i].x;
//		vertices[8*i + 7] = pMesh->mTextureCoords[0][i].y;
	}

	unsigned int *indices = new unsigned int[pMesh->mNumFaces * 3];
	numIndices = pMesh->mNumFaces * 3;
	for (int32_t i = 0 ; i < pMesh->mNumFaces ; i++) {
		const aiFace& Face = pMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		indices[3*i + 0] = Face.mIndices[0];
		indices[3*i + 1] = Face.mIndices[1];
		indices[3*i + 2] = Face.mIndices[2];
	}

	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);

    m_context = renderer::Context::createContext(winId(), 8, 8, 8, 8, 24, 8, shareGroup);

	glm::vec3 globalColor(0.6,0.5,0.3);
    m_uniformBuffer = renderer::Buffer::create(m_context, renderer::BufferUsage::StaticDraw, sizeof(glm::vec3), glm::value_ptr(globalColor));
    m_vertexArray = renderer::VertexArray::create(m_context);
	m_vertexArray->bindVertexBuffer(0,
                                    renderer::Buffer::create(m_context, renderer::BufferUsage::StaticDraw, numVertices*8*sizeof(float), vertices),
									2, renderer::VertexArrayAttributePointerType::Type_32f, false, 8*sizeof(float), 0*sizeof(float));
	m_vertexArray->bindVertexBuffer(1,
									m_vertexArray->vertexBuffer(0),
									3, renderer::VertexArrayAttributePointerType::Type_32f, false, 8*sizeof(float), 3*sizeof(float));
	m_vertexArray->bindVertexBuffer(2,
									m_vertexArray->vertexBuffer(0),
									2, renderer::VertexArrayAttributePointerType::Type_32f, false, 8*sizeof(float), 6*sizeof(float));
    m_vertexArray->bindIndexBuffer(renderer::Buffer::create(m_context, renderer::BufferUsage::StaticDraw, numIndices*sizeof(unsigned int), indices));

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

void GLESWidget::paintEvent(QPaintEvent *pEvent)
{
	m_framebuffer->clearColorBuffer(0, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	m_framebuffer->renderIndexedGeometry(m_program, m_vertexArray, renderer::PrimitiveType::Triangles, numIndices, renderer::GeometryIndexType::Type_32ui);

    auto pMainFramebuffer = renderer::Framebuffer::mainFramebuffer(m_context);
	pMainFramebuffer->setViewport(0, 0, width(), height());
	pMainFramebuffer->clearColorBuffer(0, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
	pMainFramebuffer->copyColorBuffer(glm::uvec4(10, 10, width()-10, height()-10), glm::uvec4(0, 0, 512, 512), m_framebuffer);

	m_context->windowSurface()->swapBuffers();
}
