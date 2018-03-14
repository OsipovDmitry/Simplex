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

	// Set the viewport
	glViewport ( 0, 0, width(), height() );

	// Clear the color buffer
	glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
	glClear ( GL_COLOR_BUFFER_BIT );

	// Use the program object
	m_context->bindProgram(m_program);
	m_context->bindVAO(m_vertexArray);

	//glDrawArrays ( GL_TRIANGLES, 0, 3 );
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	m_context->windowSurface()->swapBuffers();
}

void GLESWidget::closeEvent(QCloseEvent*)
{
	m_program.reset();
	m_vertexArray.reset();
	m_context.reset();
}
