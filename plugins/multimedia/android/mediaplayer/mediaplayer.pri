INCLUDEPATH += $$PWD

DEFINES += QMEDIA_GSTREAMER_PLAYER

HEADERS += \
    $$PWD/qandroidplayercontrol.h \
    $$PWD/qandroidplayerservice.h \
    $$PWD/qandroidplayersession.h \
    $$PWD/mediaPlayerJNI.h \
    $$PWD/qandroidmetadatacontrol.h


SOURCES += \
    $$PWD/qandroidplayercontrol.cpp \
    $$PWD/qandroidplayerservice.cpp \
    $$PWD/qandroidplayersession.cpp \
    $$PWD/mediaPlayerJNI.cpp \
    $$PWD/qandroidmetadatacontrol.cpp

