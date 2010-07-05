QT += testlib
TEMPLATE=app
TARGET=tst_qvcard30writer
CONFIG+=testcase

include(../../../common.pri)
DEFINES += QT_ASCII_CAST_WARNINGS

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit \
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters

HEADERS += tst_qvcard30writer.h
SOURCES += tst_qvcard30writer.cpp

CONFIG += mobility
MOBILITY = contacts versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
