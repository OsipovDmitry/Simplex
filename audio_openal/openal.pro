include(../include/build/build.pri)
TEMPLATE = lib

DEFINES += \
    OPENAL_LIBRARY

INCLUDEPATH += \
    "D:/OpenAL 1.1 SDK/include"

HEADERS += \
    $$files("../include/openal/*.h", true) \
    $$files("src/*.h", true)

SOURCES += \
    $$files("src/*.cpp", true)

LIBS += \
    -lutils \
    -lcore \
    -L"D:/OpenAL 1.1 SDK/libs/Win64" \
    -lOpenAL32

RESOURCES +=
