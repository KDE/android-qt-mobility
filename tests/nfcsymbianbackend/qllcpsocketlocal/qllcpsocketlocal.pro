
QT       += testlib

TARGET = tst_qllcpsocketlocal
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += testcase
CONFIG   += mobility
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
DEPENDPATH += ../common

INCLUDEPATH += ../../../src/global
DEPENDPATH += ../../../src/global

QMAKE_LIBDIR += $$QT_MOBILITY_BUILD_TREE/lib

HEADERS += ../common/qautomsgbox.h
SOURCES += tst_qllcpsocketlocal.cpp \
        ../common/qautomsgbox.cpp
symbian:TARGET.CAPABILITY = ALL -TCB
