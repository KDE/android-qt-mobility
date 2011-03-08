TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_android)
PLUGIN_TYPE=mediaservice

include(../../../common.pri)
INCLUDEPATH+=$$SOURCE_DIR/src/multimedia \
             $$SOURCE_DIR/src/multimedia/video \
             $$SOURCE_DIR/src/multimedia/audio \
             $$SOURCE_DIR/plugins/multimedia/android/core/include/ \
             $$SOURCE_DIR/plugins/multimedia/android/base/include/

LIBS += -lQtCore -lQtMultimediaKit -L$$SOURCE_DIR/plugins/multimedia/android/froyolibs -lcutils -lutils -lmedia -lEGL \


DEPENDPATH += .
HEADERS = \
    qandroidserviceplugin.h


SOURCES = \
    qandroidserviceplugin.cpp


target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS += target
CONFIG += mobility
MOBILITY = multimedia
include(mediaplayer/mediaplayer.pri)
