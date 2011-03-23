TEMPLATE = app
TARGET = weatherinfo_with_location

HEADERS = ../lightmaps/satellitedialog.h \
            ../../examples/flickrdemo/connectivityhelper.h
SOURCES = weatherinfo.cpp \
            ../lightmaps/satellitedialog.cpp \
            ../../examples/flickrdemo/connectivityhelper.cpp

RESOURCES = weatherinfo.qrc
QT += network svg

include(../demos.pri)

CONFIG += mobility
MOBILITY = location

equals(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7) {
    MOBILITY += bearer
    INCLUDEPATH += ../../src/bearer
} else {
    # use Bearer Management classes in QtNetwork module
    DEFINES += BEARER_IN_QTNETWORK
}

INCLUDEPATH += ../../src/global \
                ../../src/location \
                ../lightmaps \
                ../../examples/flickrdemo

symbian {
    symbian {
        addFiles.sources = nmealog.txt
        DEPLOYMENT += addFiles
        TARGET.CAPABILITY += Location \
                NetworkServices \
                ReadUserData
    }
    wince* {
        addFiles.sources = ./nmealog.txt
        addFiles.path = .
        DEPLOYMENT += addFiles
    }
} else {
    logfile.path = $$QT_MOBILITY_DEMOS
    logfile.files = nmealog.txt
    INSTALLS += logfile
}
