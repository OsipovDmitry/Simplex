include(../include/build/build.pri)

TEMPLATE = lib

DEFINES += \
	RENDERER_LIBRARY

SOURCES += \
    src/context.cpp \
    src/shader.cpp \
    src/program.cpp \
    src/buffer.cpp \
    src/vertexarray.cpp \
    src/texture.cpp \
    src/textureprivate.cpp \
    src/renderbuffer.cpp \
    src/framebuffer.cpp \
    src/framebufferprivate.cpp \
    src/bufferprivate.cpp \
    src/display.cpp \
    src/context_p.cpp \
    src/contextgroup_p.cpp \
    src/contextgroup.cpp

HEADERS += \
    ../include/renderer/renderer_global.h \
    ../include/renderer/forwarddecl.h \
    ../include/renderer/display.h \
    ../include/renderer/context.h \
    ../include/renderer/contextgroup.h \
    ../include/renderer/shader.h \
    ../include/renderer/program.h \
    ../include/renderer/buffer.h \
	../include/renderer/vertexarray.h \
    ../include/renderer/texture.h \
    ../include/renderer/renderbuffer.h \
    ../include/renderer/framebuffer.h \
    src/shaderprivate.h \
    src/programprivate.h \
    src/bufferprivate.h \
    src/vertexarrayprivate.h \
    src/glutils.h \
    src/textureprivate.h \
    src/renderbufferprivate.h \
    src/framebufferprivate.h \
    src/display_p.h \
    src/context_p.h \
    src/contextgroup_p.h

LIBS += \
    -llogger \
    -lEGL \
    -lGLESv2
