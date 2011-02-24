TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qgstengine)
PLUGIN_TYPE=mediaservice

include(../../../common.pri)
INCLUDEPATH+=../../../src/multimedia \
             ../../../src/multimedia/video \
             ../../../src/multimedia/audio

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

unix:contains(QT_CONFIG, alsa) {
DEFINES += HAVE_ALSA
LIBS += \
    -lasound
}

CONFIG += link_pkgconfig

PKGCONFIG += \
    gstreamer-0.10 \
    gstreamer-base-0.10 \
    gstreamer-interfaces-0.10 \
    gstreamer-audio-0.10 \
    gstreamer-video-0.10

maemo*:PKGCONFIG +=gstreamer-plugins-bad-0.10

maemo5 {
  HEADERS += camerabuttonlistener_maemo.h
  SOURCES += camerabuttonlistener_maemo.cpp

  QT += dbus
}

maemo6 {
    HEADERS += camerabuttonlistener_meego.h
    SOURCES += camerabuttonlistener_meego.cpp

    PKGCONFIG += qmsystem2 libresourceqt1
}

# Input
HEADERS += \
    qgstreamermessage.h \
    qgstreamerbushelper.h \
    qgstreamervideorendererinterface.h \
    qgstreamerserviceplugin.h \
    qgstreameraudioinputendpointselector.h \
    qgstreamervideorenderer.h \
    qgstvideobuffer.h \
    qvideosurfacegstsink.h \
    qgstreamervideoinputdevicecontrol.h \
    gstvideoconnector.h \
    qabstractgstbufferpool.h

SOURCES += \
    qgstreamermessage.cpp \
    qgstreamerbushelper.cpp \
    qgstreamervideorendererinterface.cpp \
    qgstreamerserviceplugin.cpp \
    qgstreameraudioinputendpointselector.cpp \
    qgstreamervideorenderer.cpp \
    qgstvideobuffer.cpp \
    qvideosurfacegstsink.cpp \
    qgstreamervideoinputdevicecontrol.cpp \
    gstvideoconnector.c


!win32:!embedded:!mac:!symbian:!simulator:!contains(QT_CONFIG, qpa) {
    LIBS += -lXv -lX11 -lXext

    HEADERS += \
        qgstreamervideooverlay.h \
        qgstreamervideowindow.h \
        qgstreamervideowidget.h \
        qx11videosurface.h \
        qgstxvimagebuffer.h

    SOURCES += \
        qgstreamervideooverlay.cpp \
        qgstreamervideowindow.cpp \
        qgstreamervideowidget.cpp \
        qx11videosurface.cpp \
        qgstxvimagebuffer.cpp
}
include(mediaplayer/mediaplayer.pri)
include(mediacapture/mediacapture.pri)

contains(gstreamer-photography_enabled, yes) {
    include(camerabin/camerabin.pri)
}
