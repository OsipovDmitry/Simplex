include(../include/build/build.pri)
TEMPLATE = lib
CONFIG += staticlib

QT += core

DEFINES +=

HEADERS += \
    $$files("../include/qt/*", true)

SOURCES +=

LIBS +=

RESOURCES +=
