#ifndef GLESWIDGET_H
#define GLESWIDGET_H

#include <QWidget>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/shader.h>
#include <renderer/vertexarray.h>
#include <renderer/buffer.h>

class GLESWidget : public QWidget
{
	Q_OBJECT
public:
	explicit GLESWidget(QWidget *parent = nullptr);

	void setContext(renderer::ContextPtr context) {m_context = context;}
	void setProgram(renderer::ProgramPtr program) {m_program = program;}
	void setVertexArray(renderer::VertexArrayPtr vArray) {m_vertexArray = vArray;}

protected:
	void paintEvent(QPaintEvent*);
	void closeEvent(QCloseEvent*);

private:
	renderer::ContextPtr m_context;
	renderer::ProgramPtr m_program;
	renderer::VertexArrayPtr m_vertexArray;
};

#endif // GLESWIDGET_H
