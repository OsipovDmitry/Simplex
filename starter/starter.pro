include(../include/build/build.pri)
TEMPLATE = app
QT += core gui widgets

SOURCES += \
	src/main.cpp

LIBS += \
	-llogger \
	-lrenderer
