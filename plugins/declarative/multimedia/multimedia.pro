INCLUDEPATH += ../../../src/multimedia \
               ../../../src/multimedia/effects \
               ../../../src/multimedia/video
INCLUDEPATH += ../../../src/global

TEMPLATE = lib
CONFIG += plugin
TARGET  = $$qtLibraryTarget(declarative_multimedia)
TARGETPATH = QtMultimediaKit
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

HEADERS += \
        qdeclarativeaudio_p.h \
        qdeclarativemediabase_p.h \
        qdeclarativemediametadata_p.h \
        qdeclarativevideo_p.h \
        qdeclarativecamera_p.h \
        qdeclarativecamerapreviewprovider_p.h

SOURCES += \
        multimedia.cpp \
        qdeclarativeaudio.cpp \
        qdeclarativemediabase.cpp \
        qdeclarativevideo.cpp \
        qdeclarativecamera.cpp \
        qdeclarativecamerapreviewprovider.cpp

CONFIG += mobility
MOBILITY += multimedia

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x20021313
    TARGET.EPOCALLOWDLLDATA=1
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_multimedia$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
}
