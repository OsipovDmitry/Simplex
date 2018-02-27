include(../include/build/build.pri)

TEMPLATE = lib

DEFINES += \
	LOGGER_LIBRARY

SOURCES += \
	src/logger.cpp

HEADERS += \
	../include/logger/logger.h \
	../include/logger/logger_global.h \
    ../include/logger/abstractlog.h
