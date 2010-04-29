INCLUDEPATH+=../../../src/sensors

include(../../../common.pri)
include(s60_sensor_api.pri)
include(version.pri)

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(qtsensors_s60sensorapi)
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002BFC1
TARGET.CAPABILITY = ALL -TCB

QT=core
CONFIG+=mobility
MOBILITY+=sensors

#S60 v3.1 sensor back end deployment
s60sensorapi.sources = $${TARGET}.dll
s60sensorapi.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
DEPLOYMENT += s60sensorapi

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
maemo6:target.path=$$[QT_INSTALL_PLUGINS]/$${PLUGIN_TYPE}
INSTALLS += target
