INCLUDEPATH+=../../../src/sensors
INCLUDEPATH+=. \
/root/Desktop/android-ndk-r5/platforms/android-8/arch-arm/usr/include/

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
CONFIG += link_pkgconfig
PKGCONFIG += sensord

CONFIGFILES.files = Sensors.conf
CONFIGFILES.path = /etc/xdg/Nokia/
INSTALLS += CONFIGFILES
