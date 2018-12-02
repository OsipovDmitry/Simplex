#include <QApplication>
#include "gleswidget.h"


int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	GLESWidget w1;
	w1.setWindowTitle("Window 1");
	w1.show();

	return app.exec();
}
