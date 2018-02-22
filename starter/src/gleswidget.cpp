#include <QDebug>
#include <QPaintEvent>

#include <GLES3/gl3.h>
#include "gleswidget.h"

GLESWidget::GLESWidget(QWidget *parent) : QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true); // for compatibility
}

void GLESWidget::paintEvent(QPaintEvent *pEvent)
{
	renderer::Context::makeContextCurrent(m_context);

	GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
							 -0.5f, -0.5f, 0.0f,
							 0.5f, -0.5f, 0.0f
						  };

	// Set the viewport
	glViewport ( 0, 0, width(), height() );

	// Clear the color buffer
	glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear ( GL_COLOR_BUFFER_BIT );

	// Use the program object
	glUseProgram ( m_program->id() );

	// Load the vertex data
	glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
	glEnableVertexAttribArray ( 0 );

	glDrawArrays ( GL_TRIANGLES, 0, 3 );

	m_context->swapBuffers();
}
