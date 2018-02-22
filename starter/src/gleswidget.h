#ifndef GLESWIDGET_H
#define GLESWIDGET_H

#include <QWidget>
#include <renderer/context.h>
#include <renderer/program.h>
#include <renderer/shader.h>

class GLESWidget : public QWidget
{
	Q_OBJECT
public:
	explicit GLESWidget(QWidget *parent = nullptr);

	void setContext(renderer::ContextPtr context) {m_context = context;}
	void setProgram(renderer::ProgramPtr program) {m_program = program;}

protected:
	void paintEvent(QPaintEvent*);

private:
	renderer::ContextPtr m_context;
	renderer::ProgramPtr m_program;
};

#endif // GLESWIDGET_H
