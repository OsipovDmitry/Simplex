#include <QDebug>
#include <QPaintEvent>

#include <GLES3/gl3.h>
#include "gleswidget.h"

#include <renderer/context.h>
#include <renderer/buffer.h>
#include <renderer/vertexarray.h>

GLESWidget::GLESWidget(QWidget *parent) : QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true); // for compatibility
}

void GLESWidget::paintEvent(QPaintEvent *pEvent)
{
	m_context->make();

//	GLint value;

//	GLuint bufs[2];
//	glGenBuffers(2, bufs);

//	GLuint vaos[2];
//	glGenVertexArrays(2, vaos);

//	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &value);

//	glBindVertexArray(vaos[0]);
//	glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

//	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &value);

//	glBindVertexArray(vaos[1]);
//	glBindBuffer(GL_ARRAY_BUFFER, bufs[1]);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

//	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &value);

//	glBindVertexArray(0);

//	glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &value);

//	glBindVertexArray(vaos[0]);
//	glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &value);

//	glBindVertexArray(vaos[1]);
//	glGetVertexAttribiv(0, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &value);

	GLfloat vVertices[] = {  0.0f,  0.5f, 1.0f, 0.0f, 0.0f,
							 -0.5f, -0.5f,0.0f, 1.0f, 0.0f,
							 0.5f, -0.5f, 0.0f, 0.0f, 1.0f
						  };
	GLuint indices[] = {0, 1, 2};

//	auto pIndexBuffer = m_context->createBuffer(renderer::BufferUsage::StaticDraw, 3*sizeof(GLuint), indices);
//	auto pVertexArray = m_context->createVertexArray();
//	pVertexArray->bindIndexBuffer(pIndexBuffer);


	// Set the viewport
	glViewport ( 0, 0, width(), height() );

	// Clear the color buffer
	glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear ( GL_COLOR_BUFFER_BIT );

	// Use the program object
	glUseProgram ( m_program->id() );

	// Load the vertex data
	glVertexAttribPointer ( 0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), vVertices );
	glEnableVertexAttribArray ( 0 );
	glVertexAttribPointer ( 1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), vVertices+2 );
	glEnableVertexAttribArray ( 1 );

	glDrawArrays ( GL_TRIANGLES, 0, 3 );

	m_context->windowSurface()->swapBuffers();
}
