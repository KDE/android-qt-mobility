DEPENDPATH += .
INCLUDEPATH += . \
    ../../../include \
    ../../../src/organizer \
    ../../../src/organizer/requests \
    ../../../src/organizer/details \
    ../../../src/organizer/items \
    ../../../src/organizer/filters \
    ../../../src/versit \
    ../../../src/versitorganizer

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(declarative_organizer)
TARGETPATH = QtMobility/organizer
DEFINES += QT_MAKEDLL
PLUGIN_TYPE = declarative

include(../../../common.pri)

# support headers/sources for dynamic properties
include(../common/dynamicproperties.pri)

QT += declarative
QT += script network


CONFIG += mobility
MOBILITY = organizer versit

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH



# Input
HEADERS += qdeclarativeorganizermodel_p.h \
           qdeclarativeorganizeritem_p.h \
           qdeclarativeorganizeritemdetail_p.h \
           qdeclarativeorganizeritemfilter_p.h \
           qdeclarativeorganizeritemmetaobject_p.h \
           qdeclarativeorganizerrecurrencerule_p.h \
           qdeclarativeorganizercollection_p.h \
           qdeclarativeorganizeritemsortorder_p.h \
           qdeclarativeorganizeritemfetchhint_p.h

SOURCES += plugin.cpp \
           qdeclarativeorganizeritem.cpp \
           qdeclarativeorganizeritemdetail.cpp \
           qdeclarativeorganizeritemmetaobject.cpp \
           qdeclarativeorganizermodel.cpp \
           qdeclarativeorganizeritemfilter.cpp \
           qdeclarativeorganizercollection.cpp \
           qdeclarativeorganizeritemsortorder.cpp \
           qdeclarativeorganizerrecurrencerule.cpp \
           qdeclarativeorganizeritemfetchhint.cpp

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

OTHER_FILES += qmldir

INSTALLS += target qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x20021326
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_organizer$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
