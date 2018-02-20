include(../include/build/build.pri)

TEMPLATE = lib

DEFINES += \
	RENDERER_LIBRARY

SOURCES += \
    src/context.cpp \
    src/shader.cpp

HEADERS += \
	../include/renderer/renderer_global.h \
    ../include/renderer/context.h \
    ../include/renderer/shader.h \
    src/checkglerror.h

LIBS += \
	-llogger \
	-lEGL \
	-lGLESv2
