include(../include/build/build.pri)
TEMPLATE = lib
CONFIG += staticlib

QT +=

DEFINES +=

HEADERS += \
    $$files("../include/core/*.h", true) \
    $$files("src/*.h", true)

SOURCES += \
    $$files("src/*.cpp", true)

LIBS += \
    -lutils

RESOURCES +=
