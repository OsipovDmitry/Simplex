include(../include/build/build.pri)

TEMPLATE = lib

DEFINES += \
	RENDERER_LIBRARY

SOURCES += \
    src/context.cpp \
    src/shader.cpp \
    src/program.cpp \
    src/buffer.cpp \
    src/contextprivate.cpp \
    src/vertexarray.cpp

HEADERS += \
	../include/renderer/renderer_global.h \
    ../include/renderer/context.h \
    src/checkglerror.h \
    src/shaderprivate.h \
    ../include/renderer/shader.h \
    ../include/renderer/program.h \
    ../include/renderer/buffer.h \
    src/contextprivate.h \
    src/programprivate.h \
    src/bufferprivate.h \
    ../include/renderer/vertexarray.h \
    src/vertexarrayprivate.h

LIBS += \
	-llogger \
	-lEGL \
	-lGLESv2
