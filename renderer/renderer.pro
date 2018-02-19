include(../include/build/build.pri)

TEMPLATE = lib

DEFINES += \
	RENDERER_LIBRARY

SOURCES += \
    src/context.cpp

HEADERS += \
	../include/renderer/renderer_global.h \
    ../include/renderer/context.h

LIBS += \
	-lGLESv2
	-lEGL
