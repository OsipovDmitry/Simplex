#ifndef GLESWIDGET_H
#define GLESWIDGET_H

#include <QWidget>
#include <renderer/context.h>
#include <GLES3/gl3.h>

class GLESWidget : public QWidget
{
	Q_OBJECT
public:
	explicit GLESWidget(QWidget *parent = nullptr);

	void init();
	void setContext(renderer::ContextPtr context) {m_context = context;}

protected:
	void paintEvent(QPaintEvent*);

private:
	renderer::ContextPtr m_context;

	GLuint m_programId;
};

#endif // GLESWIDGET_H
