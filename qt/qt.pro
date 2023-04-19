include(../include/build/build.pri)
TEMPLATE = lib

QT += widgets opengl openglwidgets

DEFINES += \
    QT_LIBRARY

HEADERS += \
    $$files("../include/qt/*.h", true) \
    $$files("src/*.h", true)

SOURCES += \
    $$files("src/*.cpp", true)

LIBS += \
    -lutils \
    -lcore

RESOURCES +=
