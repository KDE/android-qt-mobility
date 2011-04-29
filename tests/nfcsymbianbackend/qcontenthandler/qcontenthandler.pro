QT       += testlib

TARGET = tst_qcontenthandler
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += mobility
CONFIG   += testcase
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
SOURCES += tst_qcontenthandler.cpp \
        ../common/qautomsgbox.cpp
symbian:TARGET.CAPABILITY = LocalServices ReadUserData WriteUserData NetworkServices UserEnvironment Location SwEvent ReadDeviceData WriteDeviceData
