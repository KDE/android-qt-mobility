INCLUDEPATH += ../../../src/feedback
INCLUDEPATH += ../../../src/global

TEMPLATE = lib
CONFIG += plugin
TARGET  = $$qtLibraryTarget(declarative_feedback)
TARGETPATH = QtMobility/feedback
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

HEADERS += qdeclarativehapticseffect_p.h \
           qdeclarativefileeffect_p.h \
           qdeclarativethemeeffect_p.h \
           qdeclarativefeedbackactuator_p.h \
           qdeclarativefeedbackeffect_p.h
SOURCES += qdeclarativehapticseffect.cpp \
           qdeclarativefileeffect.cpp \
           plugin.cpp \
           qdeclarativethemeeffect.cpp \
           qdeclarativefeedbackactuator.cpp \
           qdeclarativefeedbackeffect.cpp

CONFIG += mobility
MOBILITY += feedback

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x200315FC
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_feedback$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
