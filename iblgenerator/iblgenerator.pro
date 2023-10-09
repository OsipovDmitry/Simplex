include(../include/build/build.pri)
TEMPLATE = app

QT +=

HEADERS += \
    $$files("src/*.h", true)

SOURCES += \
    $$files("src/*.cpp", true)

LIBS += \
    -lutils
