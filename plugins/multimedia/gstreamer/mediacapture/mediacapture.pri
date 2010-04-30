INCLUDEPATH += $$PWD

DEFINES += QMEDIA_GSTREAMER_CAPTURE

HEADERS += $$PWD/qgstreamercaptureservice.h \
    $$PWD/qgstreamercapturesession.h \
    $$PWD/qgstreameraudioencode.h \
    $$PWD/qgstreamervideoencode.h \
    $$PWD/qgstreamerrecordercontrol.h \
    $$PWD/qgstreamermediacontainercontrol.h \
    $$PWD/qgstreamerv4l2input.h \
    $$PWD/qgstreamercapturemetadatacontrol.h

SOURCES += $$PWD/qgstreamercaptureservice.cpp \
    $$PWD/qgstreamercapturesession.cpp \
    $$PWD/qgstreameraudioencode.cpp \
    $$PWD/qgstreamervideoencode.cpp \
    $$PWD/qgstreamerrecordercontrol.cpp \
    $$PWD/qgstreamermediacontainercontrol.cpp \
    $$PWD/qgstreamerv4l2input.cpp \
    $$PWD/qgstreamercapturemetadatacontrol.cpp
