TARGET = tst_qgallerytrackerschema
INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../src/gallery
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qgallerytrackerschema.cpp

CONFIG += mobility
MOBILITY = gallery

QT += dbus
