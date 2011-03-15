INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
INCLUDEPATH += ../../../src/global

TEMPLATE = lib
CONFIG += plugin
TARGET  = $$qtLibraryTarget(declarative_publishsubscribe)
TARGETPATH = QtMobility/publishsubscribe
PLUGIN_TYPE = declarative
include(../../../common.pri)

# support headers/sources for dynamic properties
include(../common/dynamicproperties.pri)

QT += declarative

SOURCES += publishsubscribe.cpp \
    qdeclarativevaluespacepublisher.cpp \
    qdeclarativevaluespacepublishermetaobject.cpp \
    qdeclarativevaluespacesubscriber.cpp

HEADERS += \
    qdeclarativevaluespacepublisher_p.h \
    qdeclarativevaluespacepublishermetaobject_p.h \
    qdeclarativevaluespacesubscriber_p.h

CONFIG += mobility
MOBILITY += publishsubscribe

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
    TARGET.UID3 = 0x20021322
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_publishsubscribe$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
