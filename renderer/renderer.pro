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
    src/renderbuffer.cpp \
    src/framebuffer.cpp \
    src/framebufferprivate.cpp \
    src/display.cpp \
    src/context_p.cpp \
    src/contextgroup_p.cpp \
    src/contextgroup.cpp \
    src/buffer_p.cpp \
    src/texture_p.cpp

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
    src/vertexarrayprivate.h \
    src/glutils.h \
    src/framebufferprivate.h \
    src/display_p.h \
    src/context_p.h \
    src/contextgroup_p.h \
    src/buffer_p.h \
    src/shader_p.h \
    src/program_p.h \
    src/texture_p.h \
    src/renderbuffer_p.h

LIBS += \
    -llogger \
    -lEGL \
    -lGLESv2
