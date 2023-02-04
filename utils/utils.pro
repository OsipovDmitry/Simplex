include(../include/build/build.pri)
TEMPLATE = lib
CONFIG += staticlib

QT +=

DEFINES +=

HEADERS += \
    $$files("../include/utils/glm/*", true) \
    $$files("../include/utils/*", true)

SOURCES +=

LIBS +=

RESOURCES +=
