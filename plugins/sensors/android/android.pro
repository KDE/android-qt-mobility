INCLUDEPATH+=../../../src/sensors
INCLUDEPATH+=.

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_android)

include(version.pri)
include(android.pri)
include(../../../common.pri)



QT = core network
CONFIG += mobility
MOBILITY += sensors
