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
    src/vertexarray.cpp \
    src/texture.cpp \
    src/textureprivate.cpp \
    src/renderbuffer.cpp \
    src/framebuffer.cpp \
    src/framebufferprivate.cpp \
    src/bufferprivate.cpp

HEADERS += \
	../include/renderer/renderer_global.h \
    ../include/renderer/context.h \
    ../include/renderer/shader.h \
    ../include/renderer/program.h \
    ../include/renderer/buffer.h \
	../include/renderer/vertexarray.h \
    ../include/renderer/texture.h \
	../include/renderer/renderbuffer.h \
	src/shaderprivate.h \
	src/contextprivate.h \
	src/programprivate.h \
	src/bufferprivate.h \
	src/vertexarrayprivate.h \
	src/glutils.h \
	src/textureprivate.h \
    src/renderbufferprivate.h \
    ../include/renderer/framebuffer.h \
    src/framebufferprivate.h

LIBS += \
	-llogger \
	-lEGL \
	-lGLESv2
