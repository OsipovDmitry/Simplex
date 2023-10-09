TEMPLATE = subdirs

SUBDIRS += \
    utils \
    core \
    qt \
    iblgenerator \
    app

core.depends = utils
qt.depends = utils core
iblgenerator.depends = utils
app.depends = utils core qt

HEADERS += \
    include/core/cubemap.h
