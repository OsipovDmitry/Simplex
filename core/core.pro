include(../include/build/build.pri)
TEMPLATE = lib

QT +=

DEFINES += \
    CORE_LIBRARY

HEADERS += \
    $$files("../include/core/*.h", true) \
    $$files("src/*.h", true)

SOURCES += \
    $$files("src/*.cpp", true)

LIBS += \
    -lutils
