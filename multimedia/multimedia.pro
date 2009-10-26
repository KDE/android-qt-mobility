TEMPLATE = lib

# distinct from QtMultimedia
TARGET = QtMedia

include (../common.pri)

QT += network

contains(QT_CONFIG, opengl): QT += opengl

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_MEDIA_LIB

HEADERS = \
    qmediacontrol.h \
    qmediacontrol_p.h \
    qmediaobject.h \
    qmediaobject_p.h \
    qmediaservice.h \
    qmediaservice_p.h  \
    qlocalmediaplaylistprovider.h \
    qmediaimageviewer.h \
    qmediaimageviewerservice_p.h \
    qmediaplayer.h \
    qmediaplayercontrol.h \
    qmediaplaylist.h \
    qmediaplaylist_p.h \
    qmediaplaylistnavigator.h \
    qmediaplaylistprovider.h \
    qmediaplaylistprovider_p.h \
    qmediaplaylistioplugin.h \
    qmediacontent.h \
    qmediaresource.h \
    qmediarecorder.h \
    qmediaencodersettings.h \
    qmediarecordercontrol.h \
    qmediaserviceprovider.h \
    qmediaserviceproviderplugin.h \
    qmetadatacontrol.h \
    qmediastreamscontrol.h \
    qmultimediaglobal.h \
    qradiotuner.h \
    qradiotunercontrol.h \
    qtmedianamespace.h \
    qvideooutputcontrol.h \
    qvideowidget.h \
    qvideowidget_p.h \
    qvideowindowcontrol.h \
    qvideowidgetcontrol.h \
    qaudioencodercontrol.h \
    qvideoencodercontrol.h \
    qimageencodercontrol.h \
    qimagecapturecontrol.h \
    qaudiocapturesource.h \
    qcamera.h \
    qcameracontrol.h \
    qcameraexposurecontrol.h \
    qcamerafocuscontrol.h \
    qimageprocessingcontrol.h \
    qmediaformatcontrol.h \
    qmediaplaylistcontrol.h \
    qaudiodevicecontrol.h \
    qvideodevicecontrol.h \
    qmediapluginloader_p.h

SOURCES = qmediacontrol.cpp \
    qmediaobject.cpp \
    qmediaservice.cpp \
    qlocalmediaplaylistprovider.cpp \
    qmediaimageviewer.cpp \
    qmediaimageviewerservice.cpp \
    qmediaplayer.cpp \
    qmediaplayercontrol.cpp \
    qmediaplaylist.cpp \
    qmediaplaylistioplugin.cpp \
    qmediaplaylistnavigator.cpp \
    qmediaplaylistprovider.cpp \
    qmediarecorder.cpp \
    qmediaencodersettings.cpp \
    qmediarecordercontrol.cpp \
    qmediacontent.cpp \
    qmediaresource.cpp \
    qmediaserviceprovider.cpp \
    qmetadatacontrol.cpp \
    qmediastreamscontrol.cpp \
    qradiotuner.cpp \
    qradiotunercontrol.cpp \
    qvideooutputcontrol.cpp \
    qvideowidget.cpp \
    qvideowindowcontrol.cpp \
    qvideowidgetcontrol.cpp \
    qaudioencodercontrol.cpp \
    qvideoencodercontrol.cpp \
    qimageencodercontrol.cpp \
    qimagecapturecontrol.cpp \
    qaudiocapturesource.cpp \
    qcamera.cpp \
    qcameracontrol.cpp \
    qcameraexposurecontrol.cpp \
    qcamerafocuscontrol.cpp \
    qimageprocessingcontrol.cpp \
    qmediaformatcontrol.cpp \
    qmediaplaylistcontrol.cpp \
    qaudiodevicecontrol.cpp \
    qvideodevicecontrol.cpp \
    qmediapluginloader.cpp

contains(QT_CONFIG, multimedia) {
    HEADERS += \
        qgraphicsvideoitem.h \
        qpaintervideosurface_p.h \
        qvideorenderercontrol.h

    SOURCES += \
        qgraphicsvideoitem.cpp \
        qpaintervideosurface.cpp \
        qvideorenderercontrol.cpp
} else {
    HEADERS += qaudioformat.h
    SOURCES += qaudioformat.cpp
}

symbian: {
    qtmobilitymultimedia.sources = QtMedia.dll
    qtmobilitymultimedia.path = "/sys/bin"    

    DEPLOYMENT += qtmobilitymultimedia    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
}