TEMPLATE = app
TARGET = publish-subscribe

INCLUDEPATH += ../../src/publishsubscribe
DEPENDPATH += ../../src/publishsubscribe

symbian {
    crmlFiles.sources = example.qcrml
    crmlFiles.path = /resource/qt/crml
    DEPLOYMENT += crmlFiles

    #This is Symbian Signed UID3. Needs to match with uidValue in example.qcrml.
    TARGET.UID3 = 0x2002AC79
}

symbian|maemo* {
    DEFINES += QTM_SMALL_SCREEN
}

HEADERS = publisherdialog.h \
          subscriberdialog.h

SOURCES = main.cpp \
          publisherdialog.cpp \
          subscriberdialog.cpp

FORMS = publisherdialog.ui \
        subscriberdialog.ui

include(../examples.pri)

CONFIG += mobility
MOBILITY = publishsubscribe
