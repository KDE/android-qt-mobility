TEMPLATE = lib
CONFIG += plugin
QT += sql
TARGET = $$qtLibraryTarget(qtlandmarks_sqlite)
PLUGIN_TYPE=landmarks

include(../../../common.pri)

HEADERS += qlandmarkmanagerengine_sqlite_p.h \
           qlandmarkmanagerenginefactory_sqlite_p.h \
           qlandmarkfilehandler_lmx_p.h \
           qlandmarkfilehandler_gpx_p.h 



SOURCES += qlandmarkmanagerengine_sqlite.cpp \
           qlandmarkmanagerenginefactory_sqlite.cpp \
           qlandmarkfilehandler_lmx.cpp \
           qlandmarkfilehandler_gpx.cpp 


INCLUDEPATH += $$SOURCE_DIR/src/location

RESOURCES += qlandmarkmanagerengine_sqlite.qrc

target.path=$$QT_MOBILITY_PREFIX/plugins/landmarks
INSTALLS += target
CONFIG += mobility
MOBILITY = location
