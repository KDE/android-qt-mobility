TEMPLATE =      lib
CONFIG +=       plugin

TARGET =        $$qtLibraryTarget(qtmultimediakit_androidcam)
PLUGIN_TYPE =   mediaservice
include (../../../../common.pri)


CONFIG +=       mobility
MOBILITY +=     multimedia
QT = core network multimedia

include(camera.pri)

DEPENDPATH += .

INCLUDEPATH += . \
             $$SOURCE_DIR/src/multimedia \
             $$SOURCE_DIR/src/multimedia/video \
             $$SOURCE_DIR/src/multimedia/audio \
             $$SOURCE_DIR/include/QtMultimediaKit



SOURCES += \
    qandroidserviceplugin.cpp

HEADERS += \
    qandroidserviceplugin.h

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
