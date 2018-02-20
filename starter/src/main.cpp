#include <QApplication>
#include <QWidget>

#include <renderer/context.h>

#include "gleswidget.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	GLESWidget w1;
	auto c1 = renderer::Context::createContext(w1.winId());
	w1.init();
	w1.setContext(c1);
	w1.show();

//	GLESWidget w2;
//	auto c2 = renderer::Context::createContext(w2.winId(), 8,8,8,0,24,8, c1);
//	w2.init();
//	w2.setContext(c2);
//	w2.show();


	return app.exec();
}
