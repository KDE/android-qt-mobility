TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtfeedback_android)
PLUGIN_TYPE = feedback


include(../../../common.pri)
include(android.pri)
CONFIG += mobility
MOBILITY = feedback
QT = core network

INCLUDEPATH += $$SOURCE_DIR/src/feedback

