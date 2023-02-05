include(../include/build/build.pri)
TEMPLATE = lib

QT +=

DEFINES += \
    UTILS_LIBRARY

HEADERS += \
    $$files("../include/utils/glm/*", true) \
    $$files("../include/utils/*.h", true)

SOURCES += \
    $$files("src/*.cpp", true)

LIBS +=

RESOURCES +=
