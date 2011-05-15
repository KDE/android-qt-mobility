TEMPLATE =      lib
CONFIG +=       plugin

TARGET =        $$qtLibraryTarget(qtmultimediakit_androidcam)
PLUGIN_TYPE =   mediaservice
include (../../../../common.pri)


CONFIG +=       mobility
MOBILITY +=     multimedia
QT = core network

include(camera.pri)

DEPENDPATH += .

INCLUDEPATH += . \
             $$SOURCE_DIR/src/multimedia \
             $$SOURCE_DIR/src/multimedia/video \
             $$SOURCE_DIR/src/multimedia/audio



SOURCES += \
    qandroidserviceplugin.cpp

HEADERS += \
    qandroidserviceplugin.h

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
