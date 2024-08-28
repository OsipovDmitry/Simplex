TEMPLATE = subdirs

SUBDIRS += \
    utils \
    core \
    qt \
    openal \
    iblgenerator \
    app

core.depends = utils
qt.depends = utils core
openal.depends = utils core
iblgenerator.depends = utils
app.depends = utils core qt openal

HEADERS += \
    include/core/cubemap.h
