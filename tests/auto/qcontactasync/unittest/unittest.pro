QT += testlib
TEMPLATE=app
TARGET=tst_qcontactasync
CONFIG+=testcase

PLUGIN_SUBDIR=dummyplugin/plugins

include(../../../../common.pri)

INCLUDEPATH += ../../../../contacts \
               ../../../../contacts/details \
               ../../../../contacts/requests \
               ../../../../contacts/filters

qtAddLibrary(QtContacts)

SOURCES  += tst_qcontactasync.cpp

# App local deployment
symbian:QCONTACTASYNC_PLUGINS_DEPLOY.sources = contacts_maliciousplugin.dll
wince*: QCONTACTASYNC_PLUGINS_DEPLOY.sources = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin/plugins/contacts/$$mobilityDeployFilename(contacts_maliciousplugin).dll

QCONTACTASYNC_PLUGINS_DEPLOY.path = ./plugins/contacts

DEPLOYMENT += QCONTACTASYNC_PLUGINS_DEPLOY

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
}
