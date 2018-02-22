include(../include/build/build.pri)

TEMPLATE = lib

DEFINES += \
	RENDERER_LIBRARY

SOURCES += \
    src/context.cpp \
    src/shader.cpp \
    src/program.cpp

HEADERS += \
	../include/renderer/renderer_global.h \
    ../include/renderer/context.h \
    src/checkglerror.h \
    src/shaderprivate.h \
    ../include/renderer/shader.h \
    ../include/renderer/program.h

LIBS += \
	-llogger \
	-lEGL \
	-lGLESv2
