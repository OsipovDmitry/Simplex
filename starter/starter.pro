include(../include/build/build.pri)
TEMPLATE = app
QT += core gui widgets

SOURCES += \
	src/main.cpp \
    src/gleswidget.cpp

LIBS += \
	-llogger \
	-lrenderer \
    -lscenegraph \
	-lassimp

HEADERS += \
    src/gleswidget.h

RESOURCES += \
    starter.qrc
