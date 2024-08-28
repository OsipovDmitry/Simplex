include(../include/build/build.pri)
TEMPLATE = app

QT += widgets

HEADERS += \
    $$files("src/*.h", true)

SOURCES += \
    $$files("src/*.cpp", true)

FORMS += \
    $$files("src/*.ui", true)

LIBS += \
    -lutils \
    -lcore \
    -lqt \
    -lopenal

RESOURCES +=
