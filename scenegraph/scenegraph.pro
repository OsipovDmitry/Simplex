include(../include/build/build.pri)

TEMPLATE = lib

DEFINES += \
    SCENEGARPH_LIBRARY

SOURCES += \
    src/node.cpp \
    src/scene.cpp \
    src/abstractsceneoptimizer.cpp

HEADERS += \
    ../include/scenegraph/node.h \
    ../include/scenegraph/scene.h \
    ../include/scenegraph/abstractsceneoptimizer.h \
    ../include/scenegraph/forwarddecl.h \
    ../include/scenegraph/scenegraph_global.h \
    src/abstractsceneoptimizer_p.h \
    src/node_p.h \
    src/scene_p.h

LIBS += \
    -llogger \
    -lrenderer
