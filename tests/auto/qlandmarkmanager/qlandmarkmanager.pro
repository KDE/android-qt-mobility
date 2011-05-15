TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkmanager

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/landmarks \
                ../


# Input
SOURCES += tst_qlandmarkmanager.cpp

HEADERS += ../qlandmarkmanagerdataholder.h

!symbian:!maemo6:!meego {
    QT += sql
}
QT += testlib

maemo6|meego {
    DEFINES+=SPARQL_BACKEND
}

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
        load(data_caging_paths)
        INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
        TARGET.EPOCALLOWDLLDATA = 1
        TARGET.CAPABILITY = LocalServices ReadUserData WriteUserData ReadDeviceData WriteDeviceData NetworkServices
        LIBS += -leposlmdbmanlib
        addFiles.sources += data/moreplaces.lmx
        addFiles.sources += data/test.gpx
        addFiles.sources += data/convert-collection-in.xml
        addFiles.sources += data/file.omg
        addFiles.sources += data/garbage.xml
        addFiles.sources += data/AUS-PublicToilet-NewSouthWales.lmx
        addFiles.sources += data/AUS-PublicToilet-NewSouthWales.gpx
        addFiles.sources += data/AUS-PublicToilet-AustralianCapitalTerritory.gpx
        addFiles.sources += data/AUS-PublicToilet-AustralianCapitalTerritory.lmx
        addFiles.sources += data/places.gpx
        addFiles.sources += data/test.kml
        addFiles.sources += data/malformed.kml
        addFiles.sources += data/test.kmz
        addFiles.sources += data/malformed.kmz
        addFiles.path = data
        DEPLOYMENT += addFiles
}

