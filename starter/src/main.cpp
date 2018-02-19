#include <QApplication>
#include <QWidget>

#include <logger/logger.h>
#include <renderer/context.h>

#include <iostream>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QWidget w;

	LOG_INFO("Hello, world!");
	auto c = renderer::Context::createContext(w.winId());

	w.show();

	return app.exec();
}
