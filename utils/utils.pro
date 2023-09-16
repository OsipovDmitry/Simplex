include(../include/build/build.pri)
TEMPLATE = lib

QT +=

DEFINES += \
    UTILS_LIBRARY

HEADERS += \
    $files("../include/utils/glm/*", true) \
    $files("../include/utils/rapidjson/*", true) \
    $$files("../include/utils/*.h", false) \
    $files("src/stb/*.", true) \
    $$files("src/*.h", false)

SOURCES += \
    $$files("src/*.cpp", true)

LIBS +=

RESOURCES +=
