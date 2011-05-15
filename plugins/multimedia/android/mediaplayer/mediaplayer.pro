
TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmultimediakit_androidmedia)
PLUGIN_TYPE=mediaservice
include (../../../../common.pri)

INCLUDEPATH+= . \
             $$SOURCE_DIR/src/multimedia \
             $$SOURCE_DIR/src/multimedia/video \
             $$SOURCE_DIR/src/multimedia/audio \
             ../core/include/ \
             ../base/include/ \
             ../hardware/include/ \
             ../ \
             ../ffmpeg \
             $$SOURCE_DIR/include/QtMultimediaKit

LIBS += -lQtCore -lQtMultimediaKit -L../froyolibs -lcutils -lutils -lmedia -lEGL -L$$SOURCE_DIR/plugins/multimedia/android/ffmpeglibs -lffmpeg


DEPENDPATH += .
HEADERS = \
    qandroidserviceplugin.h \
    qandroidplayercontrol.h \
    qandroidplayerservice.h \
    qandroidplayersession.h \
    mediaPlayerJNI.h \
    qandroidmetadatacontrol.h \
    qandroidvideowidget.h \
    qandroidvideothread.h


SOURCES = \
    qandroidserviceplugin.cpp \
    qandroidplayercontrol.cpp \
    qandroidplayerservice.cpp \
    qandroidplayersession.cpp \
    mediaPlayerJNI.cpp \
    qandroidmetadatacontrol.cpp \
    qandroidvideowidget.cpp \
    qandroidvideothread.cpp

target.path=$$QT_MOBILITY_PREFIX/plugins/mediaservice
INSTALLS += target
CONFIG += mobility
MOBILITY = multimedia

